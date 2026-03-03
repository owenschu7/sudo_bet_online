#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../../UI/UIHelper.h"
#include "../../UI/Button.h"

class GameScreen : public Screen
{
private:
  //when we want to change screens (to main menu for example) we change this variable to say StreenState::MainMenu
  ScreenState nextState = ScreenState::None;

  //things go here
  sf::RectangleShape tableBackground;

  //betting zones (rectangles with text overlay)
  //these dont need pointers because they have default constructors
  sf::RectangleShape betZone;
  sf::RectangleShape playerBetZone;
  sf::RectangleShape bankerBetZone;
  sf::RectangleShape tieBetZone;
  sf::RectangleShape pandaBetZone;
  sf::RectangleShape dragonBetZone;
  //text for the bettingzones
  //these need pointers
  std::unique_ptr<sf::Text> playerBetZoneLabel;
  std::unique_ptr<sf::Text> bankerBetZoneLabel;
  std::unique_ptr<sf::Text> tieBetZoneLabel;
  std::unique_ptr<sf::Text> pandaBetZoneLabel;
  std::unique_ptr<sf::Text> dragonBetZoneLabel;

  //font
  sf::Font font;

  //text
  std::unique_ptr<sf::Text> playerLabel;
  std::unique_ptr<sf::Text> bankerLabel;

  //images
  sf::Texture cardBackTexture;
  //card vector to hold all 6 cards
  std::vector<std::unique_ptr<sf::Sprite>> cards;

  //deal button
  sf::RectangleShape dealButton;
  std::unique_ptr<sf::Text> dealButtonLabel;

public:
  GameScreen()
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(35,107,43)); //casino green


    //load the heavy assets first
    if (!font.openFromFile("assets/fonts/8bitOperatorPlus8-Regular.ttf"))
    {
      std::cerr << "failed to load font from assets folder!\n";
    }

    //load the image
    if (!cardBackTexture.loadFromFile("assets/images/Cards/card_back_blue.png"))
    {
      std::cerr << "Failed to load card back image!\n";
    }
    
    //time to build the objects using std::make_unique
    //player labels
    playerLabel = std::make_unique<sf::Text>(font);
    playerLabel->setString("PLAYER"); // use -> for pointers
    playerLabel->setCharacterSize(120);
    UI::centerOrigin(*playerLabel); // use * to pass the actual object
    UI::placeAtPlayerHandX(*playerLabel, 100.0f);

    //banker labels
    bankerLabel = std::make_unique<sf::Text>(font);
    bankerLabel->setString("BANKER");
    bankerLabel->setCharacterSize(120);
    UI::centerOrigin(*bankerLabel);
    UI::placeAtBankerHandX(*bankerLabel, 100.0f);

    //cards
    for (int i = 0; i < 6; ++i) 
    {
      auto newCard = std::make_unique<sf::Sprite>(cardBackTexture);
      UI::centerOrigin(*newCard);
      // std::move transfers ownership of the unique_ptr into the vector
      cards.push_back(std::move(newCard)); 
    }
    UI::placeAllCards(cards, 300.0f);

    // Betting Zone Outlines
    betZone.setSize({500.0f, 500.0f});
    betZone.setFillColor(sf::Color::Transparent);
    betZone.setOutlineColor(sf::Color::Black);
    betZone.setOutlineThickness(5.0f);
    UI::centerOrigin(betZone);
    UI::placeCenteredX(betZone, 800.0f);

    playerBetZone.setSize({400.0f, 150.0f});
    playerBetZone.setFillColor(sf::Color::Transparent);
    playerBetZone.setOutlineColor(sf::Color::Black);
    playerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(playerBetZone);
    UI::placeCenteredX(playerBetZone, 780.0f); // Low on the screen

    playerBetZoneLabel = std::make_unique<sf::Text>(font);
    playerBetZoneLabel->setString("PLAYER");
    playerBetZoneLabel->setCharacterSize(80);
    playerBetZoneLabel->setFillColor(sf::Color(255, 255, 255, 150)); // Semi-transparent white
    UI::centerOrigin(*playerBetZoneLabel);
    UI::placeCenteredX(*playerBetZoneLabel, 780.0f);

    // Banker Zone Outline
    bankerBetZone.setSize({400.0f, 150.0f});
    bankerBetZone.setFillColor(sf::Color::Transparent);
    bankerBetZone.setOutlineColor(sf::Color::Black);
    bankerBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(bankerBetZone);
    UI::placeCenteredX(bankerBetZone, 960.0f);

    bankerBetZoneLabel = std::make_unique<sf::Text>(font);
    bankerBetZoneLabel->setString("BANKER");
    bankerBetZoneLabel->setCharacterSize(80);
    bankerBetZoneLabel->setFillColor(sf::Color(255, 255, 255, 150));
    UI::centerOrigin(*bankerBetZoneLabel);
    UI::placeCenteredX(*bankerBetZoneLabel, 960.0f);

    // Tie Zone Outline (Dead Center)
    tieBetZone.setSize({120.0f, 100.0f});
    tieBetZone.setFillColor(sf::Color::Transparent);
    tieBetZone.setOutlineColor(sf::Color::Black);
    tieBetZone.setOutlineThickness(5.0f);
    UI::centerOrigin(tieBetZone);
    UI::placeCenteredX(tieBetZone, 630.0f);

    tieBetZoneLabel = std::make_unique<sf::Text>(font);
    tieBetZoneLabel->setString("TIE");
    tieBetZoneLabel->setCharacterSize(60);
    tieBetZoneLabel->setFillColor(sf::Color(255, 215, 0, 150));
    UI::centerOrigin(*tieBetZoneLabel);
    UI::placeCenteredX(*tieBetZoneLabel, 630.0f);

    //deal button
    dealButtonLabel = std::make_unique<sf::Text>(font);
    dealButtonLabel->setString("DEAL");
    dealButtonLabel->setCharacterSize(80);
    dealButtonLabel->setFillColor(sf::Color::Black);
    UI::centerOrigin(*dealButtonLabel);
    UI::placeLeft(*dealButtonLabel, 860.0f);
    // deal button rectangle
    dealButton.setSize({400.0f, 150.0f});
    dealButton.setFillColor(sf::Color::White);
    dealButton.setOutlineColor(sf::Color::Black);
    dealButton.setOutlineThickness(5.0f);
    UI::centerOrigin(dealButton);
    UI::placeLeft(dealButton, 860.0f);
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "escape" signals we want to switch to the main menu state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        nextState = ScreenState::MainMenu;
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

    // draw betting zone outlines
    window.draw(betZone);
    window.draw(playerBetZone);
    window.draw(bankerBetZone);
    window.draw(tieBetZone);

    // draw labels (* dereferences pointers)
    window.draw(*playerBetZoneLabel);
    window.draw(*bankerBetZoneLabel);
    window.draw(*tieBetZoneLabel);

    window.draw(*playerLabel);
    window.draw(*bankerLabel);

    //deal button drawing
    window.draw(dealButton);
    window.draw(*dealButtonLabel);

    //cards
    for (const std::unique_ptr<sf::Sprite>& card : cards)
    {
      window.draw(*card);
    }
  }
  
  ScreenState getNextState() const override
  {
    return nextState;
  }
};
