#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../core/player.h"

class BaccaratScreen : public Screen
{
private:
  // Background
  sf::RectangleShape tableBackground;

  // Vectors to hold our card placeholders (Max 3 per side in Baccarat)
  std::vector<std::unique_ptr<sf::RectangleShape>> playerCards;
  std::vector<std::unique_ptr<sf::RectangleShape>> bankerCards;

  // Betting zone placeholders
  sf::RectangleShape playerBetZone;
  sf::RectangleShape tieBetZone;
  sf::RectangleShape bankerBetZone;

  // Vector to hold our current players
  std::vector<Player> currentPlayers;

public:
  BaccaratScreen(SharedData &sharedData) : Screen(sharedData)
  {
    // Set up a 1080p green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(35, 107, 43)); // Casino green

    // ---------------------------------------------------------
    // 1. CARD PLACEHOLDERS (The Empty Boxes)
    // ---------------------------------------------------------
    sf::Vector2f cardSize(120.0f, 180.0f);
    float cardSpacing = 150.0f; // Distance between cards

    // Player Cards (Left Side)
    for (int i = 0; i < 3; ++i) 
    {
      auto cardBox = std::make_unique<sf::RectangleShape>(cardSize);
      cardBox->setFillColor(sf::Color::Transparent); // Empty inside
      cardBox->setOutlineColor(sf::Color::Blue);     // Blue outline for Player
      cardBox->setOutlineThickness(3.0f);
      // Position them spaced out on the left side, upper half of the screen
      cardBox->setPosition({400.0f + (i * cardSpacing), 250.0f});
      playerCards.push_back(std::move(cardBox));
    }

    // Banker Cards (Right Side)
    for (int i = 0; i < 3; ++i) 
    {
      auto cardBox = std::make_unique<sf::RectangleShape>(cardSize);
      cardBox->setFillColor(sf::Color::Transparent); // Empty inside
      cardBox->setOutlineColor(sf::Color::Red);      // Red outline for Banker
      cardBox->setOutlineThickness(3.0f);
      // Position them spaced out on the right side
      cardBox->setPosition({1100.0f + (i * cardSpacing), 250.0f});
      bankerCards.push_back(std::move(cardBox));
    }

    // ---------------------------------------------------------
    // 2. BETTING ZONES
    // ---------------------------------------------------------
    sf::Vector2f betZoneSize(300.0f, 150.0f);

    // Player Bet Zone (Left)
    playerBetZone.setSize(betZoneSize);
    playerBetZone.setFillColor(sf::Color(20, 80, 25)); // Dark casino green
    playerBetZone.setOutlineColor(sf::Color::Blue);
    playerBetZone.setOutlineThickness(5.0f);
    playerBetZone.setPosition({400.0f, 700.0f});

    // Tie Bet Zone (Center)
    tieBetZone.setSize(betZoneSize);
    tieBetZone.setFillColor(sf::Color(20, 80, 25));
    tieBetZone.setOutlineColor(sf::Color::Yellow);
    tieBetZone.setOutlineThickness(5.0f);
    tieBetZone.setPosition({810.0f, 700.0f});

    // Banker Bet Zone (Right)
    bankerBetZone.setSize(betZoneSize);
    bankerBetZone.setFillColor(sf::Color(20, 80, 25));
    bankerBetZone.setOutlineColor(sf::Color::Red);
    bankerBetZone.setOutlineThickness(5.0f);
    bankerBetZone.setPosition({1220.0f, 700.0f});
  }

  //for use in the global debug ui in top left to display what object is being hovered over
  std::string getHoveredElement(sf::Vector2f mousePos) override
  {
    // 1. Check Betting Zones
    if (playerBetZone.getGlobalBounds().contains(mousePos)) return "Player Bet Zone";
    if (tieBetZone.getGlobalBounds().contains(mousePos)) return "Tie Bet Zone";
    if (bankerBetZone.getGlobalBounds().contains(mousePos)) return "Banker Bet Zone";

    // 2. Check Player Cards
    for (size_t i = 0; i < playerCards.size(); ++i)
    {
      if (playerCards[i]->getGlobalBounds().contains(mousePos))
      {
        return "Player Card " + std::to_string(i + 1);
      }
    }

    // 3. Check Banker Cards
    for (size_t i = 0; i < bankerCards.size(); ++i)
    {
      if (bankerCards[i]->getGlobalBounds().contains(mousePos))
      {
        return "Banker Card " + std::to_string(i + 1);
      }
    }

    // 4. Background or empty space
    if (tableBackground.getGlobalBounds().contains(mousePos)) return "Table Background";

    return "None";
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      // Pressing "escape" signals we want to switch to the main menu state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
      }
    }
  }

  void update() override
  {
    // Baccarat game logic (dealing cards, checking win conditions) goes here
  }

  void draw(sf::RenderWindow& window) override
  {
    // 1. Draw Background
    window.draw(tableBackground);

    // 2. Draw Betting Zones
    window.draw(playerBetZone);
    window.draw(tieBetZone);
    window.draw(bankerBetZone);

    // 3. Draw Player Cards
    for (const auto& card : playerCards)
    {
      window.draw(*card);
    }

    // 4. Draw Banker Cards
    for (const auto& card : bankerCards)
    {
      window.draw(*card);
    }
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
