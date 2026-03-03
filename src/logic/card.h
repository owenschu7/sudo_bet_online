#ifndef CARD_H
#define CARD_H

#include <iostream>

using namespace std;

class Card
{
public:
  Card();
  ~Card();

  Card(int value, char suit);

  char get_suit();
  void set_suit(char suit);

  int get_value();
  void set_value(int value);

  bool is_flipped() {return m_is_flipped;}
  void set_flipped(bool is_flipped) {m_is_flipped = is_flipped;}

  int calc_card_value();

  void print_card();

private:
  int m_value;
  char m_suit;
  bool m_is_flipped;

};

#endif
