#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Label
{
  sf::Text textObj;
  bool isCentered = false;

  // 1. Constructor: Requires the font immediately (SFML 3 rule!)
  Label(const sf::Font& font) : textObj(font) {}

  // 2. Setup function to style and position the text
  void setup(const std::string& text, unsigned int characterSize, sf::Color color, sf::Vector2f pos, bool centerAlign = true)
  {
    isCentered = centerAlign;

    textObj.setString(text);
    textObj.setCharacterSize(characterSize);
    textObj.setFillColor(color);

    // Automatically handle the origin math based on alignment
    recalculateOrigin();
    
    textObj.setPosition(pos);
  }

  // 3. Easy way to update dynamic text (like a typing username!)
  void setString(const std::string& newText)
  {
    textObj.setString(newText);
    
    // If the text gets longer or shorter, we MUST recalculate the center!
    if (isCentered) 
    {
      recalculateOrigin();
    }
  }

  // Helper function to handle the annoying math
  void recalculateOrigin()
  {
    if (isCentered)
    {
      sf::FloatRect bounds = textObj.getLocalBounds();
      // Center the origin perfectly in the middle of the text
      textObj.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
    }
    else
    {
      // Reset to top-left corner
      textObj.setOrigin({0.0f, 0.0f}); 
    }
  }

  // 4. Draw function
  void draw(sf::RenderWindow& window)
  {
    window.draw(textObj);
  }
};
