#include <SFML/Graphics.hpp>
#include <memory>
#include "Screens/Screen.h"
#include "Screens/MainMenuScreen.h"
#include "Screens/GameScreen.h"
//include other screens here

int main()
{
  //            sf::RenderWindow: this created the actual window on your operating system.
  //                              you are passing it a VideoMode and the title that appears at the top 
  //                              of the window. the u just tells C++ that these are "unsigned" integers
  auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");

  //without this your program will run as fast as your cpu possiblly can
  //this caps it at 60fps
  window.setFramerateLimit(60);

  //create a virtual camera locked exactaly to 1920x1080
  sf::View view(sf::FloatRect({0.f, 0.f}, {1920.f, 1080.f}));
  //tell the window to look through this camera
  window.setView(view);

  //start by loading the main menu
  //currentScreen will hold the current screen we are on (main menu, game screen, settings menu etc)
  std::unique_ptr<Screen> currentScreen = std::make_unique<MainMenuScreen>();


  // the main game loop inside this is where two things happen, Handling events and Rendering
  while (window.isOpen())
  {
    // the event loop (input)
    // these actions are queued up as "events"
    //                                 window.pollEvent() - this grabs the oldest event waiting in the queue
    //                                                      it returns a std::optional, which basically means
    //                                                      "here is an event, or here is nothing(if the queue is empty)"
    //while () - because the user might press a key
    //           and move at the exact same time there
    //           might be multiple events waiting,
    //           this inner loop processes all pending
    //           events before moving on
    while (const std::optional event = window.pollEvent())
    {
      // (event->is<sf::Event::Closed>()) - this checks if the current event is the user trying to close the
      //                                    window. if it is, you call window.close() which breaks the main
      //                                    game loop on the next pass and cleanly ends the program
      if (event->is<sf::Event::Closed>())
      {
        window.close();
      }

      // pass the event down to whatever screen is currently active
      currentScreen->handleEvent(*event, window);
    }

    //state switching logic
    // on every single frame, the manager asks the active screen
    // do you want to switch to a different screen?
    // if the user pressed somithing which triggers its internal state to change screens
    // then the manager will switch to the nextState
    ScreenState nextState = currentScreen->getNextState();
    if (nextState != ScreenState::None)
    {
      if (nextState == ScreenState::MainMenu)
      {
        currentScreen = std::make_unique<MainMenuScreen>();
      } else if (nextState == ScreenState::Game)
      {
        currentScreen = std::make_unique<GameScreen>();
      }//add new screens here
    }

    //update and draw the active screen
    currentScreen->update();

    //SFML uses a technique called Double Buffering.
    // Imagine an artist painting on a canvas hidden bedind a curtain, and when they are done,
    // they swap, it with the canvas the aduience is currently looking at

    // this wipes the hidden canvas clean by default it clears to black.
    // if you don't do this your new frame will just smear on top of the old frame
    window.clear();

    currentScreen->draw(window);

    // this drops the curtain and swaps the hidden canvas (which you just drew on) with the visible one on your monitor
    window.display();
  }
}





