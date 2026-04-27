#pragma once

#include <SFML/Graphics.hpp>
#include <memory> // unique_ptr
#include <string>
#include <imgui.h>
#include <imgui-SFML.h>

#include "../../core/SharedData.h"
#include "../../core/SettingsManager.h"
#include "../../Screens/Screen.h"
#include "../../Screens/StartScreen.h"
#include "../../Screens/MainMenuScreen.h"
#include "../../Screens/BaccaratScreen.h"
#include "../../Screens/SettingsScreen.h"
#include "../../Screens/demoScreen.h"
#include "../../Screens/availableTablesScreens.h"

#include "../../network/client/NetworkClient.h"
#include "../../network/PacketParser.h"

#include "../../core/uuid.h"
#include "../../core/Debug.h"


class clientApplication
{
public:
  clientApplication() : m_settings("data/settings.txt")
  {
    initWindow();
    initImGui();
    initData();
    loadAssets();

    //load the main menu
    //currentScreen will hold the current screen we are on (main menu, game screen, settings menu etc)
    m_currentScreen = std::make_unique<StartScreen>(m_sharedData);
  }

  ~clientApplication()
  {
    // Clean up resources when the window is finally closed
    ImGui::SFML::Shutdown();
  }

  void run()
  {
    while (m_window.isOpen())
    {
      processEvents();
      update();
      render();
    }
  }

private:
  //init functions
  void initWindow()
  {
    //m_window is a sf::RenderWindow: this created the actual window on your operating system.
    //  you are passing it a VideoMode and the title that appears at the top 
    //  of the window. the "u" just tells C++ that these are "unsigned" integers
    m_window.create(sf::VideoMode({1920u,1080u}), "Baccarat Online");

    //without this your program will run as fast as your cpu possiblly can
    //this caps it at 60fps
    m_window.setFramerateLimit(60);

    //create a virtual camera locked exactaly to 1920x1080
    sf::View view(sf::FloatRect({0.f, 0.f}, {1920.f, 1080.f}));

    //tell the window to look through this camera
    m_window.setView(view);
  }

  void initImGui()
  {
    if (!ImGui::SFML::Init(m_window, false))
    {
      std::cerr << "Initialization failed! We can't safely use ImGui.\n";
      exit(-1);
    }
    // Stop ImGui from overriding our custom SFML cursor
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  }

  void initData()
  {
    m_settings.Load(m_sharedData); // loads data from settings.txt and stores it in m_sharedData

    // create a UUID if there is none
    if (m_sharedData.s_currentUUID.empty())
    {
      // Generate a new local UUID for the offline client
      m_sharedData.s_currentUUID = generateRealUUID();
      // Save the new UUID immediately so it persists next time
      m_settings.Save(m_sharedData);
      m_sharedData.s_needUsername = true;
    }
    if (m_sharedData.s_currentUsername.empty())
    {
      m_sharedData.s_needUsername = true;
    } else {
      m_sharedData.s_needUsername = false;
    }

    //force an update to load session settings
    m_sharedData.s_settingsChanged = true;
  }

  void loadAssets() 
  {
    ImGuiIO& io = ImGui::GetIO();

    // 1. Load ImGui Fonts
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("assets/fonts/8bitOperatorPlus8-Regular.ttf", 20.0f);
    if (!defaultFont) { std::cerr << "Failed to load default font!\n"; exit(-1); }

    ImFont* bigFont = io.Fonts->AddFontFromFileTTF("assets/fonts/8bitOperatorPlus8-Regular.ttf", 70.0f);
    if (!bigFont) { std::cerr << "Failed to load big font!\n"; exit(-1); }

    ImFont* titleButtonFont = io.Fonts->AddFontFromFileTTF("assets/fonts/8bitOperatorPlus8-Regular.ttf", 40.0f);
    if (!titleButtonFont) { std::cerr << "Failed to load title button font!\n"; exit(-1); }

    if (!ImGui::SFML::UpdateFontTexture()) 
    {
      std::cerr << "Failed to update ImGui font texture!\n";
    }


    // Inside clientApplication::loadAssets()
    m_sharedData.s_assets.loadTexture("UIDemo", "assets/images/UIPack/UI assets Demo (2x).png");
    m_sharedData.s_assets.loadTexture("BlackAndWhiteUI", "assets/images/UI/BlackandWhiteUI.png");


    // 2. Load SFML Global Font
    // We load this into m_sharedData so all screens can use it instantly
    if (!m_sharedData.s_assets.loadFont("gameFont", "assets/fonts/8bitOperatorPlus8-Regular.ttf"))
    {
      std::cerr << "Failed to load SFML game font via AssetManager!\n";
      exit(-1);
    }
    if (!m_sharedData.s_assets.loadFont("gameFontTitle", "assets/fonts/monogram-extended-italic.ttf"))
    {
      std::cerr << "Failed to load SFML title font via AssetManager!\n";
      exit(-1);
    }
    if (!m_sharedData.s_assets.loadFont("gameNumbersFont", "assets/fonts/PixelBlock.ttf"))
    {
      std::cerr << "Failed to load SFML title font via AssetManager!\n";
      exit(-1);
    }

    // 3. Load UI Textures using the AssetManager
    if (!m_sharedData.s_assets.loadTexture("HUD_Coin", "assets/images/UI/playerHUDCoin.png"))
    {
      std::cerr << "Failed to load HUD texture via AssetManager!\n";
    }

    sf::Image cursorImage;
    if (!cursorImage.loadFromFile("assets/images/UI/cursor.png"))
    {
      std::cerr << "Failed to load cursor image!\n";
    }

    m_cursor = sf::Cursor::createFromPixels(
      cursorImage.getPixelsPtr(),   // raw RGBA pixel array
      cursorImage.getSize(),        // sf::Vector2u size
      sf::Vector2u(0, 0)           // hotspot
    );

    if (m_cursor.has_value())
    {
      m_window.setMouseCursor(m_cursor.value());
    }
    else
    {
      std::cerr << "Failed to create cursor!\n";
    }
    m_sharedData.s_assets.loadAllCardTextures("assets/images/Cards2/");
  }

  //main loop phases
  void processEvents()
  {
    // the event loop (input)
    // these actions are queued up as "events"
    //                                 m_window.pollEvent() - this grabs the oldest event waiting in the queue
    //                                                      it returns a std::optional, which basically means
    //                                                      "here is an event, or here is nothing(if the queue is empty)"
    //while () - because the user might press a key
    //           and move at the exact same time there
    //           might be multiple events waiting,
    //           this inner loop processes all pending
    //           events before moving on

    while (const std::optional event = m_window.pollEvent())
    {
      //pass the event to imgui so it can catch mouse clicks, typing, etc
      ImGui::SFML::ProcessEvent(m_window, *event);

      // (event->is<sf::Event::Closed>()) - this checks if the current event is the user trying to close the
      //                                    window. if it is, you call window.close() which breaks the main
      //                                    game loop on the next pass and cleanly ends the program
      if (event->is<sf::Event::Closed>())
      {
        m_window.close();
      }

//    if (const auto* resized = event->getIf<sf::Event::Resized>())
//    {
//      //update the view to match the new window size exactaly
//      sf::FloatRect visibleArea({0.0f, 0.0f},
//                                {static_cast<float>(resized->size.x),
//                                static_cast<float>(resized->size.y)});

//      //tell the window to look through this camera
//      m_window.setView(sf::View(visibleArea));
//    }

      // ui screensize handling
      if (const auto* resized = event->getIf<sf::Event::Resized>())
      {
        // 1. Calculate the aspect ratio of the physical window vs our 1920x1080 game
        float windowRatio = static_cast<float>(resized->size.x) / static_cast<float>(resized->size.y);
        float viewRatio = 1920.0f / 1080.0f;

        float sizeX = 1.0f;
        float sizeY = 1.0f;
        float posX = 0.0f;
        float posY = 0.0f;

        // 2. Math to figure out if the black bars go on the top/bottom or left/right
        if (windowRatio >= viewRatio) 
        {
            // Window is wider than the game (Black bars on left/right)
            sizeX = viewRatio / windowRatio;
            posX = (1.0f - sizeX) / 2.0f;
        } 
        else 
        {
            // Window is taller than the game (Black bars on top/bottom)
            sizeY = windowRatio / viewRatio;
            posY = (1.0f - sizeY) / 2.0f;
        }

        // 3. Apply the viewport to the camera
        sf::View view = m_window.getView();
        view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
        m_window.setView(view);
      }

      // pass the event down to whatever screen is currently active
      m_currentScreen->handleEvent(*event, m_window);
    }
  }


  void update()
  {
    //update imgui before your screens update
    ImGui::SFML::Update(m_window, m_deltaClock.restart());

    // process the mail carrier
    processNetwork();

    applySettingsChanges(); // this is where the changes (if any) will be applied to the game
    handleScreenTransitions(); // changes screens if needed

    //update and draw the active screen
    m_currentScreen->update(m_window);

    drawGlobalDebugUI(); // runs after the screen updates so it draws on top of everything!
  }

  void render()
  {
    //SFML uses a technique called Double Buffering.
    // Imagine an artist painting on a canvas hidden bedind a curtain, and when they are done,
    // they swap, it with the canvas the aduience is currently looking at

    // this wipes the hidden canvas clean by default it clears to black.
    // if you don't do this your new frame will just smear on top of the old frame
    m_window.clear();
    m_currentScreen->draw(m_window);

    ImGui::SFML::Render(m_window); // Render the UI on top of whatever the current screen just drew
    m_window.display(); // this drops the curtain and swaps the hidden canvas (which you just drew on) with the visible one on your monitor
  }

  //network
  void processNetwork()
  {
    /*======================================================================
     * OUTBOUND EVENTS 
     * all events in the outbound queue gets sent to server
     *======================================================================*/
    while (!m_sharedData.s_outboundEvents.empty())
    {
      DEBUG_PRINT << "application.cpp: processNetwork():\n";
      GameEvent event = m_sharedData.s_outboundEvents.front();
      m_sharedData.s_outboundEvents.pop();

      /*================================================================
      * SYSTEM EVENTS
      * if the packet is a sys_connect we dont send a packet, instead we call connectToServer()
      ================================================================*/
      if (event.action == Action::SYS_Connect)
      {
        DEBUG_PRINT << "sys_connect detected\n";
        m_network.connectToServer("127.0.0.1", "8080");
      }

      /*================================================================
      * GAME EVENTS (Bets, Raises, Checks etc)
      * These need to be packed into binary and shipped across the internet.
      ================================================================*/
      DEBUG_PRINT << "\nSENDING EVENT: " << event <<"\n";

      PacketBuilder packet;
      packet.append8(static_cast<uint8_t>(event.action)); // OpCode (1 byte)
      packet.append8(static_cast<uint8_t>(event.game)); // OpCode (1 byte) NEW!!!!!!!!!!
      packet.appendString(event.senderUUID.c_str());    // UUID (Dynamic length + Null term)
      packet.appendString(event.senderUsername.c_str()); // Username + Null term
      packet.append32(event.intPayload);                // Int (4 bytes, Network Byte Order)
      packet.appendString(event.stringPayload.c_str()); // String (Dynamic length + Null term)
      packet.finalize();

      m_network.sendPacket(packet);
    }

    /*======================================================================
     * INBOUND EVENTS
     * SERVER -> CLIENT
     * these events go directly into s_inboundEvents to be processed by the screens
     *======================================================================*/
    std::vector<uint8_t> rawPacket;

    // grab a packet if there is one
    while (m_network.pollPacket(rawPacket))
    {
      GameEvent newEvent = PacketParser::parse(rawPacket);
      DEBUG_PRINT << "\nNEW EVENT RECIEVED: " << newEvent << "\n";
      m_sharedData.s_inboundEvents.push(newEvent);
    }
  }

  //game state and UI helpers
  void handleScreenTransitions()
  {
    //state switching logic
    // on every single frame, the manager asks the active screen
    // do you want to switch to a different screen?
    // if the user pressed somithing which triggers its internal state to change screens
    // then the manager will switch to the nextState
    ScreenState nextState = m_currentScreen->getNextState();
    if (nextState != ScreenState::None) 
    {
      switch (nextState) 
      {
        case ScreenState::Start:
          m_currentScreen = std::make_unique<StartScreen>(m_sharedData); break;
        case ScreenState::MainMenu:
          m_currentScreen = std::make_unique<MainMenuScreen>(m_sharedData); break;
        case ScreenState::Settings:
          m_currentScreen = std::make_unique<SettingsScreen>(m_sharedData); break;
        case ScreenState::Tables:
          m_currentScreen = std::make_unique<AvailableTablesScreen>(m_sharedData); break;
        case ScreenState::Baccarat:
          m_currentScreen = std::make_unique<BaccaratScreen>(m_sharedData); break;
        case ScreenState::Demo:
          m_currentScreen = std::make_unique<DemoScreen>(m_sharedData); break;
        case ScreenState::Quit:
          m_window.close(); break;
        default: break;
      }
    }
  }
  void applySettingsChanges()
  {
    if (m_sharedData.s_settingsChanged)
    {
      m_sharedData.s_lightMode ? ImGui::StyleColorsLight() : ImGui::StyleColorsDark();
      m_settings.Save(m_sharedData);

      //after we apply changes we set it to false to catch for other changes in
      // the future so that we dont constantly waste compute
      m_sharedData.s_settingsChanged = false;
    }
  }

  void drawGlobalDebugUI()
  {
    //for if the user wants to show these
    static bool showMouseTracker = false;
    static bool showHoverTracker = false;

    //displays on top left corner
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));

    // Make it an invisible floating window with no title bar or background
    ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoBackground | 
      ImGuiWindowFlags_AlwaysAutoResize | 
      ImGuiWindowFlags_NoMove | 
      ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("Global Debug", nullptr, overlayFlags);

    // Draw the checkbox and bind it directly to your SharedData bool
    ImGui::Checkbox("Show Demo", &m_sharedData.s_demoWindow);
    ImGui::Checkbox("Debug", &m_sharedData.s_debugMode);
    ImGui::End();

    // draw the actual demo window if the checkbox is ticked
    if (m_sharedData.s_demoWindow)
    {
      ImGui::ShowDemoWindow(&m_sharedData.s_demoWindow);
    }

    // New Developer Tools window
    if (m_sharedData.s_debugMode)
    {
      ImGui::Begin("Developer Tools", &m_sharedData.s_debugMode);
      
      if (ImGui::CollapsingHeader("Network Stats", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::Text("Inbound Events: %zu", m_sharedData.s_inboundEvents.size());
        ImGui::Text("Outbound Events: %zu", m_sharedData.s_outboundEvents.size());
        ImGui::Text("Online: %s", m_sharedData.s_isOnline ? "Yes" : "No");
      }

      ImGui::Separator();

      if (ImGui::CollapsingHeader("Game State"))
      {
        ImGui::Text("things can go here");
      }

      ImGui::Separator();

      if (ImGui::CollapsingHeader("Cheats / Tools"))
      {
        if (ImGui::Button("Simulate Event")) { /* Logic goes here */ }
        ImGui::SameLine();
        if (ImGui::Button("Clear Queues")) { /* Logic goes here */ }
      }

      ImGui::Separator();

      if (ImGui::CollapsingHeader("Mouse/Positioning"))
      {
        // Buttons to toggle our new popout windows
        if (ImGui::Button("Toggle Mouse Position")) 
        { 
          showMouseTracker = !showMouseTracker; 
        }
        ImGui::SameLine();
        if (ImGui::Button("Toggle Hover Tracker")) 
        { 
          showHoverTracker = !showHoverTracker; 
        }
      }
      ImGui::End();
    }

    // ==========================================
    // POPOUT WINDOW 1: Real-time Mouse Tracker
    // ==========================================
    if (showMouseTracker)
    {
      // Passing &showMouseTracker adds an "X" close button to the corner
      ImGui::Begin("Mouse Position Tracker", &showMouseTracker, ImGuiWindowFlags_AlwaysAutoResize);
      
      ImVec2 mousePos = ImGui::GetIO().MousePos;
      ImGui::Text("X: %.1f", mousePos.x);
      ImGui::Text("Y: %.1f", mousePos.y);
      
      ImGui::End();
    }

    // ==========================================
    // POPOUT WINDOW 2: Element Hover Tracker
    // ==========================================
    if (showHoverTracker)
    {
      ImGui::Begin("SFML Hover Tracker", &showHoverTracker, ImGuiWindowFlags_AlwaysAutoResize);

      // 1. Get the raw pixel position from ImGui (Monitor Coordinates)
      ImVec2 mousePos = ImGui::GetIO().MousePos;
      sf::Vector2i pixelPos(static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));

      // 2. Convert physical pixels to virtual game coordinates!
      sf::Vector2f sfMousePos = m_window.mapPixelToCoords(pixelPos);

      // Ask the active screen what we are hovering over using the fixed coordinates
      std::string hoveredItem = "None";
      if (m_currentScreen)
      {
          hoveredItem = m_currentScreen->getHoveredElement(sfMousePos);
      }

      ImGui::Text("Currently Hovering:");
      ImGui::Separator();

      if (hoveredItem != "None" && hoveredItem != "Table Background")
      {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "> %s <", hoveredItem.c_str());
      }
      else
      {
        ImGui::Text("%s", hoveredItem.c_str());
      }

      ImGui::End();
    }
  }

  //utils
  std::string generateRealUUID() 
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    uuids::uuid_random_generator uuid_gen{gen};
    uuids::uuid const id = uuid_gen();
    return uuids::to_string(id);
  }

  //prints a packet to debug
  void logPacket(PacketBuilder packet)
  {
    const uint8_t* rawData = packet.getPtr();
    size_t totalSize = packet.getSize();
    DEBUG_PRINT << "[PACKET DUMP]\n"
      << "Size: " << totalSize << " bytes\n"
      << "Data: ";
    for (size_t i = 0; i < totalSize; i++)
    {
      // Print each byte as a 2-digit Hex number (e.g., 0A, FF, 62)
      DEBUG_PRINTF("%02X ", rawData[i]); 
    }
    DEBUG_PRINT << "\n";
  }

  //core variables
  sf::RenderWindow m_window;
  sf::Clock m_deltaClock; // ImGui needs a clock to track time between frames

  //data and settings
  SharedData m_sharedData;
  SettingsManager m_settings;
  NetworkClient m_network;

  //cursor
  std::optional<sf::Cursor> m_cursor;

  //state machine
  std::unique_ptr<Screen> m_currentScreen;
};
