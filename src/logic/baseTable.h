#pragma once
#include <vector>
#include <algorithm>
#include "../core/GameEvents.h" 
#include "../network/server/player.h"

enum class TableState {
  WAITING_FOR_PLAYERS,
  BETTING_PHASE,
  GAME_IN_PROGRESS,
  PAYOUT,
};

class BaseTable {
protected:
  int tableID;
  GameType type;
  TableState state;
  int maxPlayers = 6;
  std::vector<Player*> currentPlayers;

public:
  BaseTable(int id, GameType t, int maxP)
  : tableID(id), type(t), state(TableState::WAITING_FOR_PLAYERS), maxPlayers(maxP)
  {
  }

  virtual ~BaseTable() = default; 

  // --- PURE VIRTUAL FUNCTIONS ---
  virtual void handleEvent(Player* player, const GameEvent& event) = 0;
  virtual void update() = 0; 

  // --- COMMON ACTIONS ---
  virtual bool addPlayer(Player* player)
  {
    // Basic validation: Is table full or game already running?
    if (currentPlayers.size() >= maxPlayers || state == TableState::GAME_IN_PROGRESS)
    {
      return false; 
    }

    currentPlayers.push_back(player);
    return true;
  }

  virtual void removePlayer(Player* player)
  {
    // Remove the specific player pointer from the vector
    currentPlayers.erase(
      std::remove(currentPlayers.begin(), currentPlayers.end(), player),
      currentPlayers.end()
    );

    // If the table becomes empty, reset it to waiting
    if (currentPlayers.empty()) {
      state = TableState::WAITING_FOR_PLAYERS;
    }
  }

  // --- GETTER IMPLEMENTATIONS ---
  int getTableID() const
  {
    return tableID; 
  }

  GameType getGameType() const
  {
    return type; 
  }

  TableState getState() const
  {
    return state; 
  }

  int getPlayerCount() const
  {
    return static_cast<int>(currentPlayers.size()); 
  }

  int getMaxPlayers() const
  {
    return maxPlayers; 
  }
};


