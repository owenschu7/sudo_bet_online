#pragma once
#include "../core/player.h"
#include <SFML/Graphics.hpp>

class PlayerHUD
{
public:
  // -------------------------------------------------------
  // Layout constants — change these to redesign the HUD
  // -------------------------------------------------------
  static constexpr float SPRITE_W    = 77.f;
  static constexpr float SPRITE_H    = 27.f;
  static constexpr float SCALE       = 1.4f;
  static constexpr float WIDTH       = SPRITE_W * SCALE;  // rendered width
  static constexpr float HEIGHT      = SPRITE_H * SCALE;  // rendered height
  static constexpr float TEXT_SIZE   = 24u;
  // text offsets relative to top-left of sprite
  static constexpr float USERNAME_OFFSET_X      = 10.f;  // pushes text past the avatar
  static constexpr float USERNAME_OFFSET_Y  = 10.f;
  static constexpr float CHIPS_OFFSET_Y     = 0.f;
  static constexpr float CHIPS_OFFSET_X     = 0.f;
  // -------------------------------------------------------

  PlayerHUD(const Player& player, const sf::Texture& hudTex, const sf::Font& font, sf::Vector2f pos)
    : m_player(&player),
    m_sprite(hudTex),
    m_usernameText(font),
    m_chipsText(font)
  {
    m_sprite.setScale({SCALE, SCALE});
    m_sprite.setPosition(pos);

    m_usernameText.setCharacterSize(TEXT_SIZE);
    m_usernameText.setFillColor(sf::Color::White);
    m_usernameText.setPosition({pos.x + USERNAME_OFFSET_X, pos.y + USERNAME_OFFSET_Y});

    m_chipsText.setCharacterSize(TEXT_SIZE);
    m_chipsText.setFillColor(sf::Color::Yellow);
    m_chipsText.setPosition({pos.x + CHIPS_OFFSET_Y, pos.y + CHIPS_OFFSET_Y});
  }

  void update()
  {
    if (!m_player) return;
    m_usernameText.setString(m_player->getUsername());
    m_chipsText.setString("$" + std::to_string(m_player->getChips()));
  }

  void draw(sf::RenderWindow& window) const
  {
    window.draw(m_sprite);
    window.draw(m_usernameText);
    window.draw(m_chipsText);
  }

  void setPlayer(const Player* newPlayer) { m_player = newPlayer; }
  void setPosition(sf::Vector2f pos)
  {
    m_sprite.setPosition(pos);
    m_usernameText.setPosition({pos.x + USERNAME_OFFSET_X, pos.y + USERNAME_OFFSET_Y});
    m_chipsText.setPosition({pos.x + CHIPS_OFFSET_X, pos.y + CHIPS_OFFSET_Y});
  }

private:
  const Player*  m_player;
  sf::Sprite     m_sprite;
  sf::Text       m_usernameText;
  sf::Text       m_chipsText;
};
