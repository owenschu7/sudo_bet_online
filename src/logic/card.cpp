#include <iostream>
#include "card.h"

using namespace std;

//default constructor
Card::Card()
{
    m_value = 0;
    m_suit = 'n';
}

//default deconstructor
Card::~Card()
{
}

//full constructor
Card::Card(int value, char suit)
{
  m_value = value;
  m_suit = suit;
  m_is_flipped = true;
}

char Card::get_suit()
{
  return m_suit;
}

void Card::set_suit(char suit)
{
  m_suit = suit;
}

int Card::get_value()
{
  return m_value;
}

void Card::set_value(int value)
{
  m_value = value;
}

int Card::calc_card_value()
{
  if (2 <= m_value && m_value <= 10)
  {
    //number card 2-10
    return m_value % 10;
  }
  else if ((11 <= m_value && m_value <= 13) || m_value == 0)
  {
    //jack, queen, king or empty 
    return 0;
  }
  else
  {
    //ace
    return 1;
  }
}

void Card::print_card()
{
  cout << m_value << " of " << m_suit<< endl;
}

