// this is a class that represents an area where you can draw things to and it will automatically be resized when different menu states are open
// when you create it you pass in how big you want it to be and where its start position is
// inside that area you specify it will draw the play area evenly
// this class is the base class that will be the template for other games like blackjack and poker and baccarat


// base class will handle the environment
// child class will handle the configuration

#pragma once
#include <SFML/Graphics.hpp>

//the play area for all games will follow these guidelines
enum class WindowStates
{
  MenuOpen = 0,
  MenuClosed
};

//base class
class Area {
  protected:
    //(every child can see and use these variables)
    sf::RectangleShape m_area;
    float m_x, m_y, m_w, m_h; // use these to position your elements
    WindowStates m_currentWindowState;

  public:
    Area(float x, float y, float w, float h)
      : m_x(x), m_y(y), m_w(w), m_h(h),
        m_currentWindowState(WindowStates::MenuClosed)
    {
      //set up the green felt rectangle
      m_area.setFillColor(sf::Color(0, 100, 0)); // Dark Green
      m_area.setOutlineThickness(20);
      m_area.setOutlineColor(sf::Color(139, 69, 19)); // Brown wood border
    }

    virtual ~Area() {}

    void draw(sf::RenderWindow& window)
    {
      window.draw(m_area);
      drawGameElements(window); // draws the game elements implemented by the children
    }

    void setWindowState(WindowStates state)
    {
        m_currentWindowState = state;
        updateInternalLayout(); // recalculate the window
    }

  protected:
    virtual void drawGameElements(sf::RenderWindow& window) = 0;
    virtual void updateInternalLayout() {}
};
