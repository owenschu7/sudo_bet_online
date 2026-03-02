#pragma once
#include <SFML/Graphics.hpp>

// creates a dedicated toolbox named UI that you can open from anywhere in your game

namespace UI
{
  //the virtual canvas size
  inline const float SCREEN_W = 1920.0f;
  inline const float SCREEN_H = 1080.0f;

  //the centering functions
  inline void centerOrigin(sf::Text &text)
  {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
  }
  
  inline void centerOrigin(sf::Shape &shape)
  {
    sf::FloatRect bounds = shape.getLocalBounds();
    shape.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
  }

  //table anchords (baccarat specific)
  inline void placeAtPlayerHand(sf::Transformable &object, float yPos)
  {
    // SCREEN_W * 0.35f puts the object 35% of the way across the screen (Left Side)
    object.setPosition({SCREEN_W * 0.35f, yPos});
  }
  inline void placeAtBankerHand(sf::Transformable& object, float yPos)
  {
    // SCREEN_W * 0.65f puts the object 65% of the way across the screen (Right Side)
    object.setPosition({SCREEN_W * 0.65f, yPos});
  }
}

