#pragma once
#include <SFML/Graphics.hpp>
#include "../core/AssetManager.h"
#include <string>

// This class owns the text and handles its logic
class CursorUI {
private:
    sf::Text m_text;
    bool m_isVisible;

public:
    CursorUI(AssetManager &assetMan)
      : m_isVisible(false),
      m_text(assetMan.getFont("gameFont"))
    {
        // 2. Setup the text object
        m_text.setCharacterSize(48);
        m_text.setFillColor(sf::Color::Yellow);
        m_text.setOutlineColor(sf::Color::Black);
        m_text.setOutlineThickness(2.0f);
    }

    // Call this when the user selects or deselects chips
    void updateMultiplier(int count) 
    {
        if (count > 0) {
            m_text.setString("x" + std::to_string(count));
            m_isVisible = true;
        } else {
            m_isVisible = false; // Hide if nothing is selected
        }
    }

    // Call this EVERY FRAME in your main update loop
    void updatePosition(const sf::RenderWindow& window) 
    {
        if (!m_isVisible) return;

        // 1. Get the mouse position relative to the window
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

        // 2. Convert pixel coordinates to world coordinates 
        // (Crucial if you use sf::View to move a camera around)
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

        // 3. Add an offset so the cursor doesn't cover the text
        sf::Vector2f offset(-45.0f, -45.0f); // 15 pixels right, 15 pixels down

        // 4. Update the text position
        m_text.setPosition(worldPos + offset);
    }

    // Call this in your render loop
    void draw(sf::RenderTarget& target) 
    {
        if (m_isVisible) {
            target.draw(m_text);
        }
    }
};
