#pragma once
#include <SFML/Graphics.hpp>
#include <string>

//an enum to help us switch between screens easily
enum class ScreenState
{
  None,
  MainMenu,
  Settings,
  Tables,
  Game,
  Quit,
  Demo
};

enum class AuthState
{
  CheckingFiles,   // Looking for player_data.json
  NeedUsername,    // First time playing
  MainMenu,        // Logged in (playing the game)
  LinkingAccount   // User clicked the "Secure Account" button
};

struct SharedData {
  float s_musicVolume = 0.5f;
  float s_sfxVolume = 0.3f;
  bool s_videoHigh = false;
  bool s_videoLow = true;
  int s_currentLanguage = 0;

  // authentication data
  AuthState s_AuthState = AuthState::CheckingFiles;
  std::string s_currentUsername = "";
  std::string s_currentUUID = "";

  bool s_demoWindow = false; // displays the demo window for debugging

};


class Screen
{
protected:
  SharedData& m_shared; // Every screen has a reference to the same data
  
public:
  virtual ~Screen() = default;

  // member initializer list
  Screen(SharedData& shared) : m_shared(shared) {}

  // every screen must implement these three functions
  virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
  virtual void update() = 0;
  virtual void draw(sf::RenderWindow& window) = 0;

  // this allows a screen to tell the main loop it's time to switch
  virtual ScreenState getNextState() const = 0;
};
