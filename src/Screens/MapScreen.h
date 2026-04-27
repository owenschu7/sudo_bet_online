#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "../core/Debug.h"
#include "../UI/menuButton.h"
#include "../UI/textLabel.h"
#include "../UI/UIHelper.h"

#include "../core/GameEvents.h"

class MapScreen : public Screen
{
private:
  //Button m_exitButton;

  //Label m_usernameDisplay;

  sf::RectangleShape menuBackground;

  // --- UI HELPER METHODS ---


public:
  MapScreen(SharedData &sharedData)
    : Screen(sharedData)
  {
    //set up a dark blue background for the menu
    menuBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    menuBackground.setFillColor(sf::Color::White);


    //setup labels
    //m_usernameDisplay.setup("Username: ", 30, sf::Color::White, {30.0f, UI::SCREEN_H - 60.0f}, false);
  }

  //handle events that came in from server to the client
  void onNetworkEvent(const GameEvent& event) override
  {
    TRACE_FUNCTION();
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    //mouse clicks on buttons

    //keyboard clicks
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "Enter" signals we want to switch to the game state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
      {
        //m_nextState = ScreenState::Baccarat;
      }
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
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


    //m_usernameDisplay.setString("Username: " + m_shared.s_currentUsername);


    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(screenSize);
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Menu Layer", nullptr, windowFlags);

    ImGui::End();
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(menuBackground);

    if (!m_shared.s_needUsername)
    {
      //m_usernameDisplay.draw(window);
    }
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
