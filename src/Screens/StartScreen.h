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
  Button m_quitButton;
  Label m_gameTitle;
  Label m_usernameDisplay;
  sf::Texture m_buttonSheet;

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
    m_gameTitle(sharedData.s_gameFont),
    m_usernameDisplay(sharedData.s_gameFont)
  {
    // 1. Background
    m_menuBackground.setSize({UI::SCREEN_W, UI::SCREEN_H});
    m_menuBackground.setFillColor(sf::Color(20, 20, 50));

    // 2. Load texture sheet
    if (!m_buttonSheet.loadFromFile("assets/images/UIPack/UI assets Demo (2x).png"))
    {
      DEBUG_PRINT << "ERROR: Could not load assets/ui/buttons.png\n";
    }

    // 3. Setup buttons
    float btnScale = 5.0f;
    sf::IntRect buttonCrop({194, 290}, {92, 30});

    // scaledW = how wide the button will actually appear on screen
    float scaledW = buttonCrop.size.x * btnScale;
    float centerX = (UI::SCREEN_W - scaledW) / 2.0f;
    float startY  = 450.0f;
    float spacing = 140.0f;

    m_startButton.setup(m_buttonSheet, buttonCrop, m_shared.s_gameFont, "START", btnScale, {centerX, startY});
    m_quitButton.setup (m_buttonSheet, buttonCrop, m_shared.s_gameFont, "QUIT",  btnScale, {centerX, startY + spacing});

    // 4. Setup labels
    m_gameTitle.setupWithBanner(
      "Sudo Bet Online", 
      120, sf::Color(61,41,54),
      {UI::SCREEN_W / 2.0f, 250.0f},
      true,
      m_buttonSheet,
      BannerType::Banner2,
      15.0f,
      {0, -28.0f}
    );
    m_usernameDisplay.setup("Username: ", 30, sf::Color::White, {30.0f, UI::SCREEN_H - 60.0f}, false);
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    if (m_startButton.isClicked(mousePos, event))   // FIX: m_ prefix
    {
      GameEvent connectEvent;
      connectEvent.action         = Action::SYS_Connect;
      connectEvent.game           = Game::NONE;   // FIX: plain member access
      connectEvent.senderUsername = m_shared.s_currentUsername;
      connectEvent.senderUUID     = m_shared.s_currentUUID;
      connectEvent.stringPayload  = "";
      m_shared.s_outboundEvents.push(connectEvent);

      m_nextState = ScreenState::MainMenu;
    }

    if (m_quitButton.isClicked(mousePos, event))
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

  void update() override
  {
    processEventsFromServer();

    ImVec2 rawMouse   = ImGui::GetIO().MousePos;
    sf::Vector2f mousePos(rawMouse.x, rawMouse.y);

    m_startButton.update(mousePos);     // FIX: m_ prefix
    m_quitButton.update(mousePos);      // FIX: m_ prefix

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
    window.draw(m_menuBackground);      // FIX: m_ prefix
    m_gameTitle.draw(window);           // FIX: m_ prefix
    m_startButton.draw(window);         // FIX: m_ prefix
    m_quitButton.draw(window);          // FIX: m_ prefix

    if (!m_shared.s_needUsername)
    {
      m_usernameDisplay.draw(window); // FIX: m_ prefix
    }
  }

  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
