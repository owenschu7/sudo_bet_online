#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>
#include <string>
#include "baccarat_defs.h"

using namespace std;

class Player
{
public:
  Player();
  ~Player();

  Player(string name, int cash);

  void create_player(string name, int cash) {m_name = name; m_cash = cash;}

  void add_outcome(int outcome); // 0 for banker; 1 for player; 2 for tie; 3 for panda; 4 for dragon; also keeps track of how many games played (length of vector)

  bool set_curr_bet_amount(int amount);
  int get_curr_bet_amount();
  bool reset_bet_amount();

  int set_choice(int bet);
  bool set_choice(Baccarat_Outcome o);
  Baccarat_Outcome get_choice();

  string get_name(){return m_name;}
  int get_cash(){return m_cash;}
  void add_cash(int amount){m_cash += amount;}

  //need to implement a way to calculate win % from m_outcomes

private:
  string m_name;
  int m_cash;
  Baccarat_Outcome m_choice;
  int m_curr_bet_amount;
  vector<int> m_outcomes;
};

#endif
