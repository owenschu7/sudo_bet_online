#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include <memory>
#include "../core/player.h"
#include "../UI/UIHelper.h"


// to keep the tab elements bundled together
struct PlayerTab
{
  sf::RectangleShape box;
  sf::Text nameText;

  Player *player = NULL;
  PlayerTab(const sf::Font& font) : nameText(font) {}
};

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

  // Vector to hold our PlayerTabs
  std::vector<PlayerTab> playerTabs;
  std::vector<Player> activePlayers;

public:
  BaccaratScreen(SharedData &sharedData) : Screen(sharedData)
  {
    // Set up a 1080p green background
    tableBackground.setSize(sf::Vector2f({UI::SCREEN_W, UI::SCREEN_H}));
    tableBackground.setFillColor(sf::Color(35, 107, 43)); // Casino green

    // ---------------------------------------------------------
    // 1. CARD PLACEHOLDERS (The Empty Boxes)
    // ---------------------------------------------------------
    sf::Vector2f cardSize(120.0f, 180.0f);

    // Calculate starting X
    float playerStartX = UI::SCREEN_W * 0.35f - 150.0f;
    float bankerStartX = UI::SCREEN_W * 0.65f - 150.0f;

    float cardSpacing = 150.0f; // Distance between cards

    for (int i = 0; i < 3; ++i) 
    {
      // Player Card
      auto pCard = std::make_unique<sf::RectangleShape>(cardSize);
      pCard->setFillColor(sf::Color::Transparent);
      pCard->setOutlineColor(sf::Color::Blue);
      pCard->setOutlineThickness(3.0f);
      UI::centerOrigin(*pCard); // Center the origin
      UI::placeCardInHand(*pCard, playerStartX, 250.0f, i); // Use UIHelper
      playerCards.push_back(std::move(pCard));

      // Banker Card
      auto bCard = std::make_unique<sf::RectangleShape>(cardSize);
      bCard->setFillColor(sf::Color::Transparent);
      bCard->setOutlineColor(sf::Color::Red);
      bCard->setOutlineThickness(3.0f);
      UI::centerOrigin(*bCard); // Center the origin
      UI::placeCardInHand(*bCard, bankerStartX, 250.0f, i); // Use UIHelper
      bankerCards.push_back(std::move(bCard));
    }

    // ---------------------------------------------------------
    // 2. BETTING ZONES
    // ---------------------------------------------------------
    sf::Vector2f betZoneSize(300.0f, 150.0f);

    float betZoneY = 700.0f;
    float betZoneOffset = 350.0f; // Distance from the center Tie zone

    // Tie Bet Zone (Center)
    tieBetZone.setSize(betZoneSize);
    tieBetZone.setFillColor(sf::Color(20, 80, 25));
    tieBetZone.setOutlineColor(sf::Color::Yellow);
    tieBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(tieBetZone);
    UI::placeCentered(tieBetZone, betZoneY); // Locked to exact center;

    // Player Bet Zone (Left)
    playerBetZone.setSize(betZoneSize);
    playerBetZone.setFillColor(sf::Color(20, 80, 25)); 
    playerBetZone.setOutlineColor(sf::Color::Blue);
    playerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(playerBetZone);
    UI::placeCenteredoffsetLeft(playerBetZone, betZoneY, betZoneOffset); // Locked to center-left

    // Banker Bet Zone (Right)
    bankerBetZone.setSize(betZoneSize);
    bankerBetZone.setFillColor(sf::Color(20, 80, 25));
    bankerBetZone.setOutlineColor(sf::Color::Red);
    bankerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(bankerBetZone);
    UI::placeCenteredoffsetRight(bankerBetZone, betZoneY, betZoneOffset); // Locked to center-right

    // ---------------------------------------------------------
    // 3. INITIALIZE EMPTY PLAYER TABS
    // ---------------------------------------------------------
    int numTabs = 6;
    sf::Vector2f tabSize(240.0f, 60.0f);
    float tabSpacing = 20.0f;
    float totalWidth = (numTabs * tabSize.x) + ((numTabs - 1) * tabSpacing);
    float startX = (UI::SCREEN_W - totalWidth) / 2.0f;
    float startY = 980.0f;

    for (int i = 0; i < numTabs; ++i)
    {
      // Pass the globally shared font
      PlayerTab newTab(sharedData.s_gameFont);

      newTab.box.setSize(tabSize);
      newTab.box.setFillColor(sf::Color(30, 30, 30, 200));
      newTab.box.setOutlineColor(sf::Color(100, 100, 100));
      newTab.box.setOutlineThickness(2.0f);
      // We don't center the origin on these so they lay out sequentially nicely left-to-right
      newTab.box.setPosition({startX + (i * (tabSize.x + tabSpacing)), startY});

      newTab.nameText.setCharacterSize(20);
      newTab.nameText.setPosition({newTab.box.getPosition().x + 15.0f, newTab.box.getPosition().y + 15.0f});

      // Default to empty
      newTab.player = NULL;
      newTab.nameText.setString("Empty Seat");
      newTab.nameText.setFillColor(sf::Color(150, 150, 150));

      playerTabs.push_back(newTab);
    }
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
    //check for player tabs
    for (size_t i = 0; i < playerTabs.size(); ++i)
    {
      if (playerTabs[i].box.getGlobalBounds().contains(mousePos))
      {
        return "Tab: " + playerTabs[i].nameText.getString().toAnsiString();
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

  void update(sf::RenderWindow& window) override
  {
    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

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

    // 5. Draw player tabs
    for (const auto& tab : playerTabs)
    {
      window.draw(tab.box);
      window.draw(tab.nameText);
    }
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
