#pragma once
#include <SFML/Graphics.hpp>

class Background {
private:
  sf::Sprite m_sprite;
  float m_imageWidth;

public:
  // Pass the texture directly from your Asset Manager
  Background(const sf::Texture& texture, float scale) 
  : m_sprite(texture)
  {
    const float screenWidth = 1920.0f;
    const float screenHeight = 1080.0f;

    // 1. Get the original size of the texture
    sf::Vector2f textureSize(texture.getSize());

    // 2. Calculate the required scale factors
    float scaleX = screenWidth / textureSize.x;
    float scaleY = screenHeight / textureSize.y;

    /* --- CHOOSE ONE OF THE FOLLOWING SCALING METHODS --- */

    // OPTION A: Stretch to perfectly fit the 1920x1080 screen (ignores aspect ratio)
    //m_sprite.setScale({scaleX, scaleY}); 

    // OPTION B: Scale to cover the screen but maintain aspect ratio 
    float uniformScale = std::max(scaleX, scaleY);
    m_sprite.setScale({uniformScale, uniformScale});

    /* --------------------------------------------------- */

    // 3. Get the scaled width
    m_imageWidth = m_sprite.getGlobalBounds().size.x;

    // 4. Position it at the top-left corner
    m_sprite.setPosition({0.0f, 0.0f});
  }

  // Standard draw call
  void draw(sf::RenderTarget& target)
  {
    target.draw(m_sprite);
  }
};
