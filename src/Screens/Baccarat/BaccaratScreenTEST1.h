//(when a real player joins at a seat you do this)
#pragma once
#include "../Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include "../../core/player.h"
#include "../../UI/UIHelper.h"
#include "../../UI/PlayerHUD.h"
#include "../../UI/CardSprite.h"
#include "../../logic/card.h"
#include "../../UI/PlayerTab.h"
#include "../../UI/ChipSprite.h"
#include "../../UI/TableChipRack.h"


class BaccaratScreenTEST1 : public Screen
{
private:
  // Background
  sf::RectangleShape tableBackground;

  Player m_emptyPlayer; // empty player

  std::vector<CardSprite> m_playerCards;
  std::vector<CardSprite> m_bankerCards;

  // Betting zone placeholders
  sf::RectangleShape playerBetZone;
  sf::RectangleShape tieBetZone;
  sf::RectangleShape bankerBetZone;

  PlayerTab m_playerTab;
  std::vector<Player> activePlayers;

  
  TableChipRack m_tableChipRack;

public:
  //PlayerHUD has no default constructor because sf::Text and sf::Sprite both require arguments
  //so we initialize m_playerTab in the initializer list
  BaccaratScreenTEST1(SharedData &sharedData) :
    Screen(sharedData),
    m_playerTab(
      sharedData.s_assets.getFont("gameFont"),
      sharedData.s_assets.getFont("gameNumbersFont"),
      sharedData.s_assets.getTexture("HUD_Coin"),
      m_emptyPlayer,
      sf::Vector2f{10.f, 10.f}
    ),
    m_tableChipRack(sharedData.s_testPlayer, sharedData.s_assets)
  {
    // Set up a 1080p green background
    tableBackground.setSize(sf::Vector2f({UI::SCREEN_W, UI::SCREEN_H}));
    tableBackground.setFillColor(sf::Color(35, 107, 0)); // Casino green

    // ---------------------------------------------------------
    // 1. CARD PLACEHOLDERS 
    // ---------------------------------------------------------
    // Calculate starting X
    float playerStartX = UI::SCREEN_W * 0.35f - 150.0f;
    float bankerStartX = UI::SCREEN_W * 0.65f - 150.0f;

    for (int i = 0; i < 3; ++i)
    {
      // Player cards
      m_playerCards.emplace_back(Card(5, 'S'), sharedData.s_assets);
      m_playerCards.back().setOrigin(m_playerCards.back().getLocalSize() / 2.f);
      UI::placeCardInHand(m_playerCards.back().getSFMLSprite(), playerStartX, 250.0f, i);

      // Banker cards
      m_bankerCards.emplace_back(Card(2, 'S'), sharedData.s_assets);
      m_bankerCards.back().setOrigin(m_bankerCards.back().getLocalSize() / 2.f);
      UI::placeCardInHand(m_bankerCards.back().getSFMLSprite(), bankerStartX, 250.0f, i);
    }

    // ---------------------------------------------------------
    // 2. BETTING ZONES
    // ---------------------------------------------------------
    sf::Vector2f betZoneSize(300.0f, 150.0f);
    float betZoneY = 500.0f;
    float betZoneOffset = 350.0f; 

    tieBetZone.setSize(betZoneSize);
    tieBetZone.setFillColor(sf::Color(20, 80, 25));
    tieBetZone.setOutlineColor(sf::Color::Yellow);
    tieBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(tieBetZone);
    UI::placeCentered(tieBetZone, betZoneY);

    playerBetZone.setSize(betZoneSize);
    playerBetZone.setFillColor(sf::Color(20, 80, 25)); 
    playerBetZone.setOutlineColor(sf::Color::Blue);
    playerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(playerBetZone);
    UI::placeCenteredoffsetLeft(playerBetZone, betZoneY, betZoneOffset); 

    bankerBetZone.setSize(betZoneSize);
    bankerBetZone.setFillColor(sf::Color(20, 80, 25));
    bankerBetZone.setOutlineColor(sf::Color::Red);
    bankerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(bankerBetZone);
    UI::placeCenteredoffsetRight(bankerBetZone, betZoneY, betZoneOffset); 

    // 3. INITIALIZE the PLAYER TAB
    m_playerTab.hud.setPlayer(&sharedData.s_testPlayer);

    //chips

    

  }

  std::string getHoveredElement(sf::Vector2f mousePos) override
  {
    if (playerBetZone.getGlobalBounds().contains(mousePos)) return "Player Bet Zone";
    if (tieBetZone.getGlobalBounds().contains(mousePos))    return "Tie Bet Zone";
    if (bankerBetZone.getGlobalBounds().contains(mousePos)) return "Banker Bet Zone";

    for (size_t i = 0; i < m_playerCards.size(); ++i)
      if (m_playerCards[i].getGlobalBounds().contains(mousePos))
        return "Player Card " + std::to_string(i + 1);

    for (size_t i = 0; i < m_bankerCards.size(); ++i)
      if (m_bankerCards[i].getGlobalBounds().contains(mousePos))
        return "Banker Card " + std::to_string(i + 1);

    if (m_playerTab.bounds.contains(mousePos))
      return "Player HUD";

    if (tableBackground.getGlobalBounds().contains(mousePos)) return "Table Background";
    return "None";
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
      }
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        for (auto& card : m_playerCards)
          card.flip();

        for (auto& card : m_bankerCards)
          card.flip();
      }
    }
  }

  void update(sf::RenderWindow& window, sf::Time dt) override
  {
    //calu mouse cords
    sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
    sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePosWindow);

    m_playerTab.hud.update();
    m_tableChipRack.update(mouseCoords);

    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    // -------------------------------------------------------
    // ImGui Chip Control Window
    // -------------------------------------------------------
    ImGui::Begin("Chip Controls"); 

    ImGui::Text("Enter desired chip amount:");
    
    static int inputAmount = 0; 

    // 1. Removed the crash-causing flag!
    ImGui::InputInt("##ChipAmount", &inputAmount, 1, 100);

    // 2. This is the correct ImGui way to check if the user pressed Enter
    // or clicked away after changing the number.
    bool userFinishedTyping = ImGui::IsItemDeactivatedAfterEdit();

    // Prevent negative chip values
    if (inputAmount < 0) {
        inputAmount = 0;
    }

    // 3. Update if the button is clicked OR they finished typing
    if (ImGui::Button("Update Rack") || userFinishedTyping)
    {
        m_tableChipRack.setValue(inputAmount);
        //std::cout << "Rack updated to: " << inputAmount << std::endl;
    }

    ImGui::End();
    // -------------------------------------------------------

    // Baccarat game logic (dealing cards, checking win conditions) goes here
  }

  void draw(sf::RenderWindow& window) override
  {
    window.draw(tableBackground);
    window.draw(playerBetZone);
    window.draw(tieBetZone);
    window.draw(bankerBetZone);

    for (const auto& card : m_playerCards)
    card.draw(window);

    for (const auto& card : m_bankerCards)
    card.draw(window);

    m_playerTab.hud.draw(window);
    m_tableChipRack.draw(window);
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
