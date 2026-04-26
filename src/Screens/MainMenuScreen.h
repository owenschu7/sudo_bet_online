#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "../core/Debug.h"

#include "../core/GameEvents.h"

class MainMenuScreen : public Screen
{
private:

  sf::RectangleShape menuBackground;

  // --- UI HELPER METHODS ---

  void drawServerStatus(ImVec2 screenSize)
  {
    ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0];
    ImGui::PushFont(defaultFont);

    // 1. Check the flag to determine the text and the color
    const char* statusString = m_shared.s_isOnline ? "Status: Online" : "Status: Offline";

    // Green (RGBA) for Online, Red (RGBA) for Offline
    ImVec4 statusColor = m_shared.s_isOnline ? ImVec4(0.2f, 1.0f, 0.2f, 1.0f) 
                                             : ImVec4(1.0f, 0.2f, 0.2f, 1.0f); 

    // 2. Calculate size based on the specific word so right-alignment is flawless
    ImVec2 statusSize = ImGui::CalcTextSize(statusString);

    float topRightPadding = 20.0f;
    float statusX = screenSize.x - statusSize.x - topRightPadding;
    float statusY = topRightPadding; 

    ImGui::SetCursorPos(ImVec2(statusX, statusY));

    // 3. Apply the dynamic color and print
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor); 
    ImGui::Text("%s", statusString);

    ImGui::PopStyleColor();
    ImGui::PopFont();
  }

  
  void drawTopRightUI(ImVec2 screenSize)
  {
    // Define the strings to display
    // Using m_shared.s_isOnline which you update in your network loop
    const char* statusText = m_shared.s_isOnline ? "Status: Online" : "Status: Offline"; 

    std::string currentUser;
    std::string usernameText = "Current User: " + m_shared.s_currentUsername; // Change to m_shared.s_username.c_str() if you have one!
    
    ImVec4 statusColor = m_shared.s_isOnline ? ImVec4(0.2f, 1.0f, 0.2f, 1.0f) 
                                             : ImVec4(1.0f, 0.2f, 0.2f, 1.0f); 


    // Margin from the top and right edges of the screen
    float margin = 20.0f;
    float currentY = 20.0f;

    // 1. Draw Status
    ImVec2 statusSize = ImGui::CalcTextSize(statusText);
    ImGui::SetCursorPos(ImVec2(screenSize.x - statusSize.x - margin, currentY));
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor); 
    ImGui::Text("%s", statusText);
    currentY += statusSize.y + 5.0f; // Move down for the next element

    // 2. Draw Username (Below Status)
    ImVec2 userSize = ImGui::CalcTextSize(usernameText.c_str());
    ImGui::SetCursorPos(ImVec2(screenSize.x - userSize.x - margin, currentY));
    ImGui::Text("%s", usernameText.c_str());
    currentY += userSize.y + 10.0f; // Move down and add a little extra padding for the button

    ImGui::PopStyleColor();
  }


  void drawTitle(ImVec2 screenSize)
  {
    ImFont* titleFont = ImGui::GetIO().Fonts->Fonts[1];
    ImGui::PushFont(titleFont);
    const char* titleString = "Sudo Bet Online";
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImVec2 textSize = ImGui::CalcTextSize(titleString);

    float textX = (screenSize.x - textSize.x) * 0.5f;
    float textY = (screenSize.y - textSize.y) * 0.4f;

    ImGui::SetCursorPos(ImVec2(textX, textY));
    ImGui::Text("%s", titleString);
    ImGui::PopFont();
    ImGui::PopStyleColor();
  }


  void drawButtons(ImVec2 screenSize)
  {
    float buttonWidth = 600.0f;
    float buttonHeight = 80.0f;
    float leftPadding = 40.0f;
    float centerY = screenSize.y * 0.50f; 

    ImFont* titleButtonFont = ImGui::GetIO().Fonts->Fonts[2];
    ImGui::PushFont(titleButtonFont);

    // Move cursor for the first button
    ImGui::SetCursorPosX(leftPadding);
    ImGui::SetCursorPosY(centerY);

    // Push Styles
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(40.0f/255.0f, 150.0f/255.0f, 60.0f/255.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(60.0f/255.0f, 170.0f/255.0f, 80.0f/255.0f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(20.0f/255.0f, 130.0f/255.0f, 40.0f/255.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 30.0f)); // adds a space vertically
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    // 2. Pass ImVec2(0.0f, 0.0f) to make the button auto-size to the text width and height
    // (If you still want them to be chunky 100px tall buttons, use ImVec2(0.0f, 100.0f) instead!)
    if (ImGui::Button("Join A Table", ImVec2(0.0f, buttonHeight))) 
    {
      //create event
      GameEvent event;
      event.action = Action::GET_AvailableTables;
      event.game = Game::NONE;
      event.senderUUID = m_shared.s_currentUUID;
      event.senderUsername = m_shared.s_currentUsername;
      //send event
      m_shared.s_outboundEvents.push(event);
      DEBUG_PRINT << "SENDING EVENT: " << event << "\n";
      m_nextState = ScreenState::Tables;
    }

    ImGui::SetCursorPosX(leftPadding);
    if (ImGui::Button("Settings", ImVec2(0.0f, buttonHeight))) 
    {
      m_nextState = ScreenState::Settings;
    }

    ImGui::SetCursorPosX(leftPadding);
    if (ImGui::Button("Quit", ImVec2(0.0f, buttonHeight))) 
    {
      m_nextState = ScreenState::Quit;
    }

    // Pop Styles
    ImGui::PopStyleColor(4); 
    ImGui::PopStyleVar(2);  
    ImGui::PopFont();
  }

  void DrawUsername(ImVec2 screenSize)
  {
    static char usernameBuf[32] = "";

    if(m_shared.s_needUsername)
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
    } else { //user already has username
      ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0];
      ImGui::PushFont(defaultFont);

      // Combine the label with the actual username 
      // (Assumes m_shared.s_currentUsername is a std::string or const char*)
      std::string displayText = "Username: " + m_shared.s_currentUsername;

      // Calculate size based on the complete text
      ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());

      float bottomLeftPadding = 20.0f; 

      // Calculate X and Y positions
      float textX = bottomLeftPadding; 
      float textY = screenSize.y - textSize.y - bottomLeftPadding; 

      ImGui::SetCursorPos(ImVec2(textX, textY));

      // Push White text color (R: 1.0, G: 1.0, B: 1.0, Alpha: 1.0)
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); 

      // Print the final string
      ImGui::Text("%s", displayText.c_str());

      ImGui::PopStyleColor();
      ImGui::PopFont();
    }
  }


public:
  MainMenuScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a dark blue background for the menu
    menuBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    menuBackground.setFillColor(sf::Color(20, 20, 50));
  }

  //handle events that came in from server to the client
  void onNetworkEvent(const GameEvent& event) override
  {
    TRACE_FUNCTION();
    // Handle events specific only related to the screen
    switch (event.action)
    {
      case Action::SYS_Connect_Success:
        DEBUG_PRINT << "SYS_Connect_Success\n";
        DEBUG_PRINT << event.senderUsername << " joined the server.\n";
        m_shared.s_isOnline = true;
        // Add them to your local ImGui player list
        break;
      default:
        break;
    }
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "Enter" signals we want to switch to the game state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        m_nextState = ScreenState::Baccarat;
      }
    }
  }


  void update(sf::RenderWindow& window) override
  {
    processEventsFromServer();

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    ImVec2 rawMouse = ImGui::GetIO().MousePos;
    sf::Vector2i pixelPos(static_cast<int>(rawMouse.x), static_cast<int>(rawMouse.y));
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(screenSize);
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Menu Layer", nullptr, windowFlags);

    drawServerStatus(screenSize);
    drawTitle(screenSize);
    drawButtons(screenSize);
    drawTopRightUI(screenSize);
    DrawUsername(screenSize);

    ImGui::End();
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(menuBackground);
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
