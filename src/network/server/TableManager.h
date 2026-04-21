// TableManger class
// responsible for managing all active tables (game instances) on the server
// - add/remove player to table
// - get all active tables
// - create tables (eventually)
// - 
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <sstream>

#include "../../core/GameEvents.h"
#include "../../core/Debug.h"

#include "../../logic/baseTable.h"
#include "../../logic/baccarat/baccarat_table.h"

#include "player.h"

// If you add Poker or Blackjack later, include them here!

class TableManager
{
private:
  // The master list of all active tables on the server.
  // Maps TableID -> The actual Table object
  std::unordered_map<int, std::unique_ptr<BaseTable>> m_tables;

public:
  TableManager() // constructor initalizes two tables on boot
  {
    TRACE_FUNCTION();
    m_tables[1] = std::make_unique<Baccarat_table>(1, 8); 
    m_tables[2] = std::make_unique<Baccarat_table>(2, 8);
    DEBUG_PRINT << "TableManager: Initialized " << m_tables.size() << " tables on the casino floor.\n";
  }

  ~TableManager() = default;

  // The Global Tick: Called by ServerApplication ~10 times a second
  void updateAll()
  {
    TRACE_FUNCTION();
    // Tick every single table. If a table is empty, its update() function 
    // will just instantly return, costing you basically zero CPU time.
    for (auto& pair : m_tables)
    {
      pair.second->update(); 
    }
  }


  // event routing
  // Takes an action from SessionManager and hands it to the specific table
  void routeEventToTable(int tableID, Player* player, const GameEvent& event)
  {
    TRACE_FUNCTION();
    auto it = m_tables.find(tableID);

    if (it != m_tables.end()) {
      // We found the table! Hand the event to the game logic.
      it->second->handleEvent(player, event);
    } else {
      std::cerr << "[Warning] Player " << player->getUsername() 
        << " sent an event to an invalid Table ID: " << tableID << "\n";
    }
  }

  // Packages up all the table info so SessionManager can send it to a new client
  //returns a GameEvent that holds
  GameEvent getAvailableTablesListEvent()
  {
    TRACE_FUNCTION();
    GameEvent event;

    // Updated to match your specific Enum name
    event.type = EventType::GET_AvailableTables; 

    // We use a stringstream to build a "CSV-style" string of all tables
    // Format: "ID,Type,Current,Max|ID,Type,Current,Max|"
    std::stringstream ss;

    for (const auto& pair : m_tables) {
      BaseTable* t = pair.second.get();

      ss << t->getTableID() << ","
        << static_cast<int>(t->getGameType()) << ","
        << t->getPlayerCount() << ","
        << t->getMaxPlayers() << "|";
    }

    // Store the built string in the payload
    event.stringPayload = ss.str();

    DEBUG_PRINT << "TableManager: Generated lobby list: " << event.stringPayload << "\n";

    return event;
  }

  //player management
  bool addPlayerToTable(int tableID, Player* player)
  {
    TRACE_FUNCTION();
    auto it = m_tables.find(tableID);
    if (it != m_tables.end()) {
      bool success = it->second->addPlayer(player);
      if (success) {
        // Update the player's internal state so they know where they are sitting
        player->setCurrTableID(tableID); 
        DEBUG_PRINT << "Player " << player->getUsername() << " joined Table " << tableID << "\n";
      }
      return success;
    }
    return false;
  }
  void removePlayerFromTable(int tableID, Player* player)
  {
    TRACE_FUNCTION();
    auto it = m_tables.find(tableID);
    if (it != m_tables.end()) {
      it->second->removePlayer(player);
      player->setCurrTableID(-1); // -1 means "in the lobby"
    }
  }

  // Crucial for when a player's network cable gets unplugged
  void removePlayerFromAllTables(Player* player)
  {
    TRACE_FUNCTION();
    // If we already know what table they are at, we can just remove them from that one
    int currentTable = player->getCurrTableID();
    if (currentTable != -1) {
      removePlayerFromTable(currentTable, player);
    } else {
      // Fallback: search all tables just in case their state got out of sync
      for (auto& pair : m_tables) {
        pair.second->removePlayer(player);
      }
    }
  }
};
