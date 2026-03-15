#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

class MainMenuScreen : public Screen
{
private:
  ScreenState nextState = ScreenState::None;
  
  sf::RectangleShape menuBackground;

  // --- UI HELPER METHODS ---

  void drawServerStatus(ImVec2 screenSize)
  {
    ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0];
    ImGui::PushFont(defaultFont);

    const char* statusString = "Status: Servers Offline";

    ImVec2 statusSize = ImGui::CalcTextSize(statusString);
    
    float topRightPadding = 20.0f;
    float statusX = screenSize.x - statusSize.x - topRightPadding;
    float statusY = topRightPadding; 

    ImGui::SetCursorPos(ImVec2(statusX, statusY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // Red
    ImGui::Text("%s", statusString);
    ImGui::PopStyleColor();
    ImGui::PopFont();
  }

  void drawTitle(ImVec2 screenSize)
  {
    ImFont* titleFont = ImGui::GetIO().Fonts->Fonts[1];
    ImGui::PushFont(titleFont);
    const char* titleString = "Baccarat Online";
    ImVec2 textSize = ImGui::CalcTextSize(titleString);

    float textX = (screenSize.x - textSize.x) * 0.5f;
    float textY = (screenSize.y - textSize.y) * 0.4f;

    ImGui::SetCursorPos(ImVec2(textX, textY));
    ImGui::Text("%s", titleString);
    ImGui::PopFont();
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

    // 2. Pass ImVec2(0.0f, 0.0f) to make the button auto-size to the text width and height
    // (If you still want them to be chunky 100px tall buttons, use ImVec2(0.0f, 100.0f) instead!)
    if (ImGui::Button("Join A Table", ImVec2(0.0f, buttonHeight))) 
    {
        nextState = ScreenState::Tables;
    }

    ImGui::SetCursorPosX(leftPadding);
    if (ImGui::Button("Settings", ImVec2(0.0f, buttonHeight))) 
    {
        nextState = ScreenState::Settings;
    }

    ImGui::SetCursorPosX(leftPadding);
    if (ImGui::Button("Quit", ImVec2(0.0f, buttonHeight))) 
    {
        nextState = ScreenState::Quit;
    }

    // Pop Styles
    ImGui::PopStyleColor(3); 
    ImGui::PopStyleVar(2);  
    ImGui::PopFont();
  }

  // Define the stages of your login process
  // FIX: put this in a better spot
  enum class AuthState {
      CheckingFiles,   // Looking for player_data.json
      NeedUsername,    // First time playing
      MainMenu,        // Logged in (playing the game)
      LinkingAccount   // User clicked the "Secure Account" button
  };
  AuthState s_currentAuthState = AuthState::NeedUsername;
  char usernameBuf[32] = "";
  std::string s_currentUsername = "";
  std::string s_currentUUID = "";



  void DrawUsernameSelectionModal()
  {
    //first we check which AuthState we are in to determine where to draw/update stuff goes
    if (s_currentAuthState == AuthState::NeedUsername)
    {

      //draw the popup to get username info
      // 2. Center the popup on the screen
      ImVec2 center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

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
          if (strlen(usernameBuf) > 0) {
            // 1. Generate UUID
            // 2. Save UUID + usernameBuf to local JSON file
            // 3. Update memory
            s_currentUsername = usernameBuf;

            // Move to the next screen!
            s_currentAuthState = AuthState::MainMenu; 

            ImGui::CloseCurrentPopup();
          }
        }
        ImGui::EndPopup();
      }
    // if username is already set (do somthing)
    }
  }

public:

  MainMenuScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a dark blue background for the menu
    menuBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    menuBackground.setFillColor(sf::Color(20, 20, 50));
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "Enter" signals we want to switch to the game state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        nextState = ScreenState::Game;
      }
    }
  }

  void update() override
  {
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

      // 2. Call our neat little helper functions!
    drawServerStatus(screenSize);
    drawTitle(screenSize);
    drawButtons(screenSize);

    // will draw username based on 3 conditions 
    // - new user (prompts for username, then stores it)
    // - logged in (draws username in corner)
    // - searching for username (looking in files/authenticating with servers)
    DrawUsernameSelectionModal(); 

    // 3. End the window
    ImGui::End();
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(menuBackground);
  }
  
  ScreenState getNextState() const override
  {
    return nextState;
  }
};
