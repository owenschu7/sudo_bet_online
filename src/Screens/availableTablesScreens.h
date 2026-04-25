#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <vector>
#include <string>
#include <iostream>

#include "../core/Debug.h"

class AvailableTablesScreen : public Screen
{
private:

  sf::RectangleShape tableBackground;

  struct Player {
    std::string name;
    int money;
  };

  struct GameTableData {
    int tableID;
    Game game;
    int playerCount;
    int maxPlayers;
    std::string name;
    std::vector<Player> players;
  };

  // myTables holds whatever the servers sends it regarding table data
  std::vector<GameTableData> myTables;

  std::vector<GameTableData> parseAvailableTablesList(const std::string& payload)
  {
    std::vector<GameTableData> parsedTables;
    std::stringstream payloadStream(payload);
    std::string tableSegment;

    // Split the payload by the '|' delimiter
    while (std::getline(payloadStream, tableSegment, '|'))
    {
        // Skip empty segments (like the trailing delimiter)
        if (tableSegment.empty())
        {
            continue;
        }

        std::stringstream segmentStream(tableSegment);
        std::string field;

        // Initialize an empty struct
        GameTableData table; 

        try
        {
            // Parse ID
            if (std::getline(segmentStream, field, ','))
            {
                table.tableID = std::stoi(field);
            }
            // Parse Game Type (requires casting the int back to the Game enum)
            if (std::getline(segmentStream, field, ','))
            {
                table.game = static_cast<Game>(std::stoi(field)); 
            }
            // Parse Current Players
            if (std::getline(segmentStream, field, ','))
            {
                table.playerCount = std::stoi(field);
            }
            // Parse Max Players
            if (std::getline(segmentStream, field, ','))
            {
                table.maxPlayers = std::stoi(field);
            }

            // Note: 'name' and 'players' are not present in the CSV string 
            // from getAvailableTablesListEvent(), so they remain default-initialized 
            // (empty string and empty vector).

            parsedTables.push_back(table);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing table segment '" << tableSegment << "': " << e.what() << "\n";
        }
    }

    return parsedTables;
}



  //table for the available tables
  void drawTable(ImVec2 screenSize, const std::vector<GameTableData>& availableTables)
  {
    // --- 1. Draw Title ---
    ImFont* bigFont = ImGui::GetIO().Fonts->Fonts[1];
    ImGui::PushFont(bigFont);
    const char* titleString = "Available Tables";
    ImVec2 titleSize = ImGui::CalcTextSize(titleString);
    ImGui::SetCursorPos(ImVec2((screenSize.x - titleSize.x) * 0.5f, 40.0f));
    ImGui::Text("%s", titleString);
    ImGui::PopFont();

    // --- 2. Positioning ---
    float padding = 100.0f;
    float bottomSpace = 80.0f;
    float topOffset = padding + 50.0f;
    float tableHeight = screenSize.y - topOffset - bottomSpace;
    ImVec2 tableSize = ImVec2(screenSize.x - (padding * 2), tableHeight);

    ImGui::SetCursorPos(ImVec2(padding, topOffset)); 

    // --- 3. Main Table ---
    ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

    // Safety: Only 1 StyleVar for CellPadding here to keep it simple
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));

    if (ImGui::BeginTable("MainTablesList", 2, flags, tableSize))
    {
      ImGui::TableSetupColumn("Table List", ImGuiTableColumnFlags_WidthStretch, 0.6f);
      ImGui::TableSetupColumn("Action",     ImGuiTableColumnFlags_WidthStretch, 0.2f);
      ImGui::TableHeadersRow();

      for (int i = 0; i < (int)availableTables.size(); ++i)
      {
        const auto& table = availableTables[i];

        ImGui::PushID(i); 

        // Start the main row
        ImGui::TableNextRow(ImGuiTableRowFlags_None, 40.0f); 

        // --- Column 0: Tree Node ---
        ImGui::TableNextColumn();

        // We REMOVE SpanAllColumns here because it often conflicts with nested tables 
        // inside the same row structure. SpanFullWidth is enough.
        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

        // TODO: change the table name to be user generated (need to prompt user after pressing create table)
        bool isRowExpanded = ImGui::TreeNodeEx((void*)(intptr_t)i, treeFlags, "%s", GameToString(table.game)); // prints the name of the table
        //table is the current table in the availabletables vector

        // --- Column 2: Action ---
        // present a join button regardless of if there is people in it
        ImGui::TableNextColumn();
        if (ImGui::Button("JOIN", ImVec2(-FLT_MIN, 0)))
        {
          DEBUG_PRINT << "availableTablesScreen.h: drawTable(): joining " << table.name << std::endl;

          //create event
          GameEvent event;
          event.action = Action::JOIN_Table;
          event.game = table.game;
          event.senderUUID = m_shared.s_currentUUID;
          event.senderUsername = m_shared.s_currentUsername;
          //send event
          m_shared.s_outboundEvents.push(event);
          DEBUG_PRINT << "SENDING EVENT: " << event << "\n";
          //m_nextState = table game they are joining
        }

        // --- 4. Sub-Table (The problematic part) ---
        if (isRowExpanded)
        {
          // We must move to a NEW row to house the sub-table
          ImGui::TableNextRow();
          // We use TableSetColumnIndex to make sure we are drawing in the first column
          ImGui::TableSetColumnIndex(0); 

          // We use a dummy or a group to ensure the sub-table doesn't 
          // accidentally try to fill Column 1 or 2 of the parent.
          ImGui::BeginGroup();
          ImGui::Indent(20.0f);

          if (table.players.empty()) {
            ImGui::TextDisabled("No players currently at this table.");
          } else {
            // Give the child table a unique ID based on the parent index
            char childId[32];
            sprintf(childId, "ChildTable_%d", i);

            if (ImGui::BeginTable(childId, 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
              ImGui::TableSetupColumn("Player");
              ImGui::TableSetupColumn("Balance");
              ImGui::TableHeadersRow();

              for (const auto& p : table.players) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::Text("%s", p.name.c_str());
                ImGui::TableNextColumn(); ImGui::Text("$%d", p.money);
              }
              ImGui::EndTable();
            }
          }
          ImGui::Unindent(20.0f);
          ImGui::EndGroup();

          ImGui::TreePop(); 
        }

        ImGui::PopID(); 
      }
      ImGui::EndTable();
    }
    ImGui::PopStyleVar(1); 
  }

  void drawButton_AddTable(ImVec2 screenSize, const std::vector<GameTableData>& myTables)
  {
    // Define your button size
    float buttonWidth = 220.0f;
    float buttonHeight = 60.0f;

    // Calculate the absolute position: Bottom Right Corner
    // X = Screen width minus padding minus button width
    // Y = Screen height minus padding minus button height
    float paddingX = 120.0f; // Matches your table padding
    float paddingY = 30.0f;  // Keeps it just off the very bottom edge    // 3. Set the cursor and draw

    ImGui::SetCursorPosX(screenSize.x - buttonWidth - paddingX);
    ImGui::SetCursorPosY(screenSize.y - buttonHeight - paddingY);

    // Optional: Give it a distinct color (like a subtle blue) so it doesn't blend in
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.7f, 1.0f));

    if (ImGui::Button("Add New Table", ImVec2(buttonWidth, buttonHeight)))
    {
      // Your logic to add a table to myTables goes here
      ImGui::OpenPopup("ADDING NEW TABLE");
    }

    ImGui::PopStyleColor(2);
  }


  void DrawGameSelectionPopup()
  {
    // 2. Center the popup on the screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // 3. Define the flags to make it static (no moving, no resizing, no collapsing)
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | 
      ImGuiWindowFlags_NoResize | 
      ImGuiWindowFlags_AlwaysAutoResize | 
      ImGuiWindowFlags_NoCollapse;

    // 4. Begin the modal
    if (ImGui::BeginPopupModal("ADDING NEW TABLE", NULL, flags)) {
      ImGui::Text("Please select a game to play:");
      ImGui::Separator();

      //print the buttons in the center horizontally
      // 1. Get the current width of the popup window
      float windowWidth = ImGui::GetWindowSize().x;

      // 2. Calculate the exact width of the auto-sized button
      const char* text = "Baccarat";
      float textWidth = ImGui::CalcTextSize(text).x;
      float padding = ImGui::GetStyle().FramePadding.x * 2.0f;
      float buttonWidth = textWidth + padding;

      ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

      // Game selection buttons
      if (ImGui::Button("Baccarat", ImVec2(0, 0)))
      {
        //create event
        GameEvent event;
        event.action = Action::CREATE_Table;
        event.game = Game::BACCARAT;
        event.senderUUID = m_shared.s_currentUUID;
        event.senderUsername = m_shared.s_currentUsername;
        //send event
        m_shared.s_outboundEvents.push(event);
        DEBUG_PRINT << "SENDING EVENT: " << event << "\n";

        ImGui::CloseCurrentPopup(); // Closes the modal upon selection
      }

      //print the buttons in the center horizontally
      // 1. Get the current width of the popup window
      windowWidth = ImGui::GetWindowSize().x;

      // 2. Calculate the exact width of the auto-sized button
      text = "Blackjack";
      textWidth = ImGui::CalcTextSize(text).x;
      padding = ImGui::GetStyle().FramePadding.x * 2.0f;
      buttonWidth = textWidth + padding;

      ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

      if (ImGui::Button("Blackjack", ImVec2(0, 0)))
      {
        //create event
        GameEvent event;
        event.action = Action::CREATE_Table;
        event.game = Game::BLACKJACK;
        event.senderUUID = m_shared.s_currentUUID;
        event.senderUsername = m_shared.s_currentUsername;
        //send event
        m_shared.s_outboundEvents.push(event);
        DEBUG_PRINT << "SENDING EVENT: " << event << "\n";

        ImGui::CloseCurrentPopup();
      }

      //print the buttons in the center horizontally
      // 1. Get the current width of the popup window
      windowWidth = ImGui::GetWindowSize().x;

      // 2. Calculate the exact width of the auto-sized button
      text = "Poker";
      textWidth = ImGui::CalcTextSize(text).x;
      padding = ImGui::GetStyle().FramePadding.x * 2.0f;
      buttonWidth = textWidth + padding;

      ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

      if (ImGui::Button("Poker", ImVec2(0, 0)))
      {
        //create event
        GameEvent event;
        event.action = Action::CREATE_Table;
        event.game = Game::POKER;
        event.senderUUID = m_shared.s_currentUUID;
        event.senderUsername = m_shared.s_currentUsername;
        //send event
        m_shared.s_outboundEvents.push(event);
        DEBUG_PRINT << "SENDING EVENT: " << event << "\n";

        ImGui::CloseCurrentPopup();
      }

      ImGui::Separator();

      // Optional cancel button
      if (ImGui::Button("Cancel", ImVec2(0, 0))) {
        ImGui::CloseCurrentPopup();
      }

      // End the popup
      ImGui::EndPopup();
    }
  }



public:
  AvailableTablesScreen(SharedData &sharedData) : Screen(sharedData)
  {
    //set up a green background
    tableBackground.setSize(sf::Vector2f({1920.0f, 1080.0f}));
    tableBackground.setFillColor(sf::Color(20, 20, 50));
  }

  void handleEvent(const sf::Event& event, sf::RenderWindow& window) override
  {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
      //pressing "escape" signals we want to switch to the main menu state
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
      {
        m_nextState = ScreenState::MainMenu;
      }
    }
  }

  void update() override
  {
    processEventsFromServer(); // used to actually check if there are any incoming packets in the queue

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(screenSize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                   ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus; // used so that demo window can display over the screen

    ImGui::Begin("Table Layer", nullptr, windowFlags);
    // 1. Define how much vertical space you need for the button + padding
    float bottomReservedSpace = 100.0f; 

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    // 2. Wrap the table in a Child Window. 
    // ImVec2(0.0f, -bottomReservedSpace) means: 
    // Width = 0 (Fill width), Height = Fill available height MINUS bottomReservedSpace
    ImGui::BeginChild("TableScrollRegion", ImVec2(0.0f, -bottomReservedSpace), false);
    drawTable(screenSize, myTables);
    ImGui::PopStyleColor(1);
    ImGui::EndChild();

    // 3. The cursor is now safely positioned below the child window
    // Draw your button here, and it will sit cleanly at the bottom
    drawButton_AddTable(screenSize, myTables);

    DrawGameSelectionPopup();

    ImGui::End();
  }

  void onNetworkEvent(const GameEvent& event) override
  {
    TRACE_FUNCTION();
    // Handle events specific only related to the screen
    switch (event.action)
    {
      case Action::GET_AvailableTables_Response:
        DEBUG_PRINT << "GET_AvailableTables_Response\n";
        //clear original vector list
        myTables.clear();
        // parse the packet
        myTables = parseAvailableTablesList(event.stringPayload);
        break;
      default:
        break;
    }
  }


  void draw(sf::RenderWindow& window) override
  {
    window.draw(tableBackground);
  }
  
  ScreenState getNextState() const override
  {
    return m_nextState;
  }
};
