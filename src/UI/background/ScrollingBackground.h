#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

class ScrollingBackground {
private:
  sf::Sprite sprite1;
  sf::Sprite sprite2;
  float scrollSpeed;
  float imageWidth;

public:
  // Pass the texture directly from your Asset Manager
  ScrollingBackground(const sf::Texture& texture, float speed, float scale) 
  : sprite1(texture), sprite2(texture), scrollSpeed(speed) 
  {
    //set scale
    // 1. Scale the sprites up first!
    sprite1.setScale({scale, scale});
    sprite2.setScale({scale, scale});

    // 2. NOW get the width (getGlobalBounds automatically accounts for the scale)
    imageWidth = sprite1.getGlobalBounds().size.x;

    // 3. Position them
    sprite1.setPosition({0.0f, 0.0f});
    sprite2.setPosition({imageWidth, 0.0f});  }

    // Pass sf::Time directly for modern SFML practice
    void update(sf::Time dt)
    {
      float offset = scrollSpeed * dt.asSeconds();

      // move() also requires sf::Vector2f brace initialization
      sprite1.move({-offset, 0.0f});
      sprite2.move({-offset, 0.0f});

      // If sprite1 moves completely off the left edge, snap it to the right of sprite2
      if (sprite1.getPosition().x + imageWidth <= 0.0f) {
        sprite1.setPosition({sprite2.getPosition().x + imageWidth, 0.0f});
      }

      // If sprite2 moves completely off the left edge, snap it to the right of sprite1
      if (sprite2.getPosition().x + imageWidth <= 0.0f) {
        sprite2.setPosition({sprite1.getPosition().x + imageWidth, 0.0f});
      }
    }

  // Standard draw call
  void draw(sf::RenderTarget& target) {
    target.draw(sprite1);
    target.draw(sprite2);
  }
};
