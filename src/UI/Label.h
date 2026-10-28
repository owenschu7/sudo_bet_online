#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BannerType
{
  Banner1,
  Banner2,
  Banner3,
  Banner4,
  WhiteBanner1,
  WhiteBanner2,
};

class Label
{
private:
  sf::Text m_textObj;
  std::optional<sf::Sprite> m_bannerSprite;

  bool m_isCentered = true;

  // Positions
  sf::Vector2f m_position;
  sf::Vector2f m_textOffset;

  // Helper to keep the text perfectly centered mathematically
  void recalculateOrigin()
  {
    if (m_isCentered)
    {
      sf::FloatRect bounds = m_textObj.getLocalBounds();
      m_textObj.setOrigin({
          bounds.position.x + bounds.size.x / 2.0f,
          bounds.position.y + bounds.size.y / 2.0f
      });
    }
    else
    {
      m_textObj.setOrigin({0.0f, 0.0f});
    }
  }

public:
  // ==========================================
  // CONSTRUCTOR 1: Plain text ONLY (No Banner)
  // ==========================================
  Label(const sf::Font& font, const std::string& text, unsigned int characterSize,
        sf::Color color, sf::Vector2f pos, bool centerAlign = true)
  : m_textObj(font)
  {
    m_isCentered = centerAlign;
    m_textOffset = {0.0f, 0.0f};

    m_textObj.setString(text);
    m_textObj.setCharacterSize(characterSize);
    m_textObj.setFillColor(color);
    
    recalculateOrigin();
    setPosition(pos);
  }

  // ==========================================
  // CONSTRUCTOR 2: Text WITH a Banner
  // ==========================================
  Label(const sf::Font& font, const sf::Texture& tex, BannerType bannerType,
        const std::string& text, unsigned int characterSize, sf::Color color,
        float scaleFactor, sf::Vector2f pos, bool centerAlign = true,
        sf::Vector2f textOffset = {0.0f, 0.0f})
  : m_textObj(font), m_bannerSprite(tex) // Initialize immediately
  {
    m_isCentered = centerAlign;
    m_textOffset = textOffset;

    // 'emplace' creates the sprite directly inside the optional and gives it the texture
    m_bannerSprite.emplace(tex);

    // Pick the crop rect based on which banner style you want
    sf::IntRect cropRect;
    switch (bannerType)
    {
      case BannerType::Banner1: cropRect = sf::IntRect({0, 100}, {96, 26}); break;
      case BannerType::Banner2: cropRect = sf::IntRect({100, 100}, {88, 28}); break;
      case BannerType::Banner3: cropRect = sf::IntRect({261, 210}, {88, 44}); break;
      case BannerType::Banner4: cropRect = sf::IntRect({417, 129}, {90, 44}); break;
      case BannerType::WhiteBanner1: cropRect = sf::IntRect({272, 400}, {48, 16}); break;
      case BannerType::WhiteBanner2: cropRect = sf::IntRect({336, 400}, {32, 16}); break;
    }

    // Configure the banner
    m_bannerSprite->setTextureRect(cropRect);
    m_bannerSprite->setScale({scaleFactor, scaleFactor});

    // Center the banner's origin mathematically using getLocalBounds()
    sf::FloatRect bannerBounds = m_bannerSprite->getLocalBounds();
    m_bannerSprite->setOrigin({
        bannerBounds.position.x + bannerBounds.size.x / 2.0f,
        bannerBounds.position.y + bannerBounds.size.y / 2.0f
    });

    // Configure the text
    m_textObj.setString(text);
    m_textObj.setCharacterSize(characterSize);
    m_textObj.setFillColor(color);
    
    recalculateOrigin();
    setPosition(pos); // Sets both banner and text position
  }

  // ==========================================
  // PUBLIC METHODS
  // ==========================================
  void setPosition(sf::Vector2f pos)
  {
    m_position = pos;

    if (m_bannerSprite)
    {
      m_bannerSprite->setPosition(m_position);
    }

    m_textObj.setPosition({m_position.x + m_textOffset.x, m_position.y + m_textOffset.y});
  }

  void setPosition(float x, float y)
  {
    setPosition({x, y});
  }

  void setScale(float bannerScale, unsigned int charSize)
  {
    if (m_bannerSprite)
    {
      m_bannerSprite->setScale({bannerScale, bannerScale});
    }

    m_textObj.setCharacterSize(charSize);
    recalculateOrigin();
  }

  // Easy way to update dynamic text
  void setString(const std::string& newText)
  {
    m_textObj.setString(newText);
    if (m_isCentered)
    {
      recalculateOrigin();
    }
  }

  void setColor(sf::Color color)
  {
    if (m_bannerSprite)
    {
      m_bannerSprite->setColor(color);
    }
  }

  void setTextColor(sf::Color color)
  {
    m_textObj.setFillColor(color);
  }

  // Render method is marked 'const' as it doesn't modify data
  void draw(sf::RenderTarget& window) const
  {
    if (m_bannerSprite)
    {
      window.draw(*m_bannerSprite);
    }
    window.draw(m_textObj);
  }
};
