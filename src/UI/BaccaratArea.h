
#include <SFML/Graphics.hpp>
#include <vector>

#include "Area.h"
#include "textLabel.h"
#include "CardSprite.h"
#include "../core/AssetManager.h"

#include "../logic/card.h"

//Global constants

class BaccaratArea : public Area
{
  private:
    //card zone labels
    //this is where the cards are drawn to
    sf::RectangleShape m_bankerCardsZone;
    sf::RectangleShape m_playerCardsZone;
    Label m_bankerZoneLabel;
    Label m_playerZoneLabel;

    std::vector<CardSprite> m_playerCards;
    std::vector<CardSprite> m_bankerCards;

    // Betting zone placeholders 
    // FIX: these should be buttons
    Label m_bankerLabel;
    Label m_playerLabel;
    Label m_tieLabel;

    AssetManager *m_assetMan;

  public:
    BaccaratArea(AssetManager& assetMan, float x, float y, float w, float h)
      : Area(x, y, w, h),
      m_bankerLabel(assetMan.getFont("gameFont")),
      m_playerLabel(assetMan.getFont("gameFont")),
      m_tieLabel(assetMan.getFont("gameFont")),
      m_bankerZoneLabel(assetMan.getFont("gameFont")),
      m_playerZoneLabel(assetMan.getFont("gameFont"))
  {
    m_assetMan = &assetMan;
    const sf::Texture& uiTexture = assetMan.getTexture("BlackAndWhiteUI");

    sf::Color blue(0, 0, 255);
    sf::Color red(255, 0, 0);
    sf::Color lightRed(255, 200, 200); 
    sf::Color lightBlue(200, 235, 255);
    sf::Color bananaYellow(255, 235, 100);
    sf::Color softYellow(255, 245, 150);

    m_bankerZoneLabel.setup(
        "BANKER", 
        80,
        lightBlue,
        {0.0f, 0.0f},
        true
        );
    m_playerZoneLabel.setup(
        "PLAYER", 
        80,
        lightRed,
        {0.0f, 0.0f},
        true
        );

    m_bankerLabel.setupWithBanner(
        "BANKER", 
        80, lightBlue,
        {0,0},
        true,
        uiTexture,
        BannerType::WhiteBanner1,
        8.0f,
        {0.0f, 0.0f}
        );
    m_bankerLabel.setColor(blue);

    m_playerLabel.setupWithBanner(
        "PLAYER", 
        80, lightRed,
        {0,0},
        true,
        uiTexture,
        BannerType::WhiteBanner1,
        8.0f,
        {0.0f, 0.0f}
        );
    m_playerLabel.setColor(red);

    m_tieLabel.setupWithBanner(
        "TIE", 
        50, softYellow,
        {0,0},
        true,
        uiTexture,
        BannerType::WhiteBanner2,
        6.0f,
        {0.0f, 0.0f}
        );
    m_tieLabel.setColor(bananaYellow);

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
      float cardOffset = 0.f;

      // 2. Assign the specific layout numbers based on the menu state
      if (m_currentWindowState == WindowStates::MenuClosed) 
      {
        m_x = 350.f;
        m_y = 75.f;
        m_w = 1200.f;
        m_h = 545.f;

        labelYOffset = 500.0f;
        cardYOffset = 200.0f;
        cardScale = 1.0f;
        labelScale = 7.0f;
        charSize = 50;
        cardOffset = 160.f;
      }
      else if (m_currentWindowState == WindowStates::MenuOpen) 
      {
        m_x = 790.f;
        m_y = 75.f;
        m_w = 1000.f;
        m_h = 545.f;

        labelYOffset = 500.0f;
        cardYOffset = 200.0f;
        cardScale = 0.75f;
        labelScale = 6.f;
        charSize = 40;
        cardOffset = 130.f;
      }
      else
      {
        return; // Optional: Bail out if in an unhandled state
      }

      // 3. Calculate the shared positions
      float centerX = m_x + (m_w / 2.0f);
      float bankerX = m_x + (m_w / 4.0f);
      float playerX = m_x + (m_w * 0.75f);
      float labelY = (m_h / 5.f) * 5; // the cards will print 4/5th the current heigh

      // banker player zone LABELS
      float zoneLabelY = m_h / 4.f;
      float zoneLabelBankerX = m_w * 0.25f;
      float zoneLabelPlayerX = m_w * 0.75f;


      m_bankerZoneLabel.setPosition({zoneLabelBankerX + m_x, zoneLabelY});
      m_playerZoneLabel.setPosition({zoneLabelPlayerX + m_x, zoneLabelY});

      // 4. Apply positions to labels
      m_tieLabel.setPosition({centerX, labelY});
      m_bankerLabel.setPosition({bankerX, labelY});
      m_playerLabel.setPosition({playerX, labelY});

      m_tieLabel.setScale(labelScale, charSize);
      m_bankerLabel.setScale(labelScale, charSize);
      m_playerLabel.setScale(labelScale, charSize);

      m_area.setPosition({m_x, m_y});
      m_area.setSize({m_w, m_h});

      float cardY  = m_y + cardYOffset; 

      if (!m_bankerCards.empty())
      {
        float totalWidth = (m_bankerCards.size() - 1) * cardOffset;
        float startX = bankerX - (totalWidth / 2.0f); // Center on the banker zone

        for (size_t i = 0; i < m_bankerCards.size(); i++)
        {
          m_bankerCards[i].setPosition({startX + (i * cardOffset), cardY});
          m_bankerCards[i].setScale({cardScale, cardScale});
        }
      }

      if (!m_playerCards.empty())
      {
        float totalWidth = (m_playerCards.size() - 1) * cardOffset;
        float startX = playerX - (totalWidth / 2.0f); // Center on the player zone

        for (size_t i = 0; i < m_playerCards.size(); i++)
        {
          m_playerCards[i].setPosition({startX + (i * cardOffset), cardY});
          m_playerCards[i].setScale({cardScale, cardScale});
        }
      }
    }

    void drawGameElements(sf::RenderWindow &window) override
    {
      // betting zones these should be buttons
      m_bankerLabel.draw(window);
      m_playerLabel.draw(window);
      m_tieLabel.draw(window);

      //zone labels
      m_bankerZoneLabel.draw(window);
      m_playerZoneLabel.draw(window);

      for (auto& card : m_playerCards) card.draw(window);
      for (auto& card : m_bankerCards) card.draw(window);
    }

    void flipAllCards()
    {
      for (auto& card : m_playerCards) card.flip();
      for (auto& card : m_bankerCards) card.flip();
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

    void addBankerCard(Card card)
    {
      m_bankerCards.emplace_back(card, *m_assetMan);
      m_bankerCards.back().setOrigin(m_bankerCards.back().getLocalSize() / 2.f);

      updateInternalLayout();
    }

    void clearHands()
    {
      m_playerCards.clear();
      m_bankerCards.clear();
    }
};
