#pragma once
#include <SFML/Graphics.hpp>
#include <string>


// inheriting from sf::Drawable to allow button class to be drawed to screen
// allows you to treat the button class as a native SFML shape
class Button : public sf::Drawable, public sf::Transformable
{
private:
  sf::RectangleShape m_background;
  sf::Text m_label;

  // This allows you to use window.draw(myButton) directly
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override
  {
    // Apply the Button's transform (position, rotation, scale) to its children
    states.transform *= getTransform();
    target.draw(m_background, states);
    target.draw(m_label, states);
  }
  // Internal helper to re-center the label whenever text or size changes
  void centerLabel()
  {
    sf::FloatRect textBounds = m_label.getLocalBounds();
    // SFML 3 text centering: center = position + origin
    m_label.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, 
                       textBounds.position.y + textBounds.size.y / 2.0f});
    
    // Since Button inherits from Transformable, we leave its position at 0,0 
    // relative to the Button's own coordinate system.
    m_label.setPosition({0.0f, 0.0f});
  }

public:
  // Constructor handles all the boilerplate styling
  Button(const std::string& text, const sf::Font& font, sf::Vector2f size = {400.0f, 150.0f})
        : m_label(font, text, 80) // Modern SFML 3 constructor
    {
        // 1. Setup Rectangle
        m_background.setSize(size);
        m_background.setFillColor(sf::Color::White);
        m_background.setOutlineColor(sf::Color::Black);
        m_background.setOutlineThickness(5.0f);

        // Center the background relative to the Button's origin
        m_background.setOrigin(size / 2.0f);

        // 2. Setup Text
        m_label.setFillColor(sf::Color::Black);
        centerLabel();
    }

  // Optional: Getters to change colors later if you want hover effects
  void setFillColor(sf::Color color) { m_background.setFillColor(color); }
  void setTextColor(sf::Color color) { m_label.setFillColor(color); }
};
