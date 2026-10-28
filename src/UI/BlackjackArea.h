#include <SFML/Graphics.hpp>
#include <vector>

#include "Area.h"
#include "Label.h"
#include "Button.h"
#include "CardSprite.h"
#include "../core/AssetManager.h"
#include "../logic/card.h"

class BlackjackArea : public Area
{
  private:
    sf::RectangleShape m_dealerCardsZone;

    // --- NEW: Betting Area & Decor Shapes ---
    sf::CircleShape m_bettingCircleOuter;
    sf::CircleShape m_bettingCircleInner;
    sf::RectangleShape m_insuranceLine;
    
    // You can use your Label class here for table text!
    // Label m_blackjackPayoutText; 
    // Label m_dealerRulesText;
    // Label m_insuranceText;

    std::vector<CardSprite> m_dealerCards;
    std::vector<CardSprite> m_playerCards;

    AssetManager *m_assetMan;

  public:
    BlackjackArea(AssetManager& assetMan, float x, float y, float w, float h)
      : Area(x, y, w, h)
  {
    m_assetMan = &assetMan;

    sf::Color blue(0, 0, 255);
    sf::Color red(255, 0, 0);
    sf::Color lightRed(255, 200, 200); 
    sf::Color lightBlue(200, 235, 255);
    sf::Color bananaYellow(255, 235, 100);
    sf::Color softYellow(255, 245, 150);

    // --- NEW: Initialize Betting Circle ---
    // Outer Ring
    m_bettingCircleOuter.setFillColor(sf::Color::Transparent);
    m_bettingCircleOuter.setOutlineThickness(4.f);
    m_bettingCircleOuter.setOutlineColor(sf::Color(0, 75, 0)); 

    // Inner Ring
    m_bettingCircleInner.setFillColor(sf::Color::Transparent);
    m_bettingCircleInner.setOutlineThickness(2.f);
    m_bettingCircleInner.setOutlineColor(sf::Color(0, 125, 0));

    // --- NEW: Initialize Insurance Line ---
    m_insuranceLine.setFillColor(sf::Color(0, 125, 0));

    updateInternalLayout();
  }

    void updateInternalLayout() override
    {
      // 1. Set default variables that change based on state
      float labelYOffset = 0.0f;
      float cardYOffset = 0.0f;
      float cardScale = 1.0f;
      float labelScale = 1.0f;
      unsigned int charSize = 50;
      float buttonScale = 7.0f;
      float cardOffset = 0.f;

      // 2. Assign the specific layout numbers based on the menu state
      if (m_currentWindowState == WindowStates::MenuClosed) 
      {
        m_x = 350.f;
        m_y = 75.f;
        m_w = 1200.f;
        m_h = 545.f;
        // ... (other variables)
      }
      else if (m_currentWindowState == WindowStates::MenuOpen) 
      {
        m_x = 790.f;
        m_y = 75.f;
        m_w = 1000.f;
        m_h = 545.f;
        // ... (other variables)
      }
      else
      {
        return; 
      }

      // 3. Calculate the shared positions
      float centerX = m_x + (m_w / 2.0f);
      float bankerX = m_x + (m_w / 4.0f);
      float playerX = m_x + (m_w * 0.75f);
      float labelY = (m_h / 5.f) * 5; 

      // banker player zone LABELS
      float zoneLabelY = m_h / 4.f;
      float zoneLabelBankerX = m_w * 0.25f;
      float zoneLabelPlayerX = m_w * 0.75f;

      // --- NEW: Position the Betting Circles ---
      float betRadiusOuter = 100.f;
      float betRadiusInner = 80.f;
      
      // Set origins to the center of the circles so they position perfectly
      m_bettingCircleOuter.setRadius(betRadiusOuter);
      m_bettingCircleOuter.setOrigin({betRadiusOuter, betRadiusOuter});
      
      m_bettingCircleInner.setRadius(betRadiusInner);
      m_bettingCircleInner.setOrigin({betRadiusInner, betRadiusInner});

      // Place the betting circle in the lower-middle portion of the felt
      sf::Vector2f betPos(centerX, m_y + (m_h * 0.75f));
      m_bettingCircleOuter.setPosition(betPos);
      m_bettingCircleInner.setPosition(betPos);

      // --- NEW: Position the Insurance Line ---
      // A line spanning most of the width, sitting above the betting area
      float lineWidth = m_w * 0.8f;
      float lineHeight = 4.f;
      m_insuranceLine.setSize({lineWidth, lineHeight});
      m_insuranceLine.setOrigin({lineWidth / 2.f, lineHeight / 2.f});
      m_insuranceLine.setPosition({centerX, m_y + (m_h * 0.45f)});


      // 4. Apply positions to labels and base area
      m_area.setPosition({m_x, m_y});
      m_area.setSize({m_w, m_h});
    }

    void drawGameElements(sf::RenderWindow &window) override
    {
      // --- NEW: Draw Table Graphics ---
      window.draw(m_insuranceLine);
      window.draw(m_bettingCircleOuter);
      window.draw(m_bettingCircleInner);

      // (If you implement Labels, draw them here)
      // m_blackjackPayoutText.draw(window);
      // m_dealerRulesText.draw(window);
    }

    void flipAllCards()
    {
    }

    // for dev
    sf::FloatRect getFeltBounds() const
    {
      return m_area.getGlobalBounds();
    }

    //CARDS!!!
    void addPlayerCard(Card card)
    {
      m_playerCards.emplace_back(card, *m_assetMan);

      // Set the origin to the center so they rotate/scale nicely
      m_playerCards.back().setOrigin(m_playerCards.back().getLocalSize() / 2.f);

      updateInternalLayout(); // Reposition everything
    }

    void addDealerCard(Card card)
    {
      updateInternalLayout();
    }

    void clearHands()
    {
      m_playerCards.clear();
      m_dealerCards.clear();
    }
};
