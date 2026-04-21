/* =========================================================================
 * SessionManager
 * PURPOSE:
 * This class acts as the bridge between the raw networking layer
 * (ServerApplication) and the game logic layer (TableManager).
 * It does not handle sockets, it does not run card games.
 * It manages the PEOPLE and the routing
 *
 * HOW IT FITS THE ARCHITECTURE:
 * 1.) ServerApplication gets raw bytes, frames a packet, and hands it here.
 * 2.) SessionManager figures out WHO sent it and WHAT they want to do.
 *     - If they want to log in, SessionManager handles it.
 *     - If they want to place a bet, SessionManager forwards it to TableManager.
 * 3.) If a game needs to send a message back to a player, it tells the
 *     SessionManager which uses sendCallback to send a message back to the server.
 * ========================================================================= */

//SessionManager class
//  keeps track of all the players in the server and what they are currently doing
//  - game events are routed from the server into the table
#pragma once

#include <unordered_map>
#include <memory>
#include <functional>

#include "../../core/GameEvents.h"
#include "player.h"
#include "TableManager.h" 
#include <iostream>
#include "../../core/Debug.h"

class SessionManager
{
public:
  // Constructor: Pass in the TableManager by reference
  SessionManager(
    std::function<void(int fd, const GameEvent& event)> sendCallback,
    TableManager& tableManager
  ) : m_sendCallback(sendCallback), m_tableManager(tableManager) 
  {
  }

  // Network Event Handlers
  void onClientConnected(int client_fd)
  {
    TRACE_FUNCTION();
    DEBUG_PRINT << "Preparing guest session for FD: " << client_fd << "\n";

    // Create a "Guest" player. They don't have a UUID or Username yet.
    // We use "Guest" as a placeholder until the SYS_Connect packet arrives.
    auto guestPlayer = std::make_unique<Player>(client_fd, "0000", "Guest");

    //add the players
    m_activePlayers[client_fd] = std::move(guestPlayer);
  }

  void handleClientMessage(int client_fd, const GameEvent& event)
  {
    TRACE_FUNCTION();
    DEBUG_PRINT << "RECIEVED EVENT: " << event << "\n";

    // ---------------------------------------------------------
    // SYSTEM EVENTS (Login, Disconnect, etc.)
    // ---------------------------------------------------------
    if (event.type == EventType::SYS_Connect)
    {
      // Instantiating your actual Player class! 
      auto newPlayer = std::make_unique<Player>(client_fd, event.senderUUID, event.senderUsername);

      // Store the player in our active session map
      m_activePlayers[client_fd] = std::move(newPlayer);

      GameEvent response;
      response.type = EventType::SYS_Connect_Success;
      m_sendCallback(client_fd, response);
      return;
    }

    // ---------------------------------------------------------
    // AUTHENTICATION GUARD (Updated Fix)
    // ---------------------------------------------------------
    auto it = m_activePlayers.find(client_fd);
    if (it == m_activePlayers.end()) {
      std::cerr << "[Warning] Received event from unknown FD: " << client_fd << "\n";
      return; 
    }

    // Grab the raw pointer to the player
    Player* currentPlayer = it->second.get();

    // Block Guests from accessing the casino floor
    if (currentPlayer->getUsername() == "Guest" && event.type != EventType::SYS_Connect) {
      std::cerr << "[Warning] Guest on FD " << client_fd << " tried to send restricted event.\n";
      return;
    }

    // ---------------------------------------------------------
    // MASTER ROUTING SWITCH
    // ---------------------------------------------------------
    switch (event.type) 
    {
      // === ACCOUNT & AUTHENTICATION ===
      case EventType::USER_In:
      case EventType::USER_Out:
      case EventType::USER_CreateAccount:
      case EventType::USER_ChangeUsername:
        DEBUG_PRINT << "Handling Auth Event for: " << currentPlayer->getUsername() << "\n";
        break;

      // === LOBBY & TABLE MANAGEMENT ===
      case EventType::GET_AvailableTables:
        {
          DEBUG_PRINT << "GET_AvailableTables: \n";
          GameEvent lobbyData = m_tableManager.getAvailableTablesListEvent();
          m_sendCallback(client_fd, lobbyData);
          break;
        }
      case EventType::CREATE_Table:
        {
          // TODO: Tell TableManager to spin up a new table dynamically
          break;
        }
      case EventType::JOIN_Table:
        {
          int tableToJoin = event.intPayload;
          bool success = m_tableManager.addPlayerToTable(tableToJoin, currentPlayer);
          if (success) { 
            // send success packet 
          } else { 
            // send table full error 
          }
          break;
        }
      case EventType::LEAVE_Table:
        {
          m_tableManager.removePlayerFromTable(currentPlayer->getCurrTableID(), currentPlayer);
          break;
        }

      // === CHAT ===
      case EventType::CHAT_Send:
        {
          // TODO: Broadcast chat message to the lobby or the specific table
          break;
        }

      // === IN-GAME ACTIONS ===
      case EventType::BC_USER_Bet:
      case EventType::BJ_USER_PlaceBet:
      case EventType::BJ_USER_PlayerHit:
      case EventType::BJ_USER_PlayerStand:
        {
          m_tableManager.routeEventToTable(currentPlayer->getCurrTableID(), currentPlayer, event);
          break;
        }

      // === OUTBOUND/SERVER-ONLY EVENTS ===
      case EventType::SYS_Disconnect:
      case EventType::SYS_Connect_Success:
      case EventType::SYS_Connect_Failed:
      case EventType::NET_Connect_Success:
      case EventType::NET_Connect_Failed:
      case EventType::NET_NewAccount_Success:
      case EventType::JOIN_NET_Table:
      case EventType::LEAVE_NET_Table:
      case EventType::CHAT_NET_Recv:
      case EventType::BC_NET_Bet:
      case EventType::BC_NET_Ready:
      case EventType::BC_NET_AllReady:
        std::cerr << "[Warning] Client sent a server-only event type: " 
          << static_cast<int>(event.type) << "\n";
        break;

      default:
        std::cerr << "[Error] Unknown EventType received.\n";
        break;
    }
  }

  void onClientDisconnected(int client_fd)
  {
    TRACE_FUNCTION();

    auto it = m_activePlayers.find(client_fd);
    if (it != m_activePlayers.end())
    {
      Player* disconnectingPlayer = it->second.get();

      // [TABLE MANAGER HOOK]
      // Yank the player from whatever game they were playing
      // m_tableManager.removePlayerFromAllTables(disconnectingPlayer);

      // Delete the player from memory
      m_activePlayers.erase(it);
    }
  }

private:
  std::function<void(int fd, const GameEvent& event)> m_sendCallback;

  TableManager& m_tableManager; // Reference to the table manager

  // The actual roster of logged-in players mapped to their sockets
  std::unordered_map<int, std::unique_ptr<Player>> m_activePlayers; 
};
