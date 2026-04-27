#ifndef PLAYER_H
#define PLAYER_H

#include <string>

enum class BetChoice {
  None = 0,
  Player,
  Banker,
  Tie,
  Panda,
  Dragon,
};

class Player
{
public:
// Default constructor (In case you ever need to create a blank player)
  Player() 
    : m_fd(-1), m_chips(0), m_totalWinnings(0), m_totalLosses(0),
    m_currTableID(-1), m_currBetAmount(0), m_currBetChoice(BetChoice::None),
    m_isReady(false)
  {
  }

  ~Player() 
  {
    // Nothing dynamically allocated (no raw pointers), so this can be empty!
  }

  // Fixed: Added initialization for m_totalWinnings and m_totalLosses to prevent garbage values
  Player(int fd, std::string uuid, std::string username) 
    : m_fd(fd), m_UUID(uuid), m_username(username), m_chips(1000), 
    m_totalWinnings(0), m_totalLosses(0), // <--- FIXED HERE
    m_currTableID(-1), m_currBetAmount(0), 
    m_currBetChoice(BetChoice::None), m_isReady(false)
  {
  }

  // ----------------------------------------------------------------------
  // GETTERS (Read-only access for the Managers)
  // ----------------------------------------------------------------------

  int getFD() const { return m_fd; }
  std::string getUUID() const { return m_UUID; }
  std::string getUsername() const { return m_username; }
  int getChips() const { return m_chips; }
  int getCurrTableID() const { return m_currTableID; }
  int getCurrBetAmount() const { return m_currBetAmount; }
  BetChoice getCurrBetChoice() const { return m_currBetChoice; }
  bool isReady() const { return m_isReady; }



  // ----------------------------------------------------------------------
  // SETTERS & ACTIONS (Safe ways to modify the player's state)
  // ----------------------------------------------------------------------
  void setUsername(std::string username) { m_username = username; }
  void setUUID(std::string uuid) { m_UUID = uuid; }
  void setCurrTableID(int tableID) 
  {
    m_currTableID = tableID;

    // If they leave a table (tableID == -1), clear their active bet
    if (tableID == -1) {
      clearBet();
      m_isReady = false;
    }
  }

  void setReady(bool readyState) { m_isReady = readyState; }

  void setChips(int amount) { m_chips = amount; }
  void addChips(int amount) { if (amount > 0) { m_chips += amount; } }

  bool removeChips(int amount) 
  {
    if (amount > 0 && m_chips >= amount) {
      m_chips -= amount;
      return true;
    }
    return false; // Not enough chips!
  }

  bool placeBet(int amount, BetChoice choice) 
  {
    // Make sure they aren't already betting, and that they have the money
    if (m_currBetAmount == 0 && removeChips(amount)) {
      m_currBetAmount = amount;
      m_currBetChoice = choice;
      return true;
    }
    return false; 
  }

  void clearBet() 
  {
    m_currBetAmount = 0;
    m_currBetChoice = BetChoice::None;
  }

  void recordGameResult(int amountWon, int amountLost)
  {
    m_totalWinnings += amountWon;
    m_totalLosses += amountLost;

    if (amountWon > 0) {
      addChips(amountWon);
    }

    clearBet(); // Reset for the next round
  }
private:
  // Identifiers
  std::string m_UUID;
  std::string m_username;
  int m_fd;

  // Financials
  int m_chips;

  // Stats
  int m_totalWinnings;
  int m_totalLosses;

  // Game state
  int m_currTableID;
  int m_currBetAmount;
  BetChoice m_currBetChoice;
  bool m_isReady;
};

#endif
