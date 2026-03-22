#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include "../UI/UIHelper.h"

class GameScreen : public Screen
{
private:
  //when we want to change screens (to main menu for example) we change this variable to say StreenState::MainMenu

  //things go here
  sf::RectangleShape tableBackground;

  // for the back of the card texture
  sf::Texture cardBackTexture;

  //holds all the cards being displayed
  std::vector<std::unique_ptr<sf::Sprite>> cards;

  //outline to hold the buttons
  sf::RectangleShape betZone;


public:
  GameScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(35,107,43)); //casino green


    //load the image
    if (!cardBackTexture.loadFromFile("assets/images/Cards/card_back_blue.png"))
    {
      std::cerr << "Failed to load card back image!\n";
    }


    //cards
    for (int i = 0; i < 6; ++i) 
    {
      auto newCard = std::make_unique<sf::Sprite>(cardBackTexture);
      UI::centerOrigin(*newCard);
      // std::move transfers ownership of the unique_ptr into the vector
      cards.push_back(std::move(newCard)); 
    }
    UI::placeAllCards(cards, 300.0f);

    // Big Betting Zone Outline (Now with a dark green background)
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

  void update() override
  {
    // Baccarat game logic (dealing cards, checking win conditions) will go here
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(tableBackground);

    // Betting Zones
    window.draw(betZone); // The main outline

    //cards
    for (const std::unique_ptr<sf::Sprite>& card : cards)
    {
      window.draw(*card);
    }
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
