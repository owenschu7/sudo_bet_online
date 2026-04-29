#pragma once

#include "ChipSprite.h"
#include "../core/player.h"
#include "../core/AssetManager.h"
#include "../core/chips/ChipStack.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "textLabel.h"


class TableChipRack
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
    m_assetMan(assetMan),
    m_tableBoard(assetMan.getTexture("BlackAndWhiteUI")),
    m_lightBlueChip(1, 0, m_assetMan),
    m_redChip(5, 0, m_assetMan),
    m_yellowChip(25, 0, m_assetMan),
    m_blueChip(100, 0, m_assetMan),
    m_totalLabel(assetMan.getFont("gameNumbersFont"))

  {
    m_value = player.getChips();
    m_chipStack.setTotalValue(10000);

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

    m_lightBlueChip.setPosition({650.f, 740.f});
    m_redChip.setPosition({820.f, 740.f});
    m_yellowChip.setPosition({1000.f, 740.f});
    m_blueChip.setPosition({1175.f, 740.f});

    m_tableBoard.setTextureRect(sf::IntRect({3, 274}, {170, 60}));
    m_tableBoard.setPosition({550, 670});
    m_tableBoard.setScale({5.f, 5.f});

    m_totalLabel.setup(std::to_string(10000), 40, sf::Color::White, {1000.f, 900.f}, true);


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

  //handle mouse click/hovering
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

  //draw
  void draw(sf::RenderTarget& window)
  {
    window.draw(m_tableBoard);
    m_lightBlueChip.draw(window);
    m_redChip.draw(window);
    m_yellowChip.draw(window);
    m_blueChip.draw(window);

    if (m_showLabel)
    {
      m_totalLabel.draw(window);
    }
  }

};


