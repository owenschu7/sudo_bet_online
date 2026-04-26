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
  
  sf::RectangleShape menuBackground;

  Button startButton;
  Button quitButton;

  Label gameTitle;
  Label usernameDisplay;

  void DrawUsernamePopup(ImVec2 screenSize)
  {
    static char usernameBuf[32] = "";

    if (m_shared.s_needUsername)
    {
      //center the modal to the center of the screen

      float centerX = (screenSize.x / 2.0f);
      float centerY = (screenSize.y / 2.0f);

      //draw the popup to get username info
      // center the popup on the screen
      // to do this we use SetNextWindowPos(), it sets the next location to draw a window at 
      // (in our case its the popup and we want it in the center of the screen)
      ImGui::SetNextWindowPos(ImVec2(centerX, centerY), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

      // 1. Tell ImGui to actually open the modal!
      // The string here MUST match the string in BeginPopupModal exactly.
      ImGui::OpenPopup("WELCOME...");

      // 3. Define the flags to make it static (no moving, no resizing, no collapsing)
      ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_AlwaysAutoResize | 
        ImGuiWindowFlags_NoCollapse;

      // 4. Begin the modal
      if (ImGui::BeginPopupModal("WELCOME...", NULL, flags))
      {
        ImGui::Text("Please Enter a Username:");
        ImGui::Separator();

        ImGui::InputText("##username", usernameBuf, IM_ARRAYSIZE(usernameBuf));

        if (ImGui::Button("Enter"))
        {
          m_shared.s_currentUsername = usernameBuf;
          m_shared.s_settingsChanged = true;

          // Move to the next screen!
          m_shared.s_needUsername = false;

          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    }
  }


public:

  StartScreen(SharedData &sharedData) 
    : Screen(sharedData),
      startButton(sharedData.s_gameFont),
      quitButton(sharedData.s_gameFont),
      gameTitle(sharedData.s_gameFont),
      usernameDisplay(sharedData.s_gameFont)
  {
    menuBackground.setSize(sf::Vector2f({UI::SCREEN_W, UI::SCREEN_H}));
    menuBackground.setFillColor(sf::Color(20, 20, 50));

    // --- SETUP LABELS ---
    // setup(string, size, color, position, isCentered)
    
    // Title is Centered (true) at X: 960, Y: 250
    gameTitle.setup("Sudo Bet Online", 180, sf::Color::White, {UI::SCREEN_W / 2.0f, 250.0f}, true);

    // Username is Left-Aligned (false) in the bottom corner
    usernameDisplay.setup("Username: ", 30, sf::Color::White, {30.0f, UI::SCREEN_H - 60.0f}, false);

    // --- SETUP BUTTONS ---
    sf::Vector2f btnSize(500.0f, 100.0f);
    float centerX = (1920.0f - btnSize.x) / 2.0f; 
    float startY = 450.0f; 
    float spacing = 120.0f; 

    startButton.setup("Start", btnSize, {centerX, startY});
    quitButton.setup("Quit", btnSize, {centerX, startY + spacing});
  }


  //handle events that came in from server to the client
//void onNetworkEvent(const GameEvent& event) override
//{
//  // Handle events specific only related to the screen
//  switch (event.type)
//  {
//    case EventType::SYS_Connect_Success:
//      DEBUG_PRINT << "SYS_Connect_Success\n";
//      DEBUG_PRINT << event.senderUsername << " joined the server.\n";
//      m_shared.s_isOnline = true;
//      // Add them to your local ImGui player list
//      break;
//    default:
//      break;
//  }
//}


  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    // Get mouse position and map it to your 1920x1080 view
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    if (startButton.isClicked(mousePos, event))
    {
      GameEvent connectEvent;
      connectEvent.action = Action::SYS_Connect;
      connectEvent.game = Game::NONE;
      connectEvent.senderUsername = m_shared.s_currentUsername;
      connectEvent.senderUUID = m_shared.s_currentUUID;
      connectEvent.stringPayload = "";
      m_shared.s_outboundEvents.push(connectEvent);
      
      m_nextState = ScreenState::MainMenu;
    }


    if (quitButton.isClicked(mousePos, event))
    {
      m_nextState = ScreenState::Quit;
    }

    // Existing enter-key fallback
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

    // 1. Give the buttons the mouse position so they highlight when hovered!
    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2f mousePos(rawMouse.x, rawMouse.y); 
    startButton.update(mousePos);
    quitButton.update(mousePos);

    // 2. Update the SFML text label with the latest username
    usernameDisplay.setString("Username: " + m_shared.s_currentUsername);

    // 1. Setup the invisible full-screen window
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(screenSize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Menu Layer", nullptr, windowFlags);
    DrawUsernamePopup(screenSize); //should only contain the popup logic for getting a new username
    ImGui::End();
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw background
    window.draw(menuBackground);

    //sfml title
    gameTitle.draw(window);

    //draw the buttons
    startButton.draw(window);
    quitButton.draw(window);

    // Only draw the username in the bottom corner if they are done with the popup
    if (!m_shared.s_needUsername) 
    {
      usernameDisplay.draw(window);
    }
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
