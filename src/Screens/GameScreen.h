#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include <memory>
#include <iostream>
#include "../UI/UIHelper.h"

#include "../logic/card.h"
#include "../UI/CardSprite.h"


class GameScreen : public Screen
{
private:
  //when we want to change screens (to main menu for example) we change this variable to say StreenState::MainMenu

  //things go here
  sf::RectangleShape tableBackground;

  //holds all the cards being displayed
  std::vector<CardSprite> m_cardSprites;

  //outline to hold the buttons
  sf::RectangleShape betZone;


public:
  GameScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(35,107,43)); //casino green


    Card testCard = Card(5, 's');

    // Cards should live in SharedData, not locally
    for (int i = 0; i < 6; ++i)
    {
      CardSprite cs(testCard, sharedData.s_assets);
      UI::centerOrigin(cs.getSFMLSprite());
      m_cardSprites.emplace_back(std::move(cs));
    }

    UI::placeAllCards(m_cardSprites, 300.0f);

    betZone.setSize({500.0f, 500.0f});
    // A nice dark casino green (RGB: 20, 80, 25)
    betZone.setFillColor(sf::Color(20, 80, 25)); 
    betZone.setOutlineColor(sf::Color::Black);
    betZone.setOutlineThickness(5.0f);
    UI::centerOrigin(betZone);
    UI::placeCentered(betZone, 800.0f);


  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "escape" signals we want to switch to the main menu state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
      }
    }
  }

  void update(sf::RenderWindow& window, sf::Time dt) override
  {
    processEventsFromServer();

    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    // Baccarat game logic (dealing cards, checking win conditions) will go here
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(tableBackground);

    // Betting Zones
    window.draw(betZone); // The main outline

    //cards
    for (auto& card : m_cardSprites)
    {
      card.draw(window);
    }
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
