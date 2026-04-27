#pragma once
#include <SFML/Graphics.hpp>
#include <memory> // Required for unique_ptr
#include <string>

struct Button
{
  // We use unique_ptr so these can be "born" later in the setup()
  std::unique_ptr<sf::Sprite> sprite;
  std::unique_ptr<sf::Text> label;

  sf::Color normalColor = sf::Color::White; 
  sf::Color hoverColor  = sf::Color(200, 200, 200); 
  sf::Color activeColor = sf::Color(150, 150, 150);

  bool hasText;

  Button() {}

  void setup(const sf::Texture& tex, sf::IntRect textureRect, const sf::Font& font, 
             const std::string& text, float scaleFactor, unsigned int charSize, sf::Vector2f pos)
  {
    hasText = true;
    // 1. Create the objects now that we have the Texture and Font
    sprite = std::make_unique<sf::Sprite>(tex);
    label = std::make_unique<sf::Text>(font);

    // 2. Configure Sprite (Use -> instead of .)
    sprite->setTextureRect(textureRect); 
    sprite->setPosition(pos);

    sprite->setScale({scaleFactor, scaleFactor});
    float scaledW = textureRect.size.x * scaleFactor;
    float scaledH = textureRect.size.y * scaleFactor;
    label->setPosition({pos.x + scaledW / 2.0f, pos.y + scaledH / 2.0f});
    sprite->setColor(normalColor);

    // 3. Configure Text
    label->setString(text);
    label->setCharacterSize(charSize);  // <-- use it here
    label->setFillColor(sf::Color::White);

    // Center text math
    sf::FloatRect textBounds = label->getLocalBounds();
    label->setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, 
      textBounds.position.y + textBounds.size.y / 2.0f});

  }

  void setupNoText(const sf::Texture& tex, sf::IntRect textureRect, float scaleFactor, sf::Vector2f pos)
  {
    hasText = false;
    // 1. Create the objects now that we have the Texture and Font
    sprite = std::make_unique<sf::Sprite>(tex);

    // 2. Configure Sprite (Use -> instead of .)
    sprite->setTextureRect(textureRect); 
    sprite->setPosition(pos);

    sprite->setScale({scaleFactor, scaleFactor});
    sprite->setColor(normalColor);
  }


  void update(sf::Vector2f mousePos)
  {
    if (!sprite) return; // Safety check

    if (sprite->getGlobalBounds().contains(mousePos))
    {
      if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        sprite->setColor(activeColor);
      else
        sprite->setColor(hoverColor);
    }
    else
    {
      sprite->setColor(normalColor);
    }
  }

  bool isClicked(sf::Vector2f mousePos, const sf::Event& event)
  {
    if (!sprite) return false;

    if (sprite->getGlobalBounds().contains(mousePos))
    {
      if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
      {
        if (mouseReleased->button == sf::Mouse::Button::Left) return true;
      }
    }
    return false;
  }

  void draw(sf::RenderWindow& window)
  {
    if (sprite) window.draw(*sprite);
    if (label && hasText) window.draw(*label);
  }
};
