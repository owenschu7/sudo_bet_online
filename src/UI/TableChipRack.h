#pragma once

#include "ChipSprite.h"
#include "../core/player.h"
#include "../core/AssetManager.h"
#include "../core/chips/ChipStack.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "textLabel.h"
#include "Area.h"


class TableChipRack : public Area
{
  private:
    AssetManager &m_assetMan;
    const Player *m_player;
    int m_value;
    sf::Sprite m_tableBoard;

    ChipSprite m_lightBlueChip;
    ChipSprite m_redChip;
    ChipSprite m_yellowChip;
    ChipSprite m_blueChip;

    ChipStack m_chipStack;

    Label m_totalLabel;
    bool m_showLabel;

    //selection members
    int m_selectedChipValue = 0;
    sf::Sprite m_selectionSprite;

  public:
    // -------------------------------------------------------
    // Layout constants — if we need them (prob not)
    // -------------------------------------------------------
    static constexpr float SPRITE_W    = 77.f;
    static constexpr float SPRITE_H    = 27.f;
    static constexpr float SCALE       = 1.4f;
    static constexpr float WIDTH       = SPRITE_W * SCALE;  // rendered width
    static constexpr float HEIGHT      = SPRITE_H * SCALE;  // rendered height
    static constexpr float TEXT_SIZE   = 48u;
    // text offsets relative to top-left of sprite
    static constexpr float USERNAME_OFFSET_X      = 10.f;  // pushes text past the avatar
    static constexpr float USERNAME_OFFSET_Y  = -50.f;
    // -------------------------------------------------------
    TableChipRack(const Player& player, AssetManager& assetMan)
      : m_player(&player),
      Area(0.f, 0.f, 0.f, 0.f),
      m_assetMan(assetMan),
      m_tableBoard(assetMan.getTexture("BlackAndWhiteUI")),
      m_lightBlueChip(1, 0, m_assetMan),
      m_redChip(5, 0, m_assetMan),
      m_yellowChip(25, 0, m_assetMan),
      m_blueChip(100, 0, m_assetMan),
      m_totalLabel(assetMan.getFont("gameNumbersFont")),
      m_selectionSprite(assetMan.getTexture("SelectUI"))
  {
    m_value = player.getChips();
    m_chipStack.setTotalValue(834);

    std::map<ChipColor, int> chipCounts = m_chipStack.getChipCounts();

    std::cout << "Chip Counts:\n";
    for (const auto& [color, count] : chipCounts)
    {
      // We cast 'color' to an int just so the console prints a number 
      // instead of getting confused by the enum class.
      std::cout << "Color ID " << static_cast<int>(color) << ": " << count << " chips\n";
    }
    std::cout << std::endl;



    // If a color is missing from the map, this safely defaults to 0.
    m_lightBlueChip.setChipCount(chipCounts[ChipColor::LightBlue]);
    m_redChip.setChipCount(chipCounts[ChipColor::Red]);
    m_yellowChip.setChipCount(chipCounts[ChipColor::Yellow]);
    m_blueChip.setChipCount(chipCounts[ChipColor::Blue]);

    m_lightBlueChip.setPosition({650.f, 690.f});
    m_redChip.setPosition({820.f, 690.f});
    m_yellowChip.setPosition({1000.f, 690.f});
    m_blueChip.setPosition({1175.f, 690.f});

    m_tableBoard.setTextureRect(sf::IntRect({3, 274}, {170, 60}));
    m_tableBoard.setPosition({500, 700});
    m_tableBoard.setScale({5.5f, 5.5f});

    m_totalLabel.setup(std::to_string(10000), 40, sf::Color::White, {1000.f, 900.f}, true);

    //selection stuff
    m_selectionSprite.setTextureRect(sf::IntRect({0, 0}, {30, 30}));
    m_selectionSprite.setScale({4.5f, 4.5f});

    updateInternalLayout();
  }

    void updateInternalLayout() override
    {
      // 1. Set default variables that change based on state
      float labelYOffset = 0.0f;
      float boardScale = 5.5f;
      unsigned int charSize;

      // 2. Assign the specific layout numbers based on the menu state
      if (m_currentWindowState == WindowStates::MenuClosed) 
      {
        m_x = 500.f;
        m_y = 700.f;
        m_w = 1000.f;
        m_h = 300.f;

        labelYOffset = 500.0f;
        boardScale = 5.5f;
        charSize = 50;
      }
      else if (m_currentWindowState == WindowStates::MenuOpen) 
      {
        m_x = 800.f;
        m_y = 700.f;
        m_w = 1000.f;
        m_h = 300.f;

        labelYOffset = 500.0f;
        boardScale = 5.5f;
        charSize = 40;
      }
      else
      {
        return; // if in an unhandled state
      }

      
      m_tableBoard.setPosition({m_x, m_y});
      m_tableBoard.setScale({boardScale, boardScale});

      float center_y = m_y + (m_h / 2.f);

      // chips
      m_lightBlueChip.setPosition({m_x + (m_w * 0.20f), center_y});
      m_redChip.setPosition({m_x + (m_w * 0.40f), center_y});
      m_yellowChip.setPosition({m_x + (m_w * 0.60f), center_y});
      m_blueChip.setPosition({m_x + (m_w * 0.80f), center_y});


      // 3. Calculate the shared positions
      float centerX = m_x + (m_w / 2.0f);
      float bankerX = m_x + (m_w / 4.0f);
      float playerX = m_x + (m_w * 0.75f);
      float labelY = (m_h / 5.f) * 5; // the cards will print 4/5th the current heigh


      m_area.setPosition({m_x, m_y});
      m_area.setSize({m_w, m_h});

    }

    int getSelectedChipValue() const
    {
      return m_selectedChipValue;
    }

    void drawGameElements(sf::RenderWindow &window) override
    {
      window.draw(m_tableBoard);
      m_lightBlueChip.draw(window);
      m_redChip.draw(window);
      m_yellowChip.draw(window);
      m_blueChip.draw(window);

      // If a chip is currently selected, draw the highlight over it
      if (m_selectedChipValue != 0)
      {
        window.draw(m_selectionSprite);
      }

      if (m_showLabel)
      {
        m_totalLabel.draw(window);
      }
    }

    void setValue(int newValue)
    {
      m_chipStack.setTotalValue(newValue);
      m_chipStack.distributeChipsEvenly();

      // 2. Get the new map of chip counts
      std::map<ChipColor, int> chipCounts = m_chipStack.getChipCounts();

      // 3. Update the visual sprites with the new counts!
      // (If a color isn't in the map, it safely returns 0 and hides the chip)
      m_lightBlueChip.setChipCount(chipCounts[ChipColor::LightBlue]);
      m_redChip.setChipCount(chipCounts[ChipColor::Red]);
      m_yellowChip.setChipCount(chipCounts[ChipColor::Yellow]);
      m_blueChip.setChipCount(chipCounts[ChipColor::Blue]);

      m_totalLabel.setString(std::to_string(newValue));
    }

    //handle mouse click selection over chips
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window)
    {
      if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
      {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
          sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

          // 1. Create a helper function for the selection math
          auto selectChip = [&](int value, const ChipSprite& targetChip) {
            m_selectedChipValue = value;
            float padding = 20.0f; 

            sf::FloatRect targetBounds = targetChip.getGlobalBounds();
            sf::FloatRect selectBounds = m_selectionSprite.getLocalBounds();

            float scaleX = (targetBounds.size.x + (padding * 2.0f)) / selectBounds.size.x;
            float scaleY = (targetBounds.size.y + (padding * 2.0f)) / selectBounds.size.y;

            m_selectionSprite.setScale({scaleX, scaleY});
            m_selectionSprite.setPosition({
                targetBounds.position.x - padding,
                targetBounds.position.y - padding
                });
          };

          // 2. Use the helper function!
          if (m_lightBlueChip.getGlobalBounds().contains(mousePos)) {
            selectChip(1, m_lightBlueChip);
          } 
          else if (m_redChip.getGlobalBounds().contains(mousePos)) {
            selectChip(5, m_redChip);
          }
          else if (m_yellowChip.getGlobalBounds().contains(mousePos)) {
            selectChip(25, m_yellowChip);
          }
          else if (m_blueChip.getGlobalBounds().contains(mousePos)) {
            selectChip(100, m_blueChip);
          }
          else {
            m_selectedChipValue = 0; 
          }
        }
      }
    }

    //handle hovering
    void update(sf::Vector2f mousePos)
    {
      // Check if the mouse is inside the black tray sprite
      if (m_tableBoard.getGlobalBounds().contains(mousePos))
      {
        m_showLabel = true;
      }
      else
      {
        m_showLabel = false;
      }
    }

};


