#pragma once
#include <SFML/Graphics.hpp>
#include "../core/AssetManager.h"
#include "../core/chips/ChipColors.h"

enum class ChipStackSize { Single, Double, Triple, Lots };

class ChipSprite
{
private:
  sf::Sprite    m_sprite;
  int           m_value;
  int           m_count;
  ChipColor     m_color;
  ChipStackSize m_stack;


  static sf::IntRect getTextureRect(ChipColor color, ChipStackSize stack)
  {
    // All chips share the same left/width per stack, only top/height changes per color
    switch (stack)
    {
      case ChipStackSize::Single:
        switch (color)
        {
          case ChipColor::LightBlue:   return sf::IntRect({0, 64}, {6, 8});
          case ChipColor::Yellow: return sf::IntRect({0, 32}, {6, 8});
          case ChipColor::Red:    return sf::IntRect({0, 48},  {6, 8});
          case ChipColor::Blue:  return sf::IntRect({0, 16},  {6, 8});
        }
      case ChipStackSize::Double:
        switch (color)
        {
          case ChipColor::LightBlue:   return sf::IntRect({8, 62},  {8, 9});
          case ChipColor::Yellow: return sf::IntRect({8, 30},   {8, 9});
          case ChipColor::Red:    return sf::IntRect({8, 46},   {8, 9});
          case ChipColor::Blue:  return sf::IntRect({8, 14},   {8, 9});
        }
      case ChipStackSize::Triple:
        switch (color)
        {
          case ChipColor::LightBlue:   return sf::IntRect({15, 60}, {9, 11});
          case ChipColor::Yellow: return sf::IntRect({15, 28},  {9, 11});
          case ChipColor::Red:    return sf::IntRect({15, 44},  {9, 11});
          case ChipColor::Blue:  return sf::IntRect({15, 12},  {9, 11});
        }
      case ChipStackSize::Lots:
        switch (color)
        {
          case ChipColor::LightBlue:   return sf::IntRect({23, 56},  {17, 15});
          case ChipColor::Yellow: return sf::IntRect({23, 24},  {17, 15});
          case ChipColor::Red:    return sf::IntRect({23, 40},  {17, 15});
          case ChipColor::Blue:  return sf::IntRect({23, 8},  {17, 15});
        }
    }
    return sf::IntRect({0, 0}, {6, 8}); // fallback
  }

  static ChipColor colorFromValue(int value)
  {
    if      (value >= 100) return ChipColor::Blue;
    else if (value >= 25)  return ChipColor::Yellow;
    else if (value >= 5)   return ChipColor::Red;
    else                   return ChipColor::LightBlue;
  }

  static ChipStackSize stackFromAmount(int amount)
  {
    if      (amount >= 9) return ChipStackSize::Lots;
    else if (amount >= 3) return ChipStackSize::Triple;
    else if (amount == 2) return ChipStackSize::Double; 
    else                  return ChipStackSize::Single;
  }
  void updateVisuals()
  {
    m_stack = stackFromAmount(m_count);
    m_sprite.setTextureRect(getTextureRect(m_color, m_stack));
  }

public:
  // Pass in total chip VALUE (e.g. 500) and how many chips to display
  ChipSprite(int chipValue, int chipAmount, AssetManager& assets)
    : m_sprite(assets.getTexture("chips")),
    m_value(chipValue),
    m_count(chipAmount),
    m_color(colorFromValue(chipValue)),
    m_stack(stackFromAmount(chipAmount))
  {
    updateVisuals();
    m_sprite.setScale({8.f, 8.f}); // scale up so tiny chips are visible
  }

  // Getters
  sf::Vector2f  getPosition()  const { return m_sprite.getPosition(); }
  sf::FloatRect getGlobalBounds() const { return m_sprite.getGlobalBounds(); }
  sf::Vector2f  getLocalSize() const
  {
    auto b = m_sprite.getLocalBounds(); 
    return {b.size.x, b.size.y}; 
  }
  int getChipValue() const { return m_value; }
  int getChipCount() const { return m_count; }
  int getColor()     const { return static_cast<int>(m_color); }

  // Setters
  void setPosition(sf::Vector2f pos) { m_sprite.setPosition(pos); }
  void setScale(sf::Vector2f scale)  { m_sprite.setScale(scale); }
  void setOrigin(sf::Vector2f origin){ m_sprite.setOrigin(origin); }
  
  void setChipCount(int count)
  {
    m_count = count; 
    updateVisuals(); 
  }

  void draw(sf::RenderTarget& target) const
  {
    if (m_count > 0)
    {
      target.draw(m_sprite);
    }
  }


};
