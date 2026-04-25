#pragma once
#include <SFML/Graphics.hpp>
#include "../core/SharedData.h"

//an enum to help us switch between screens easily
enum class ScreenState
{
  None,
  Start,
  Loading,
  MainMenu,
  Settings,
  Tables,
  Baccarat,
  Blackjack,
  Poker,
  Quit,
  Demo
};

class Screen
{
protected:
  SharedData& m_shared; // Every screen has a reference to the same data
  ScreenState m_nextState = ScreenState::None;

public:
  virtual ~Screen() = default;

  // member initializer list
  Screen(SharedData& shared) : m_shared(shared) {}

  // every screen must implement these three functions
  virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
  virtual void update() = 0;
  virtual void draw(sf::RenderWindow& window) = 0;

  void processEventsFromServer() // every screen will do this
  {
    while (!m_shared.s_inboundEvents.empty())
    {
      GameEvent incoming = m_shared.s_inboundEvents.front();
      m_shared.s_inboundEvents.pop();

      // GLOBAL EVENTS: You can handle things every screen cares about here!
      // TODO: do handle these events (sys_disconnects etc)

      // Route the event to the specific active screen
      onNetworkEvent(incoming);
    }
  }

  // this function gets overridden by screens and this is where the event is actually processed on screen
  virtual void onNetworkEvent(const GameEvent& event) {}

  // this allows a screen to tell the main loop it's time to switch
  virtual ScreenState getNextState() const = 0;


  // By default, it returns "None", but specific screens can override it!
  virtual std::string getHoveredElement(sf::Vector2f mousePos)
  {
    return "None";
  }

};






