#pragma once
#include <SFML/Graphics.hpp>

//an enum to help us switch between screens easily
enum class ScreenState
{
  None,
  MainMenu,
  Settings,
  Game
};

class Screen
{
public:
  virtual ~Screen() = default;

  // every screen must implement these three functions
  virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
  virtual void update() = 0;
  virtual void draw(sf::RenderWindow& window) = 0;

  // this allows a screen to tell the main loop it's time to switch
  virtual ScreenState getNextState() const = 0;
};
