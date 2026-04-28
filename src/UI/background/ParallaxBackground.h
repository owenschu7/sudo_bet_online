#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include "ScrollingBackground.h" // Your exact file from earlier

class ParallaxBackground {
private:
    std::vector<ScrollingBackground> layers;

public:
    // Adds a new layer to the parallax. 
    // Order matters: Add the furthest back layer first!
    void addLayer(const sf::Texture& texture, float speed, float scale) {
        layers.emplace_back(texture, speed, scale);
    }

    // Updates all layers automatically
    void update(sf::Time dt) {
        for (auto& layer : layers) {
            layer.update(dt);
        }
    }

    // Draws all layers in order (back to front)
    void draw(sf::RenderTarget& target) {
        for (auto& layer : layers) {
            layer.draw(target);
        }
    }
};
