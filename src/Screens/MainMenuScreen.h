#include "Screen.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../../UI/UIHelper.h"

class MainMenuScreen : public Screen
{
private:
  ScreenState nextState = ScreenState::None;
  
  sf::RectangleShape menuBackground;

  //declare the font and UI elements here
  sf::Font font;
  sf::Text titleText;

  //play button
  sf::RectangleShape playButton;
  sf::Text playButtonText;

public:

  MainMenuScreen() : titleText(font), playButtonText(font)
  {
    //set up a dark blue background for the menu
    menuBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    menuBackground.setFillColor(sf::Color(20, 20, 50));

    //load the font firectly from the file
    if (!font.openFromFile("assets/fonts/8bitOperatorPlus8-Regular.ttf"))
    {
      std::cerr << "failed to load font from assets folder!\n";
    }

    //set up the title
    titleText.setFont(font);
    titleText.setString("Baccarat");
    titleText.setCharacterSize(120);
    titleText.setFillColor(sf::Color::White);
    UI::centerOrigin(titleText);
    UI::placeInCenter(titleText);
    
    // set up the play button background
    playButton.setSize(sf::Vector2f({400.0f, 120.0f}));
    playButton.setFillColor(sf::Color(40, 150, 60)); //green button
    //call centerOrigin to center the origin so it calculates the center
    UI::centerOrigin(playButton);
    UI::placeCenteredX(playButton, 800.0f);

    //set up the play button text
    playButtonText.setFont(font);
    playButtonText.setString("Play");
    playButtonText.setCharacterSize(60);
    playButtonText.setFillColor(sf::Color::White);
    UI::centerOrigin(playButtonText);
    UI::placeCenteredX(playButtonText, 800.0f);


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

    //handle mouse clicks
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
      if(mousePressed->button == sf::Mouse::Button::Left)
      {
        //convent the raw pixel coordinates to window corrdinates
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
        
        //check if those coordinates are inside the play button's rectangle
        if (playButton.getGlobalBounds().contains(mousePos))
        {
          nextState = ScreenState::Game;
        }
      }
    }
  }

  void update() override
  {
    // menu animateon logic goes here
  }

  void draw(sf::RenderWindow& window) override
  {
    // draw play button, settings button, background
    window.draw(menuBackground);
    window.draw(titleText);
    window.draw(playButton);     // draw button background first
    window.draw(playButtonText); // draw text on top of it

  }
  
  ScreenState getNextState() const override
  {
    return nextState;
  }
};
