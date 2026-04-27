#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "../core/Debug.h"
#include "../UI/menuButton.h"
#include "../UI/textLabel.h"
#include "../UI/UIHelper.h"

class StartScreen : public Screen
{
private:
  sf::RectangleShape m_menuBackground;

  Button m_startButton;
  Button m_settingsButton;
  Button m_exitButton;

  Label m_gameTitle;
  Label m_usernameDisplay;
  sf::Texture m_buttonSheet;
  sf::Texture m_gameSheetUI;

  void DrawUsernamePopup(ImVec2 screenSize)
  {
    static char usernameBuf[32] = "";
    if (m_shared.s_needUsername)
    {
      float centerX = (screenSize.x / 2.0f);
      float centerY = (screenSize.y / 2.0f);

      ImGui::SetNextWindowPos(ImVec2(centerX, centerY), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::OpenPopup("WELCOME...");

      ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoCollapse;

      if (ImGui::BeginPopupModal("WELCOME...", NULL, flags))
      {
        ImGui::Text("Please Enter a Username:");
        ImGui::Separator();
        ImGui::InputText("##username", usernameBuf, IM_ARRAYSIZE(usernameBuf));
        if (ImGui::Button("Enter"))
        {
          m_shared.s_currentUsername = usernameBuf;
          m_shared.s_settingsChanged = true;
          m_shared.s_needUsername = false;
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    }
  }

public:
  StartScreen(SharedData& sharedData)
    : Screen(sharedData),
    m_gameTitle(sharedData.s_assets.getFont("gameFontTitle")),
    m_usernameDisplay(sharedData.s_assets.getFont("gameFont"))
  {
    // 1. Background
    m_menuBackground.setSize({UI::SCREEN_W, UI::SCREEN_H});
    m_menuBackground.setFillColor(sf::Color(20, 20, 50));

    // 3. Setup buttons
    float btnScale = 8.0f;
    sf::IntRect startButtonCrop({126, 195}, {54, 17});
    sf::IntRect settingsButtonCrop({126, 215}, {54, 17});
    sf::IntRect exitButtonCrop({126, 235}, {54, 17});

    // scaledW = how wide the button will actually appear on screen
    float scaledW = startButtonCrop.size.x * btnScale;
    float centerX = (UI::SCREEN_W - scaledW) / 2.0f;
    float startY  = 580.0f;
    float spacing = 155.0f;

    const sf::Texture& uiTexture = sharedData.s_assets.getTexture("BlackAndWhiteUI");

    m_startButton.setupNoText(uiTexture, startButtonCrop, btnScale, {centerX, startY});
    m_settingsButton.setupNoText(uiTexture, settingsButtonCrop, btnScale, {centerX, startY + spacing});
    m_exitButton.setupNoText(uiTexture, exitButtonCrop, btnScale, {centerX, startY + (spacing * 2)});

    // 4. Setup labels
    m_gameTitle.setupWithBanner(
      "Sudo Bet \nOnline", 
      220, sf::Color::White,
      {UI::SCREEN_W / 2.0f, 320.0f},
      true,
      uiTexture,
      BannerType::Banner3,
      11.0f,
      {0.0f, 0.0f}
    );
    m_usernameDisplay.setup("Username: ", 30, sf::Color::White, {30.0f, UI::SCREEN_H - 60.0f}, false);
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    if (m_startButton.isClicked(mousePos, event))
    {
      GameEvent connectEvent;
      connectEvent.action         = Action::SYS_Connect;
      connectEvent.game           = Game::NONE;
      connectEvent.senderUsername = m_shared.s_currentUsername;
      connectEvent.senderUUID     = m_shared.s_currentUUID;
      connectEvent.stringPayload  = "";
      m_shared.s_outboundEvents.push(connectEvent);

      m_nextState = ScreenState::MainMenu;
    }

    if (m_exitButton.isClicked(mousePos, event))
    {
      m_nextState = ScreenState::Quit;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        m_nextState = ScreenState::MainMenu;
      }
    }
  }

  void update(sf::RenderWindow& window) override
  {
    processEventsFromServer();

    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    m_startButton.update(mousePos);
    m_settingsButton.update(mousePos);
    m_exitButton.update(mousePos);

    m_usernameDisplay.setString("Username: " + m_shared.s_currentUsername);

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(screenSize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize   |
      ImGuiWindowFlags_NoMove     |
      ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Menu Layer", nullptr, windowFlags);
    DrawUsernamePopup(screenSize);
    ImGui::End();
  }

  void draw(sf::RenderWindow& window) override
  {
    window.draw(m_menuBackground);
    m_gameTitle.draw(window);
    m_startButton.draw(window);
    m_settingsButton.draw(window);
    m_exitButton.draw(window);

    if (!m_shared.s_needUsername)
    {
      m_usernameDisplay.draw(window);
    }
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
