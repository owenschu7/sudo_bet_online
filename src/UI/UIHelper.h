#pragma once
#include <SFML/Graphics.hpp>

// creates a dedicated toolbox named UI that you can open from anywhere in your game
//
namespace UI
{
  // The virtual canvas size
  inline const float SCREEN_W = 1920.0f;
  inline const float SCREEN_H = 1080.0f;

  // 1. The Template for Centering (Replaces the 3 old functions)
  template <typename T>
  inline void centerOrigin(T &object)
  {
    sf::FloatRect bounds = object.getLocalBounds();
    object.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///center placement functions
///

  // 2. Menu Anchors
  inline void placeInCenter(sf::Transformable &object)
  {
    object.setPosition({SCREEN_W / 2.0f, SCREEN_H / 2.0f});
  }

  inline void placeCentered(sf::Transformable &object, float yPos)
  {
    object.setPosition({SCREEN_W / 2.0f, yPos});
  }

  // Offsets the center position to the LEFT by a specific number of pixels
  inline void placeCenteredoffsetLeft(sf::Transformable &object, float yPos, float offsetPixels)
  {
    object.setPosition({(SCREEN_W / 2.0f) - offsetPixels, yPos});
  }

  // Offsets the center position to the RIGHT by a specific number of pixels
  inline void placeCenteredoffsetRight(sf::Transformable &object, float yPos, float offsetPixels)
  {
    object.setPosition({(SCREEN_W / 2.0f) + offsetPixels, yPos});
  }

//////////////////////////////////////////////////////////////////////////////////////////////////
///hand placement (where the cards sit)
///

  // Table Anchors 
  inline void placeAtPlayerHandX(sf::Transformable &object, float yPos)
  {
    object.setPosition({SCREEN_W * 0.35f, yPos});
  }

  inline void placeAtBankerHandX(sf::Transformable& object, float yPos)
  {
    object.setPosition({SCREEN_W * 0.65f, yPos});
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
///card placement logic
///
  inline const float CARD_OFFSET_X = 160.0f;

  inline void placeCardInHand(sf::Transformable &object, float startX, float baseY, int cardIndex)
  {
    float calculatedX = startX + (cardIndex * CARD_OFFSET_X); 
    object.setPosition({calculatedX, baseY});
  }

  // 5. Place All Cards (Updated to use placeCardInHand)
  inline void placeAllCards(const std::vector<std::unique_ptr<sf::Sprite>>& cards, float baseY)
  {
    float playerStartX = SCREEN_W * 0.35f - 150.0f;
    float bankerStartX = SCREEN_W * 0.65f - 150.0f;

    for (size_t i = 0; i < cards.size(); ++i)
    {
      if (!cards[i]) continue; // Safety check

      if (i < 3) 
      {
        // Player cards (indices 0, 1, 2)
        placeCardInHand(*cards[i], playerStartX, baseY, i);
      }
      else 
      {
        // Banker cards (indices 3, 4, 5)
        placeCardInHand(*cards[i], bankerStartX, baseY, i - 3); 
      }
    }
  }

  // Left/Right Anchors (Updated to use sf::Transformable)
  inline void placeLeft(sf::Transformable &object, float yPos)
  {
    object.setPosition({SCREEN_W * 0.20f, yPos}); // 20% of screen width
  }

  inline void placeRight(sf::Transformable &object, float yPos)
  {
    object.setPosition({SCREEN_W * 0.80f, yPos}); // 80% of screen width
  }

}

