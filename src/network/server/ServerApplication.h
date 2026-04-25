//ServerApplication class handles all data coming into the server

// gets data from either a new connection, or an existing user
// - new connection: it strips the packet down into a game event and sends it to the session manager
// - existing user sends data: it strips the packet down into a game event and sends it to the session manager
// - also has the ability to send packets to the clients from

#pragma once

#include <vector>
#include <cstring>
#include <unordered_map>
#include <cstdint>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h> // for struct pollfd
#include <sys/socket.h>
#include <sys/types.h>
#include "SessionManager.h"
#include "TableManager.h"
#include "../PacketBuilder.h"
#include "../PacketParser.h"
#include "../../core/Debug.h"
#include "../../core/GameEvents.h"


class ServerApplication
{
public:
  //Sets up the server and prepares it to listen on the given port
  // ServerApplication constructor has
  // - m_tableManager // to manage the tables
  // - m_sessionManager (to manage the session)
  ServerApplication(const char* port) 
    : m_port(port),
      m_tableManager(), // Initialize the TableManager first
      m_sessionManager( // Initialize the SessionManager class here
          [this](int fd, const GameEvent& event){
            this->sendEventToClient(fd, event);
          },
          m_tableManager    // 2. Pass it as the second argument here!
      ) // lambda function for sendEventToClient for when
  {
  }

  ~ServerApplication() 
  {
    stop();
    // delete m_tableManager;
  }

  //runs the server
  //checks the data being sent in if a user sent data or a new connection happens
  void run()
  {
    TRACE_FUNCTION();
    if (!setupServer())
    {
      std::cerr << "Failed to start server on port" << m_port << "\n";
      return;
    }

    m_isRunning = true;
    DEBUG_PRINT << "Server is running....\n";

    while (m_isRunning)
    {
      //eventually switch to epoll
      //also 100 is a delay to set the speed of the server, -1 to set it to trigger when data arrives, 100 sets the delay to trigger every 100 ms
      int activity = poll(m_pollFds.data(), m_pollFds.size(), 100);
      if (activity < 0) 
      {
        perror("poll error");
        break;
      }

      // loop through the m_pollFds vector (struct of pollfds) which hold each of the clients
      // this loop figures out what was sent through poll
      for (int i = m_pollFds.size() - 1; i >= 0; i--)
      {
        // POLLIN: flag saying there is data waiting to be read IN to the server
        if ((m_pollFds[i].revents & POLLIN) == 0) //revents checks what actually happened
        {
          continue; // go to next loop iteration and skip everything below
        }

        if (m_pollFds[i].fd == m_serverSocket) // the server socket has info, this info means a player wants to connect
        {
          //so we accept them here
          acceptNewClient();
        }
        else
        {
          DEBUG_PRINT << "\n";
          //if its not from the server, its a gameEvent so we handle that data using recvClientData() with the index
          recvClientData(i);
        }
      }
    }
  }

  //Breaks the infinite loop in run(), allowing the server to shut down gracefully.
  void stop()
  {
    TRACE_FUNCTION();
    m_isRunning = false;
    for (auto& pfd : m_pollFds)
    {
      close(pfd.fd);
    }
    m_pollFds.clear();
    m_clientBuffers.clear();
  }

  //sends a packet (thats made using PacketBuilder.h) through the socket
  //handles partial sends
  int sendAll(int sockfd, const uint8_t *buf, size_t &len) 
  {
    TRACE_FUNCTION();
    size_t total = 0;
    size_t bytes_left = len;
    ssize_t n = 0;

    while (total < len) 
    {
      n = send(sockfd, buf + total, bytes_left, 0);
      if (n == -1) { break; }
      total += n;
      bytes_left -= n;
    }

    len = total;
    return n == -1 ? -1 : 0;
  }



private:
  bool setupServer()
  {
    TRACE_FUNCTION();
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, m_port, &hints, &res) != 0)
    {
      perror("getaddrinfo");
      return false;
    }

    m_serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (m_serverSocket == -1)
    {
      perror("socket");
      freeaddrinfo(res);
      return false;
    }

    int yes = 1;
    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
      perror("setsockopt");
      close(m_serverSocket);
      freeaddrinfo(res);
      return false;
    }

    if (bind(m_serverSocket, res->ai_addr, res->ai_addrlen) == -1)
    {
      perror("bind");
      close(m_serverSocket);
      freeaddrinfo(res);
      return false;
    }

    freeaddrinfo(res);

    // start listening. allow up to 10 pending connections in the OS backlog.
    if (listen(m_serverSocket, 10) == -1)
    {
      perror("listen");
      close(m_serverSocket);
      return false;
    }

    // add server socket to poll

    // register the main server socket with our polling system so we know when people connect
    pollfd server_pollfd;
    server_pollfd.fd = m_serverSocket;
    server_pollfd.events = POLLIN; // we care about incoming data/connections
    server_pollfd.revents = 0;
    m_pollFds.push_back(server_pollfd);

    return true;
  }

  // accepts a new incoming TCP connection and sets up their buffer.
  void acceptNewClient()
  {
    TRACE_FUNCTION();
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);

    int new_fd = accept(m_serverSocket, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd == -1)
    {
      perror("accept");
    }
    else
    {
      DEBUG_PRINT << "  adding FD: " << new_fd << " to m_pollFds\n";

      pollfd client_pollfd;
      client_pollfd.fd = new_fd;
      client_pollfd.events = POLLIN;
      client_pollfd.revents = 0;
      m_pollFds.push_back(client_pollfd);

      //initialize an empty vector buffer for this client
      m_clientBuffers[new_fd] = std::vector<uint8_t>();

      m_sessionManager.onClientConnected(new_fd);
    }
  }

  // Reads raw incoming bytes from a specific player, puts them in their personal buffer,
  // and slices those bytes into complete 'GameEvents' (The Framing Loop).
  void recvClientData(size_t pollIndex) 
  {
    TRACE_FUNCTION();
    int client_fd = m_pollFds[pollIndex].fd;
    uint8_t buf[1024]; // temp buffer to catch incoming OS bytes

    ssize_t bytes_received = recv(client_fd, buf, sizeof(buf), 0);

    if (bytes_received <= 0)
    {
      disconnectClient(pollIndex);
    }
    else
    {
      // 1. Grab this client's personal binary waiting room
      auto& buffer = m_clientBuffers[client_fd];

      // 2. Safely append the newly received bytes to the end of their vector
      buffer.insert(buffer.end(), buf, buf + bytes_received);

      // 3. THE FRAMING LOOP
      // We need AT LEAST 2 bytes to even know how big the packet is supposed to be.
      while (buffer.size() >= 2) 
      {
        // A. Read the first 2 bytes (The Header)
        uint16_t network_size;
        memcpy(&network_size, buffer.data(), sizeof(uint16_t));

        // B. Convert it from Network format back to a normal integer
        uint16_t payload_size = ntohs(network_size);

        // C. Do we have the entire packet yet? (2 header bytes + the payload size)
        if (buffer.size() < 2 + payload_size) 
        {
          // We only received half the packet. Break the loop and wait 
          // for poll() to wake us up again when the rest of the bytes arrive!
          break; 
        }

        // --- WE GOT A 100% COMPLETE BINARY PACKET! ---

        // D. Extract just the payload (skip the first 2 bytes)
        std::vector<uint8_t> completePacket(
          buffer.begin() + 2, 
          buffer.begin() + 2 + payload_size
        );

        // E. Erase the processed packet from the front of the waiting room
        buffer.erase(buffer.begin(), buffer.begin() + 2 + payload_size);


        // F. Pass the pure binary payload up to Layer 3!
        //
        // [LAYER 3 HOOK]: m_tableManager->routePacket(client_fd, completePacket);
        // 1. OPEN THE BOX: Ask the PacketParser to turn the raw bytes into a GameEvent
        GameEvent receivedEvent = PacketParser::parse(completePacket);

        // 2. PASS TO LOGIC: Hand the neat struct to the SessionManager!
        m_sessionManager.handleClientMessage(client_fd, receivedEvent);
      }
    }
  }

  void disconnectClient(size_t pollIndex) 
  {
    TRACE_FUNCTION();
    int client_fd = m_pollFds[pollIndex].fd;
    DEBUG_PRINT << "Client FD: " << client_fd << " disconnected." << "\n";

    m_sessionManager.onClientDisconnected(client_fd);

    close(client_fd);
    m_clientBuffers.erase(client_fd);
    m_pollFds.erase(m_pollFds.begin() + pollIndex);
  }



  // A helper function that takes a high-level GameEvent, packs it into a binary packet 
  // using PacketBuilder, and shoots it over the network using sendAll().
  //sendEventToClient - builds a packet and sends it 
  void sendEventToClient(int client_fd, const GameEvent& event)
  {
    TRACE_FUNCTION();
    DEBUG_PRINT << "SENDING EVENT " << event << "\n";

    PacketBuilder builder;
    builder.append8(static_cast<uint8_t>(event.type));
    builder.appendString(event.senderUUID.c_str());
    builder.appendString(event.senderUsername.c_str()); 
    builder.append32(event.intPayload);
    builder.appendString(event.stringPayload.c_str());
    builder.finalize(); 

    size_t len = builder.getSize();

    ssize_t bytesSent = sendAll(client_fd, builder.getPtr(), len);
    if (bytesSent < 0) {
      std::cerr << "[NETWORK] Failed to send response to FD " << client_fd << "\n";
    }
  }

  const char* m_port;
  int m_serverSocket; //The master socket ID that the server uses to listen for BRAND NEW connections.
  bool m_isRunning;

  // A list of every active connection (including the master server socket).
  // The OS 'poll()' function uses this list to tell us who has data waiting to be read
  // vector of pollfd structs
  std::vector<struct pollfd> m_pollFds;

  std::unordered_map<int, std::vector<uint8_t>> m_clientBuffers; // This prevents Player A's half-finished packets from mixing with Player B's packets
  TableManager m_tableManager; // Owns all the casino tables, manages seating, and routes game events to the right table.
  SessionManager m_sessionManager; // Keeps track of who is logged in and what table they are currently sitting at.
                                   // Acts as the bridge between raw network sockets and actual "Players". 
};


