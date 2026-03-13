#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>

class AvailableTablesScreen : public Screen
{
private:
  //when we want to change screens (to main menu for example) we change this variable to say StreenState::MainMenu
  ScreenState nextState = ScreenState::None;

  //things go here
  sf::RectangleShape tableBackground;

public:
  AvailableTablesScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(20, 20, 50));

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
  }
  
  ScreenState getNextState() const override
  {
    return nextState;
  }
};
