#pragma once
#include "PlayerHUD.h"
#include "../core/player.h"
#include <SFML/Graphics.hpp>

struct PlayerTab
{
  PlayerHUD hud;
  sf::FloatRect bounds; // Used for mouse hover detection
  bool isEmpty;

  PlayerTab(const sf::Font& usernameFont, const sf::Font& coinsFont, const sf::Texture& hudTex, const Player& player, sf::Vector2f hudPos)
    : hud(player, hudTex, usernameFont, coinsFont, hudPos),
      bounds(hudPos, sf::Vector2f(PlayerHUD::WIDTH, PlayerHUD::HEIGHT)), 
      isEmpty(true)
  {}
};
