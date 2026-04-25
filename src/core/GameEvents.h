#pragma once
#include <string>
#include <iostream>

// Define all the possible actions a user can take, or the server can trigger
enum class Action
{
  //C2S = client to server
  //S2C = server to client

  //system events
  SYS_Connect = 0,         //Done
  SYS_Disconnect,          //D
  SYS_Connect_Success,     //D
  SYS_Connect_Failed,      //D

  GET_AvailableTables,     //D get a list of all available tables to play at
  GET_AvailableTables_Response,//D get a list of all available tables to play at
  CREATE_Table,            //D user create a table
  JOIN_Table,              //D user joins
  JOIN_Table_Broadcast,    //D broadcast to all people in a table that a user joins
  JOIN_Table_Success,      //D user joins was success
  JOIN_Table_Failed,       //D user could not join
  JOIN_NET_Table,          // someone has joined the table
  LEAVE_Table,             // user leaves
  LEAVE_NET_Table,         // someone has left the table

  //baccarat events
  // user actions
  BC_USER_Bet,             // user placed bet
  // actions from other people
  BC_NET_Bet,              // someone made a bet
  BC_NET_Ready,            // someone is ready
  BC_NET_AllReady,         // everyone at table is ready
};

enum class Game {
  NONE = 0,
  BACCARAT = 1,
  POKER = 2,
  BLACKJACK = 3

};


// A generic event structure that holds a game event
// - Action type
// - std::string senderUUID
// - std::string senderUsername
// - std::string stringPayload
// - int intPayload
struct GameEvent
{
  Action action;
  Game game;

  std::string senderUUID;
  std::string senderUsername;


  // Optional data attached to the event. 
  std::string stringPayload = "";
  int intPayload = 0;
};

inline const char* ActionToString(Action type) 
{
  switch (type)
  {
    // System
    case Action::SYS_Connect:            return "SYS_Connect";
    case Action::SYS_Disconnect:         return "SYS_Disconnect";
    case Action::SYS_Connect_Success:    return "SYS_Connect_Success";
    case Action::SYS_Connect_Failed:     return "SYS_Connect_Failed";

    // Tables/Lobby
    case Action::GET_AvailableTables:    return "GET_AvailableTables";
    case Action::GET_AvailableTables_Response:    return "GET_AvailableTables_Response";
    case Action::CREATE_Table:           return "CREATE_Table";
    case Action::JOIN_Table:             return "JOIN_Table";
    case Action::JOIN_NET_Table:         return "JOIN_NET_Table";
    case Action::LEAVE_Table:            return "LEAVE_Table";
    case Action::LEAVE_NET_Table:        return "LEAVE_NET_Table";

    // Baccarat
    case Action::BC_USER_Bet:            return "BC_USER_Bet";
    case Action::BC_NET_Bet:             return "BC_NET_Bet";
    case Action::BC_NET_Ready:           return "BC_NET_Ready";
    case Action::BC_NET_AllReady:        return "BC_NET_AllReady";

    // Catch-all
    default:                                return "UNKNOWN_EVENT";
  }
}

inline const char* GameToString(Game game) 
{
  switch (game)
  {
    // System
    case Game::NONE:            return "NONE";
    case Game::BACCARAT:        return "BACCARAT";
    case Game::BLACKJACK:       return "BLACKJACK";
    case Game::POKER:           return "POKER";

    // Catch-all
    default:                    return "UNKNOWN_EVENT";
  }
}

inline Game StringToGame(std::string game)
{
    if (game == "NONE")          return Game::NONE;
    if (game == "BACCARAT")      return Game::BACCARAT;
    if (game == "BLACKJACK")     return Game::BLACKJACK;
    if (game == "POKER")         return Game::POKER;

    // Catch-all
    return Game::NONE;
}


// 2. Tells C++ how to print a GameEvent struct to the console
inline std::ostream& operator<<(std::ostream& os, const GameEvent& event) 
{
  os << "{" 
    << ActionToString(event.action)
    << " | Game: " << GameToString(event.game)
    << " | User: " << (event.senderUsername.empty() ? "None" : event.senderUsername)
    << " | PayloadInt: " << event.intPayload 
    << " | PayloadStr: '" << event.stringPayload << "'"
    << "}";
  return os;
}






