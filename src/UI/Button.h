#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

// Defines the visual style and crop region of the button from the main texture
enum class ButtonType
{
  WhiteButton1,
  WhiteButton2,
  MenuStart,
  MenuSettings,
  MenuExit,
  MenuDefault,
  LeftArrow,
  RightArrow,
};

class Button
{
private:
  sf::Sprite m_sprite;
  std::optional<sf::Text> m_label; 

  sf::Color normalColor = sf::Color::White;
  sf::Color hoverColor  = sf::Color(200, 200, 200);
  sf::Color activeColor = sf::Color(150, 150, 150);

  float m_scale;

  // Helper method to handle the shared sprite configuration logic for all button types
  void setupSprite(ButtonType buttonType, float scale, sf::Vector2f pos)
  {
    m_scale = scale;
    sf::IntRect cropRect;
    
    // Determine the specific region of the texture to use
    switch (buttonType)
    {
      case ButtonType::WhiteButton1: cropRect = sf::IntRect({272, 400}, {48, 16}); break;
      case ButtonType::WhiteButton2: cropRect = sf::IntRect({335, 400}, {33, 16}); break;
      case ButtonType::MenuStart: cropRect = sf::IntRect({126, 195}, {54, 17}); break;
      case ButtonType::MenuSettings: cropRect = sf::IntRect({126, 215}, {54, 17}); break;
      case ButtonType::MenuExit: cropRect = sf::IntRect({126, 235}, {54, 17}); break;
      case ButtonType::MenuDefault: cropRect = sf::IntRect({99, 35}, {90, 27}); break;
      case ButtonType::LeftArrow: cropRect = sf::IntRect({52, 226}, {8, 12}); break;
      case ButtonType::RightArrow: cropRect = sf::IntRect({70, 226}, {8, 12}); break;
    }
    
    m_sprite.setTextureRect(cropRect);

    // Center the sprite's origin so it draws outward from its exact middle coordinates
    sf::FloatRect spriteBounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({spriteBounds.position.x + spriteBounds.size.x / 2.0f, 
                        spriteBounds.position.y + spriteBounds.size.y / 2.0f});

    m_sprite.setScale({scale, scale});
    m_sprite.setPosition(pos);
    m_sprite.setColor(normalColor);
  }

public:
  // Constructor for an Icon-Only Button
  // Used when a button does not require text, bypassing the need for an sf::Font
  Button(const sf::Texture& tex, ButtonType buttonType, float scale, sf::Vector2f pos)
  : m_sprite(tex)
  {
    setupSprite(buttonType, scale, pos);
  }

  // Constructor for a Text Button
  // Automatically calculates text size and alignment based on the sprite's scale
  Button(const sf::Texture& tex, const sf::Font& font, sf::Color color, ButtonType buttonType, 
         const std::string& textStr, float scale, sf::Vector2f pos)
  : m_sprite(tex)
  {
    setupSprite(buttonType, scale, pos);

    // Initialize the optional sf::Text object
    m_label.emplace(font);
    m_label->setString(textStr);
    m_label->setFillColor(color); //text color

    // Calculate a dynamic character size relative to the button's visual height
    float visualHeight = m_sprite.getGlobalBounds().size.y;
    m_label->setCharacterSize(static_cast<unsigned int>(visualHeight * 0.6f));

    // Center the text's origin to align perfectly with the centered sprite
    sf::FloatRect textBounds = m_label->getLocalBounds();
    m_label->setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, 
                        textBounds.position.y + textBounds.size.y / 2.0f});

    m_label->setPosition(pos);
  }

  // Overrides the default interaction colors for this specific button instance
  void setColors(sf::Color normal, sf::Color hover, sf::Color active)
  {
      normalColor = normal;
      hoverColor = hover;
      activeColor = active;
      m_sprite.setColor(normalColor); 
  }

  void setPosition(sf::Vector2f pos)
  {
    m_sprite.setPosition(pos);
    if (m_label) 
    {
        m_label->setPosition(pos);
    }
  }

  void setScale(float scale)
  {
    m_scale = scale;
    m_sprite.setScale({scale, scale});
    
    if (m_label)
    {
        // Recalculate text size dynamically based on the new scale
        float visualHeight = m_sprite.getGlobalBounds().size.y;
        m_label->setCharacterSize(static_cast<unsigned int>(visualHeight * 0.6f));
        
        // Recenter the origin
        sf::FloatRect textBounds = m_label->getLocalBounds();
        m_label->setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, 
                            textBounds.position.y + textBounds.size.y / 2.0f});
    }
  }

  // Processes mouse movement to update the visual state (hovering or held)
  void update(sf::Vector2f mousePos)
  {
    if (m_sprite.getGlobalBounds().contains(mousePos))
    {
      if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        m_sprite.setColor(activeColor);
      else
        m_sprite.setColor(hoverColor);
    }
    else
    {
      m_sprite.setColor(normalColor);
    }
  }

  // Detects valid button clicks, triggering only when the left mouse button is released
  bool isClicked(sf::Vector2f mousePos, const sf::Event& event)
  {
    if (m_sprite.getGlobalBounds().contains(mousePos))
    {
      if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
      {
        if (mouseReleased->button == sf::Mouse::Button::Left) 
        {
          return true;
        }
      }
    }
    return false;
  }

  // Draws the button sprite and its label (if the label exists) to the target window
  void draw(sf::RenderWindow& window) const
  {
    window.draw(m_sprite);

    if (m_label) 
    {
        window.draw(*m_label);
    }
  }
};

