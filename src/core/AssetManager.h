#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>

class AssetManager
{
private:
  std::unordered_map<std::string, sf::Texture> m_textures;
  std::unordered_map<std::string, sf::Font> m_fonts;

public:
  AssetManager() = default;

  // --- TEXTURES ---
  bool loadTexture(const std::string& name, const std::string& filename)
  {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
      std::cerr << "Error: Could not load texture " << filename << "\n";
      return false;
    }
    m_textures[name] = std::move(tex);
    return true;
  }

  const sf::Texture& getTexture(const std::string& name) const
  {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) return it->second;
    throw std::runtime_error("Texture not found: " + name);
  }

  // --- FONTS ---
  bool loadFont(const std::string& name, const std::string& filename)
  {
    sf::Font font;
    if (!font.openFromFile(filename)) {
      std::cerr << "Error: Could not load font " << filename << "\n";
      return false;
    }
    m_fonts[name] = std::move(font);
    return true;
  }

  const sf::Font& getFont(const std::string& name) const
  {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) return it->second;
    throw std::runtime_error("Font not found: " + name);
  }

  // --- BULK LOADERS ---
  // --- BULK LOADERS ---
  void loadAllCardTextures(const std::string& folderPath)
  {
    const std::string suits = "SHCD";

    for (char suit : suits) {

      // 1. Convert the char to the Capitalized suit word
      std::string suitWord;
      if (suit == 'S') suitWord = "Spades";
      else if (suit == 'H') suitWord = "Hearts";
      else if (suit == 'C') suitWord = "Clubs";
      else if (suit == 'D') suitWord = "Diamonds";

      for (int value = 1; value <= 13; ++value) {

        // 2. Convert the number to the single uppercase rank letter
        std::string rankWord;
        if (value == 1) rankWord = "A";
        else if (value == 11) rankWord = "J";
        else if (value == 12) rankWord = "Q";
        else if (value == 13) rankWord = "K";
        else rankWord = std::to_string(value); // 2 through 10 stay numbers

        // 3. Keep the original short key for your game logic (e.g., "1H", "11S")
        std::string key = std::to_string(value) + suit;

        // 4. Build the actual file name string (e.g., "cardSpadesJ.png" or "cardHearts9.png")
        std::string filename = folderPath + "card" + suitWord + rankWord + ".png";

        loadTexture(key, filename);
      }
    }

    // Don't forget to check what the back of the card is named in your new asset pack!
    // You might need to change "card_back_blue.png" to match the new file.
    loadTexture("cardBack", folderPath + "cardBack_red1.png");
  }
};
