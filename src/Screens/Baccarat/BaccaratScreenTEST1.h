//(when a real player joins at a seat you do this)
#pragma once
#include "../Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include "../../core/player.h"
#include "../../UI/UIHelper.h"
#include "../../UI/PlayerHUD.h"
#include "../../UI/PlayerTab.h"
#include "../../UI/TableChipRack.h"
#include "../../UI/LeftMenu.h"
#include "../../UI/BaccaratArea.h"
#include "../../UI/CursorUI.h"

//for local testing
//this will be removed when server is done (server should handle baccarat logic not client)
#include "../../logic/baccarat/baccarat_table.h"



class BaccaratScreenTEST1 : public Screen
{
private:
  // Background
  sf::RectangleShape tableBackground;

  // Baccarat Area and Chip Rack
  BaccaratArea m_baccaratArea;
  TableChipRack m_tableChipRack;
  CursorUI m_cursorText;

  // PLAYERS
  Player m_emptyPlayer; // empty player
  PlayerTab m_playerTab;
  std::vector<Player> activePlayers;

  // MENU
  LeftMenu m_leftMenu; // displays the button and the window if button is clicked
  WindowStates m_currentWindowState; // holds the state of the screen (if menu is open or not)
  
  //baccarat logic
  Baccarat_table m_bacc;
  std::vector<Round_Record> m_rounds; // a list of all the rounds played

public:
  //PlayerHUD has no default constructor because sf::Text and sf::Sprite both require arguments
  //so we initialize m_playerTab in the initializer list
  BaccaratScreenTEST1(SharedData &sharedData) :
    Screen(sharedData),
    m_playerTab(
      sharedData.s_assets.getFont("gameFont"),
      sharedData.s_assets.getFont("gameNumbersFont"),
      sharedData.s_assets.getTexture("HUD_Coin"),
      m_emptyPlayer,
      sf::Vector2f{10.f, 10.f}
    ),
    m_tableChipRack(sharedData.s_testPlayer, sharedData.s_assets),
    m_leftMenu(sharedData.s_testPlayer, sharedData.s_assets),
    m_baccaratArea(sharedData.s_assets, 350.f, 75.0f, 1200.0f, 545.0f),
    m_bacc(1, 6),
    m_cursorText(sharedData.s_assets)
  {
    // Set up a 1080p green background
    tableBackground.setSize(sf::Vector2f({UI::SCREEN_W, UI::SCREEN_H}));
    tableBackground.setFillColor(sf::Color(35, 107, 0)); // Casino green

    // init the playerHUD
    m_playerTab.hud.setPlayer(&sharedData.s_testPlayer);

  }

  std::string getHoveredElement(sf::Vector2f mousePos) override
  {
    if (m_baccaratArea.getFeltBounds().contains(mousePos)) 
    {
      // If they are on the felt, you can get even more specific!
      // You could add similar "getBounds" for your labels if you want:
      // if (m_baccaratArea->isOverBanker(mousePos)) return "Banker Zone";

      return "Baccarat Table";
    }

    // 2. Check other Screen-level UI
    if (m_playerTab.bounds.contains(mousePos)) 
      return "Player HUD";

    if (tableBackground.getGlobalBounds().contains(mousePos)) 
      return "Floor";

    return "None";
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    m_tableChipRack.handleEvent(event, window);
    m_leftMenu.handleEvent(event, window);

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
      }
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        m_baccaratArea.flipAllCards();
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space))
      {
        m_baccaratArea.clearHands();
      }
    }
  }

  void update(sf::RenderWindow& window, sf::Time dt) override
  {
    //calu mouse cords
    sf::Vector2i mousePosWindow = sf::Mouse::getPosition(window);
    sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePosWindow);

    m_playerTab.hud.update();
    m_tableChipRack.update(mouseCoords);
    m_leftMenu.update(mouseCoords);

    int currentSelection = m_tableChipRack.getSelectedChipValue();
    m_cursorText.updateMultiplier(currentSelection);

    m_cursorText.updatePosition(window);

    WindowStates newState = m_leftMenu.isOpen() ? WindowStates::MenuOpen : WindowStates::MenuClosed;
    if (newState != m_currentWindowState)
    {
      m_currentWindowState = newState;
      m_baccaratArea.setWindowState(m_currentWindowState);
      m_tableChipRack.setWindowState(m_currentWindowState);
    }

    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    //window to control the amount of chips shown
    chipControlWindow();
    RenderBaccaratTesterUI();

    // Baccarat game logic (dealing cards, checking win conditions) goes here
  }

  void draw(sf::RenderWindow& window) override
  {
    window.draw(tableBackground);

    m_baccaratArea.draw(window);
    m_tableChipRack.draw(window);

    m_playerTab.hud.draw(window);
    m_leftMenu.draw(window);

    m_cursorText.draw(window);
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }


  //imgui functions
  void RenderBaccaratTesterUI() {
    ImGui::Begin("Baccarat Hand Tester", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Create a 2-column layout for Player and Banker areas
    ImGui::Columns(2, "BaccaratTable", false);

    ImGui::NextColumn();

    ImGui::Columns(1); // Reset back to single column layout

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- ACTION BUTTONS ---
    // Centered/grouped action buttons at the bottom
    if (ImGui::Button("Play Hand", ImVec2(200, 60)))
    {
      m_baccaratArea.clearHands(); // remove last hands cards

      Round_Record curr_round = m_bacc.play_round();
      m_rounds.push_back(curr_round);

      m_baccaratArea.addPlayerCard(curr_round.player_cards.get_first_card());
      m_baccaratArea.addPlayerCard(curr_round.player_cards.get_second_card());
      m_baccaratArea.addPlayerCard(curr_round.player_cards.get_third_card());

      m_baccaratArea.addBankerCard(curr_round.banker_cards.get_first_card());
      m_baccaratArea.addBankerCard(curr_round.banker_cards.get_second_card());
      m_baccaratArea.addBankerCard(curr_round.banker_cards.get_third_card());
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Cards", ImVec2(200, 60)))
    {
      m_baccaratArea.clearHands();
    }

    ImGui::SameLine();

    if (ImGui::Button("Flip All", ImVec2(200, 60))) {
      m_baccaratArea.flipAllCards();
    }

    ImGui::End();
  }

  void chipControlWindow()
  {
    // -------------------------------------------------------
    // ImGui Chip Control Window
    // -------------------------------------------------------
    ImGui::Begin("Chip Controls"); 

    ImGui::Text("Enter desired chip amount:");
    
    static int inputAmount = 0; 

    // 1. Removed the crash-causing flag!
    ImGui::InputInt("##ChipAmount", &inputAmount, 1, 100);

    // 2. This is the correct ImGui way to check if the user pressed Enter
    // or clicked away after changing the number.
    bool userFinishedTyping = ImGui::IsItemDeactivatedAfterEdit();

    // Prevent negative chip values
    if (inputAmount < 0) {
        inputAmount = 0;
    }

    // 3. Update if the button is clicked OR they finished typing
    if (ImGui::Button("Update Rack") || userFinishedTyping)
    {
        m_tableChipRack.setValue(inputAmount);
        //std::cout << "Rack updated to: " << inputAmount << std::endl;
    }

    ImGui::End();
    // -------------------------------------------------------
  }
};
