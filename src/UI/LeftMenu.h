// background menu sprite is at (6,80) to (92,192)
// right arrow sprite is at (70,226) to (78,238)
// left arrow sprite is at (52,226) to (60,238)

#pragma once

#include "../core/player.h"
#include "../core/AssetManager.h"
#include <SFML/Graphics.hpp>
#include "textLabel.h"
#include "UIHelper.h"
#include "menuButton.h"


class LeftMenu
{
  private:
    AssetManager &m_assetMan;
    const Player *m_player;

    bool m_isOpen = false;
    Button m_rightArrowButton; // right arrow will open menu when clicked
    Button m_leftArrowButton; // left arrow will close menu when clicked

    sf::Sprite m_menuWindow;
    Label m_leftMenuLabel;
    // other items will eventually go here

  public:
    // -------------------------------------------------------
    // Layout constants — if we need them (prob not)
    // -------------------------------------------------------
    static constexpr float MENU_START_X = 65.f;
    static constexpr float MENU_START_Y = 225.f;
    // -------------------------------------------------------
    LeftMenu(const Player& player, AssetManager& assetMan)
      : m_player(&player),
      m_assetMan(assetMan),
      m_leftMenuLabel(assetMan.getFont("gameFont")),
      m_menuWindow(assetMan.getTexture("BlackAndWhiteUI"))
  {
    m_menuWindow.setTextureRect(sf::IntRect({6, 80}, {86, 112}));
    m_menuWindow.setPosition({200.f, 300.f});
    m_menuWindow.setScale({8.f, 8.f});

    const sf::Texture& uiTexture = m_assetMan.getTexture("BlackAndWhiteUI");

    m_leftMenuLabel.setupWithBanner(
        "Menu",
        49, sf::Color::Black,
        {275.f, 200.0f},
        true,
        uiTexture,
        BannerType::WhiteBanner1,
        4.0f,
        {0.0f, 0.0f}
      );

    m_menuWindow.setPosition({15.f, 167.f});

    m_rightArrowButton.setupNoText(uiTexture, sf::IntRect({70, 226}, {8, 12}), 9.f, {30,525});
    m_leftArrowButton.setupNoText(uiTexture, sf::IntRect({52, 226}, {8, 12}), 9.f, {30,525});
  }

    //handle mouse clicks
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window)
    {
      // Check if the event is specifically a mouse button RELEASE
      if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) 
      {
        // Check if it was the Left mouse button
        if (mouseReleased->button == sf::Mouse::Button::Left)
        {
          // Map the exact coordinates of the event
          sf::Vector2f mousePos = window.mapPixelToCoords(mouseReleased->position);

          if (!m_isOpen)
          {
            // If closed, check if the right arrow is clicked to open it
            if (m_rightArrowButton.isClicked(mousePos, event))
            {
              std::cout << "Right arrow clicked! Opening menu." << std::endl;
              m_isOpen = true;
            }
          }
          else
          {
            // If open, check if the left arrow is clicked to close it
            if (m_leftArrowButton.isClicked(mousePos, event))
            {
              std::cout << "Left arrow clicked! Closing menu." << std::endl;
              m_isOpen = false;
            }

            // Handle clicks for your actual menu items here
          }
        }
      }
    }

    //handle hovering
    void update(sf::Vector2f mousePos)
    {
      m_leftArrowButton.update(mousePos);
      m_rightArrowButton.update(mousePos);
    }

    //draw
    void draw(sf::RenderWindow& window)
    {
      //check if the menu is open
      if (m_isOpen)
      {
        window.draw(m_menuWindow);
        m_leftArrowButton.draw(window);
        m_leftMenuLabel.draw(window);
      }
      else
      {
        m_rightArrowButton.draw(window);
      }
    }

    bool isOpen()
    {
      return m_isOpen;
    }

};


