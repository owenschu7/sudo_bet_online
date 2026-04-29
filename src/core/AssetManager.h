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
    if (!font.openFromFile(filename))
    {
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

  // --- BULK LOADERS --- //assets/images/Cards/
  void loadAllCardTextures(const std::string& folderPath)
  {
    loadTexture("2D", folderPath + "cardDiamonds2.png");
    loadTexture("3D", folderPath + "cardDiamonds3.png");
    loadTexture("4D", folderPath + "cardDiamonds4.png");
    loadTexture("5D", folderPath + "cardDiamonds5.png");
    loadTexture("6D", folderPath + "cardDiamonds6.png");
    loadTexture("7D", folderPath + "cardDiamonds7.png");
    loadTexture("8D", folderPath + "cardDiamonds8.png");
    loadTexture("9D", folderPath + "cardDiamonds9.png");
    loadTexture("10D", folderPath + "cardDiamonds10.png");
    loadTexture("AD", folderPath + "cardDiamondsA.png");
    loadTexture("JD", folderPath + "cardDiamondsJ.png");
    loadTexture("QD", folderPath + "cardDiamondsQ.png");
    loadTexture("KD", folderPath + "cardDiamondsK.png");

    loadTexture("2H", folderPath + "cardHearts2.png");
    loadTexture("3H", folderPath + "cardHearts3.png");
    loadTexture("4H", folderPath + "cardHearts4.png");
    loadTexture("5H", folderPath + "cardHearts5.png");
    loadTexture("6H", folderPath + "cardHearts6.png");
    loadTexture("7H", folderPath + "cardHearts7.png");
    loadTexture("8H", folderPath + "cardHearts8.png");
    loadTexture("9H", folderPath + "cardHearts9.png");
    loadTexture("10H", folderPath + "cardHearts10.png");
    loadTexture("JH", folderPath + "cardHeartsJ.png");
    loadTexture("QH", folderPath + "cardHeartsQ.png");
    loadTexture("KH", folderPath + "cardHeartsK.png");
    loadTexture("AH", folderPath + "cardHeartsA.png");

    loadTexture("2C", folderPath + "cardClubs2.png");
    loadTexture("3C", folderPath + "cardClubs3.png");
    loadTexture("4C", folderPath + "cardClubs4.png");
    loadTexture("5C", folderPath + "cardClubs5.png");
    loadTexture("6C", folderPath + "cardClubs6.png");
    loadTexture("7C", folderPath + "cardClubs7.png");
    loadTexture("8C", folderPath + "cardClubs8.png");
    loadTexture("9C", folderPath + "cardClubs9.png");
    loadTexture("10C", folderPath + "cardClubs10.png");
    loadTexture("JC", folderPath + "cardClubsJ.png");
    loadTexture("QC", folderPath + "cardClubsQ.png");
    loadTexture("KC", folderPath + "cardClubsK.png");
    loadTexture("AC", folderPath + "cardClubsA.png");

    loadTexture("2S", folderPath + "cardSpades2.png");
    loadTexture("3S", folderPath + "cardSpades3.png");
    loadTexture("4S", folderPath + "cardSpades4.png");
    loadTexture("5S", folderPath + "cardSpades5.png");
    loadTexture("6S", folderPath + "cardSpades6.png");
    loadTexture("7S", folderPath + "cardSpades7.png");
    loadTexture("8S", folderPath + "cardSpades8.png");
    loadTexture("9S", folderPath + "cardSpades9.png");
    loadTexture("10S", folderPath + "cardSpades10.png");
    loadTexture("JS", folderPath + "cardSpadesJ.png");
    loadTexture("QS", folderPath + "cardSpadesQ.png");
    loadTexture("KS", folderPath + "cardSpadesK.png");
    loadTexture("AS", folderPath + "cardSpadesA.png");

    // Don't forget to check what the back of the card is named in your new asset pack!
    // You might need to change "card_back_blue.png" to match the new file.
    loadTexture("cardBack", folderPath + "cardBack_blue1.png");
  }

  void loadAllChipTextures(const std::string& folderPath)
  {
    loadTexture("chips", folderPath + "Chips.png"); // one spritesheet
  }

};
