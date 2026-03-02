#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include "../UIHelper.h"

class GameScreen : public Screen
{
private:
  ScreenState nextState = ScreenState::None;

  //things go here
  sf::RectangleShape tableBackground;

  //font
  sf::Font font;

  //text
  sf::Text playerLabel;
  sf::Text bankerLabel;

public:
  GameScreen() : playerLabel(font), bankerLabel(font)
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(35,107,43)); //casino green

    //load the font firectly from the file
    if (!font.openFromFile("assets/8bitOperatorPlus8-Regular.ttf"))
    {
      std::cerr << "failed to load font from assets folder!\n";
    }

    playerLabel.setFont(font);
    playerLabel.setString("PLAYER");
    playerLabel.setCharacterSize(80);

    bankerLabel.setFont(font);
    bankerLabel.setString("BANKER");
    bankerLabel.setCharacterSize(80);

    //center their origins so they balance perfectly
    UI::centerOrigin(playerLabel);
    UI::centerOrigin(bankerLabel);

    //snap them to their zones
    //we pass a custom yOffset to put the labels slightly above/below
    //where the cards will go
    UI::placeAtPlayerHand(playerLabel, 400.0f); // 100px from the bottom
    UI::placeAtBankerHand(bankerLabel, 400.0f); // 100px from the bottom
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

    //draw cards, chips, etc
    window.draw(playerLabel);
    window.draw(bankerLabel);
  }
  
  ScreenState getNextState() const override
  {
    return nextState;
  }
};
