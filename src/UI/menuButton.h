#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Button
{
  sf::RectangleShape box;
  sf::Text label;

  sf::Color normalColor = sf::Color(40, 150, 60);
  sf::Color hoverColor  = sf::Color(60, 170, 80);
  sf::Color activeColor = sf::Color(20, 130, 40);

  Button(const sf::Font& font) : label(font) {}

  void setup(const std::string& text, sf::Vector2f size, sf::Vector2f pos)
  {
    box.setSize(size);
    box.setPosition(pos);
    box.setFillColor(normalColor);

    label.setString(text);
    label.setCharacterSize(40);
    label.setFillColor(sf::Color::White);

    sf::FloatRect textRect = label.getLocalBounds();
    label.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
    label.setPosition({pos.x + size.x / 2.0f, pos.y + size.y / 2.0f});
  }

  void update(sf::Vector2f mousePos)
  {
    if (box.getGlobalBounds().contains(mousePos))
    {
      if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        box.setFillColor(activeColor); 
      else
        box.setFillColor(hoverColor);  
    }
    else
    {
      box.setFillColor(normalColor);   
    }
  }

  bool isClicked(sf::Vector2f mousePos, const sf::Event& event)
  {
    if (box.getGlobalBounds().contains(mousePos))
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
    window.draw(box);
    window.draw(label);
  }
};
