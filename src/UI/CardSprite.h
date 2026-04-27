#pragma once
#include <SFML/Graphics.hpp>
#include "../logic/card.h"

class CardSprite
{
private:
  // Pointer to your logic class
  Card* m_card; 

  // The SFML 3 visual representation
  sf::Sprite m_sprite;

  // Pointers to the pre-loaded textures
  const sf::Texture* m_frontTexture;
  const sf::Texture* m_backTexture;

public:
  // Constructor takes the logic card, plus the textures for its front and back
  CardSprite(Card& card, const sf::Texture& frontTex, const sf::Texture& backTex)
    : m_card(&card), 
    m_sprite(card.is_flipped() ? backTex : frontTex),
    m_frontTexture(&frontTex), 
    m_backTexture(&backTex)
  {
    // Initialize the sprite to show the correct side immediately
    update(); 
  }

  // Call this in your game loop so the visual matches the logic state
  void update()
  {
    if (!m_card) return;

    // Assuming is_flipped() == true means showing the back of the card.
    // The 'true' parameter in setTexture resizes the sprite to fit the new texture's dimensions.
    if (m_card->is_flipped())
    {
      m_sprite.setTexture(*m_backTexture, true); 
    }
    else
  {
      m_sprite.setTexture(*m_frontTexture, true);
    }
  }

  // --- SFML 3 Rendering & Transforms ---
  // SFML 3 prefers sf::RenderTarget over RenderWindow for broader compatibility
  void draw(sf::RenderTarget& target) const
  {
    target.draw(m_sprite);
  }

  // SFML 3 heavily relies on sf::Vector2f for all coordinate math
  void setPosition(sf::Vector2f pos) 
  { 
    m_sprite.setPosition(pos); 
  }

  void setScale(sf::Vector2f scale) 
  { 
    m_sprite.setScale(scale); 
  }

  void setOrigin(sf::Vector2f origin) 
  { 
    m_sprite.setOrigin(origin); 
  }

  sf::Vector2f getPosition() const 
  {
    return m_sprite.getPosition();
  }

  // Useful for UIHelper functions (like centerOrigin)
  sf::Vector2f getLocalSize() const 
  {
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    return sf::Vector2f(bounds.size.x, bounds.size.y); // SFML 3 uses .size for FloatRects
  }

  // Useful for your getHoveredElement bounds checking
  sf::FloatRect getGlobalBounds() const 
  { 
    return m_sprite.getGlobalBounds(); 
  }

  // Expose the raw sprite for UIHelper functions
  sf::Sprite& getSFMLSprite() { return m_sprite; }
  const sf::Sprite& getSFMLSprite() const { return m_sprite; }

  // Retrieve the underlying logical card if needed
  Card* getLogicalCard() const { return m_card; }
};
