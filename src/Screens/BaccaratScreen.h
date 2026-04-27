//playerTabs[seatIndex].player = &realPlayer;
//(when a real player joins at a seat you do this)
#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include <memory>
#include "../core/player.h"
#include "../UI/UIHelper.h"
#include "../UI/PlayerHUD.h"
#include "../UI/CardSprite.h"
#include "../logic/card.h"

// to keep the tab elements bundled together
struct PlayerTab
{
  PlayerHUD hud; 
  sf::FloatRect bounds; // Used for mouse hover detection
  bool isEmpty;

  PlayerTab(const sf::Font& font, const sf::Texture& hudTex, const Player& player, sf::Vector2f hudPos)
    : hud(player, hudTex, font, hudPos),
      bounds(hudPos, sf::Vector2f(PlayerHUD::WIDTH, PlayerHUD::HEIGHT)), 
      isEmpty(true)
  {}
};

class BaccaratScreen : public Screen
{
private:
  // Background
  sf::RectangleShape tableBackground;

  Player m_emptyPlayer; // empty player

  // Vectors to hold our separated Logic and Visuals
  std::vector<std::unique_ptr<Card>> m_playerLogicCards;
  std::vector<std::unique_ptr<CardSprite>> m_playerVisualCards;

  std::vector<std::unique_ptr<Card>> m_bankerLogicCards;
  std::vector<std::unique_ptr<CardSprite>> m_bankerVisualCards;

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
    // 1. CARD PLACEHOLDERS 
    // ---------------------------------------------------------
    // Calculate starting X
    float playerStartX = UI::SCREEN_W * 0.35f - 150.0f;
    float bankerStartX = UI::SCREEN_W * 0.65f - 150.0f;

    for (int i = 0; i < 3; ++i) 
    {
      // --- Player Card Setup ---
      auto pLogic = std::make_unique<Card>(1, 'S'); 
      
      // Build the string key (e.g., "1S") to ask the manager for the right texture
      std::string pKey = std::to_string(pLogic->get_value()) + pLogic->get_suit();
      
      // Pass the specific front texture and the generic back texture from the AssetManager
      auto pVisual = std::make_unique<CardSprite>(
          *pLogic, 
          sharedData.s_assets.getTexture(pKey), 
          sharedData.s_assets.getTexture("cardBack")
      );

      pVisual->setOrigin(pVisual->getLocalSize() / 2.f); 
      UI::placeCardInHand(pVisual->getSFMLSprite(), playerStartX, 250.0f, i); 

      m_playerLogicCards.push_back(std::move(pLogic));
      m_playerVisualCards.push_back(std::move(pVisual));


      // --- Banker Card Setup ---
      auto bLogic = std::make_unique<Card>(13, 'H');
      
      std::string bKey = std::to_string(bLogic->get_value()) + bLogic->get_suit();
      
      auto bVisual = std::make_unique<CardSprite>(
          *bLogic, 
          sharedData.s_assets.getTexture(bKey), 
          sharedData.s_assets.getTexture("cardBack")
      );

      bVisual->setOrigin(bVisual->getLocalSize() / 2.f);
      UI::placeCardInHand(bVisual->getSFMLSprite(), bankerStartX, 250.0f, i);

      m_bankerLogicCards.push_back(std::move(bLogic));
      m_bankerVisualCards.push_back(std::move(bVisual));
    }

    // ---------------------------------------------------------
    // 2. BETTING ZONES
    // ---------------------------------------------------------
    sf::Vector2f betZoneSize(300.0f, 150.0f);
    float betZoneY = 700.0f;
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

    // ---------------------------------------------------------
    // 3. INITIALIZE EMPTY PLAYER TABS
    // ---------------------------------------------------------
    int numTabs = 4;
    sf::Vector2f tabSize(240.0f, 60.0f);
    float startY = 880.0f;

    float totalTabWidth = numTabs * tabSize.x;
    float tabSpacing = 220.f;
    float startX = 20.f; 

    playerTabs.reserve(numTabs); 
    
    // Fetch Font and HUD Background Texture once for the loop
    const sf::Font& font = sharedData.s_assets.getFont("gameFont");
    const sf::Texture& hudTex = sharedData.s_assets.getTexture("HUD_Coin");

    for (int i = 0; i < numTabs; ++i)
    {
      float tabX = startX + (i * (tabSize.x + tabSpacing));
      float hudX = tabX + (tabSize.x / 2.f) - (PlayerHUD::WIDTH / 2.f); 
      float hudY = startY;

      playerTabs.emplace_back(font, hudTex, m_emptyPlayer, sf::Vector2f{hudX, hudY});
    }

    playerTabs[0].hud.setPlayer(&sharedData.s_localPlayer);
    playerTabs[0].isEmpty = false; 
  }

  std::string getHoveredElement(sf::Vector2f mousePos) override
  {
    if (playerBetZone.getGlobalBounds().contains(mousePos)) return "Player Bet Zone";
    if (tieBetZone.getGlobalBounds().contains(mousePos)) return "Tie Bet Zone";
    if (bankerBetZone.getGlobalBounds().contains(mousePos)) return "Banker Bet Zone";

    // Replaced the old broken `playerCards` references with `m_playerVisualCards`
    for (size_t i = 0; i < m_playerVisualCards.size(); ++i)
    {
      if (m_playerVisualCards[i]->getGlobalBounds().contains(mousePos))
      {
        return "Player Card " + std::to_string(i + 1);
      }
    }

    // Replaced the old broken `bankerCards` references with `m_bankerVisualCards`
    for (size_t i = 0; i < m_bankerVisualCards.size(); ++i)
    {
      if (m_bankerVisualCards[i]->getGlobalBounds().contains(mousePos))
      {
        return "Banker Card " + std::to_string(i + 1);
      }
    }
    
    for (size_t i = 0; i < playerTabs.size(); ++i)
    {
      if (playerTabs[i].bounds.contains(mousePos))
      {
        return "Player HUD Seat " + std::to_string(i + 1);
      }
    }

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
    }
  }

  void update(sf::RenderWindow& window) override
  {
    for (auto& tab : playerTabs) 
    {
      tab.hud.update();  
    }
    
    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    // Baccarat game logic (dealing cards, checking win conditions) goes here
  }

  void draw(sf::RenderWindow& window) override
  {
    window.draw(tableBackground);
    window.draw(playerBetZone);
    window.draw(tieBetZone);
    window.draw(bankerBetZone);

    // Call draw() directly on the CardSprite visual classes
    for (const auto& card : m_playerVisualCards)
    {
      card->draw(window);
    }

    for (const auto& card : m_bankerVisualCards)
    {
      card->draw(window);
    }

    for (const auto& tab : playerTabs)
    {
      tab.hud.draw(window); 
    }
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
