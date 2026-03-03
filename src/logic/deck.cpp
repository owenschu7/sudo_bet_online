#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "deck.h"
#include "card.h"

using namespace std;

Deck::Deck(int num_of_decks)
{
    char suits[] = {'s', 'd', 'h', 'c'}; // s spades, d diamonds, h hearts, c clubs, each suit is mapped to a number
    //create a deck that has (num_of_decks) number of decks
    m_length = num_of_decks * 52;
    for (int i = 0; i < num_of_decks; i++)
    {
        //for each suit
        for (int j = 0; j < 4; j++)
        {
            //create a standard 52 card deck
            for (int k = 2; k < 15; k++)
            {
                m_deck.push_back(Card(k, suits[j]));
            }
        }
    }
}

Deck::~Deck()
{
}

Card Deck::deal_card()
{
    Card dealt_card = m_deck.back();
    m_deck.pop_back();
    return dealt_card;
}

void Deck::shuffle_deck()
{
    random_device rd;
    mt19937 g(rd());
    shuffle(m_deck.begin(), m_deck.end(), g);
}

void Deck::print_deck()
{
    cout << "Printing deck: " << endl;
    for (int i = 0; i < m_length; i++)
    {
        m_deck[i].print_card();
    }
}
