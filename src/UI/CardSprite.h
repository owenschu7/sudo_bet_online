#pragma once
#include <SFML/Graphics.hpp>
#include "../logic/card.h"
#include "../core/AssetManager.h"

class CardSprite
{
private:
  Card               m_card;
  AssetManager*      m_assets;
  const sf::Texture* m_frontTexture;
  const sf::Texture* m_backTexture;
  sf::Sprite         m_sprite;

public:
  CardSprite(Card card, AssetManager& assets)
    : m_card(card),
    m_assets(&assets),
    m_frontTexture(&assets.getTexture(m_card.getTextureKey())),
    m_backTexture(&assets.getTexture("cardBack")),
    m_sprite(m_card.is_flipped() ? *m_backTexture : *m_frontTexture)
  {}

  void update()
  {
    if (m_card.is_flipped())
      m_sprite.setTexture(*m_backTexture);
    else
      m_sprite.setTexture(*m_frontTexture);
  }

  void setCard(Card newCard)
  {
    m_card = newCard;
    m_frontTexture = &m_assets->getTexture(m_card.getTextureKey());
    update();
  }

  // 0 value means empty slot
  void clearCard()
  {
    m_card = Card(0, 'n');
    m_sprite.setTexture(*m_backTexture);
  }

  //bool hasCard() const { return m_card.get_value() != 0; }

  void flip()
  {
    m_card.set_flipped(!m_card.is_flipped());
    update();
  }

  // --- Rendering ---
  void draw(sf::RenderTarget& target) const { target.draw(m_sprite); }

  // --- Transforms ---
  void setPosition(sf::Vector2f pos)  { m_sprite.setPosition(pos); }
  void setScale(sf::Vector2f scale)   { m_sprite.setScale(scale); }
  void setOrigin(sf::Vector2f origin) { m_sprite.setOrigin(origin); }
  sf::Vector2f getPosition() const    { return m_sprite.getPosition(); }

  // --- Bounds ---
  sf::Vector2f getLocalSize() const
  {
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    return sf::Vector2f(bounds.size.x, bounds.size.y);
  }
  sf::FloatRect getGlobalBounds() const { return m_sprite.getGlobalBounds(); }

  // --- Accessors ---
  sf::Sprite&       getSFMLSprite()         { return m_sprite; }
  const sf::Sprite& getSFMLSprite()   const { return m_sprite; }
  Card&             getCard()               { return m_card; }
  const Card&       getCard()         const { return m_card; }
};
