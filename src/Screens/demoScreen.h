#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

class DemoScreen : public Screen
{
public:
    // Constructor matching your architecture
    DemoScreen(SharedData& sharedData) : Screen(sharedData) {}

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override 
    {
        // No custom events needed, ImGui handles the demo window clicks!
    }

    void update(sf::RenderWindow& window) override 
    {
        // This spawns the massive ImGui test window
        ImGui::ShowDemoWindow(); 
    }

    void draw(sf::RenderWindow& window) override 
    {
        // Just draw a plain black background behind the demo
        window.clear(sf::Color::Black); 
    }
    
    ScreenState getNextState() const override 
    {
        // Never switch states automatically
        return ScreenState::None; 
    }
};
