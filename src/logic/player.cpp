#include <iostream>
#include <vector>
#include "player.h"

using namespace std;

Player::Player()
{
  m_cash = 0;
}

Player::~Player()
{
  m_cash = 0;
}

Player::Player(string name, int cash)
{
  m_name = name;
  m_cash = cash;
}

void Player::add_outcome(int outcome)
{
  m_outcomes.push_back(outcome);
}

bool Player::reset_bet_amount()
{
  m_curr_bet_amount = 0;
  return true;
}

bool Player::set_curr_bet_amount(int amount)
{
  //check if amount is either: bigger, smaller or equal to current bet
  if (amount != m_curr_bet_amount)
  {
    //check if player has enough
    if (m_cash >= amount)
    {
      //player has enough
      //add curr_bet_amount to balance
      //take away new bet from balance
      //set new bet amount
      m_cash = m_cash + m_curr_bet_amount;
      m_cash = m_cash - amount;
      m_curr_bet_amount = amount;
      return true;
    } else {
      return false;
    }
  } else {
    // do nothing because values are the same
    return true;
  }
}

int Player::get_curr_bet_amount()
{
  return m_curr_bet_amount;
}

bool Player::set_choice(Baccarat_Outcome o)
{
  switch (o)
  {
    case Baccarat_Outcome::BANKER:
    case Baccarat_Outcome::PLAYER:
    case Baccarat_Outcome::TIE:
    case Baccarat_Outcome::PANDA:
    case Baccarat_Outcome::DRAGON:
      m_choice = o;
      return true;
    default:
      return false;
  }
}

Baccarat_Outcome Player::get_choice()
{
  return m_choice;
}

