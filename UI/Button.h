#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// inheriting from sf::Drawable to allow button class to be drawed to screen
// allows you to treat the button class as a native SFML shape
class Button : public sf::Drawable
{
private:
    sf::RectangleShape background;
    sf::Text label;

    // This allows you to use window.draw(myButton) directly
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(background, states);
        target.draw(label, states);
    }

public:
    // Constructor handles all the boilerplate styling
    Button(const std::string& text, const sf::Font& font, sf::Vector2f size = {400.0f, 150.0f})
    {
        // 1. Setup Rectangle
        background.setSize(size);
        background.setFillColor(sf::Color::White);
        background.setOutlineColor(sf::Color::Black);
        background.setOutlineThickness(5.0f);

        // Center origin of background
        sf::FloatRect bgBounds = background.getLocalBounds();
        background.setOrigin({bgBounds.width / 2.0f, bgBounds.height / 2.0f});

        // 2. Setup Text
        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(80);
        label.setFillColor(sf::Color::Black);

        // Center origin of text (the math is slightly different for sf::Text to account for ascenders/descenders)
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin({textBounds.left + textBounds.width / 2.0f, 
                         textBounds.top + textBounds.height / 2.0f});
    }

    // This makes it compatible with your UI::placeLeft template!
    void setPosition(sf::Vector2f pos)
    {
        background.setPosition(pos);
        label.setPosition(pos);
    }

    // Optional: Getters to change colors later if you want hover effects
    void setFillColor(sf::Color color) { background.setFillColor(color); }
    void setTextColor(sf::Color color) { label.setFillColor(color); }
};
