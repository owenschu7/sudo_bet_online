#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

enum class BannerType
{
  Banner1,
  Banner2,
  Banner3,
  Banner4,
  WhiteBanner1,
  WhiteBanner2,
};

struct Label
{
  sf::Text textObj;
  bool isCentered  = false;
  bool m_hasBanner = false;

  std::unique_ptr<sf::Sprite> bannerSprite;

  // positions
  sf::Vector2f m_position;
  sf::Vector2f m_textOffset = {0.0f, 0.0f};

  // Constructor: Requires the font immediately (SFML 3 rule!)
  Label(const sf::Font& font) : textObj(font) {}

  // Setup WITHOUT a banner — plain text only
  void setup(const std::string& text, unsigned int characterSize, sf::Color color,
      sf::Vector2f pos, bool centerAlign = true)
  {
    m_hasBanner = false;
    bannerSprite.reset();
    m_textOffset = {0.0f, 0.0f};

    isCentered = centerAlign;
    textObj.setString(text);
    textObj.setCharacterSize(characterSize);
    textObj.setFillColor(color);
    recalculateOrigin();

    setPosition(pos);
  }

  // Setup WITH a banner
  // scaleFactor: 1.0 = original size, 2.0 = double, 10.0 = ten times bigger, etc.
  void setupWithBanner(const std::string& text, unsigned int characterSize, sf::Color color,
      sf::Vector2f pos, bool centerAlign,
      const sf::Texture& tex, BannerType bannerType, float scaleFactor,
      sf::Vector2f textOffset = {0.0f, 0.0f})
  {
    m_hasBanner = true;
    m_textOffset = textOffset;

    // Pick the crop rect based on which banner style you want
    sf::IntRect cropRect;
    switch (bannerType)
    {
      case BannerType::Banner1:
        cropRect = sf::IntRect({0, 100}, {96, 26});
        break;
      case BannerType::Banner2:
        cropRect = sf::IntRect({100, 100}, {88, 28});
        break;
      case BannerType::Banner3:
        cropRect = sf::IntRect({261, 210}, {88, 44});
        break;
      case BannerType::Banner4:
        cropRect = sf::IntRect({417, 129}, {90, 44});
        break;
      case BannerType::WhiteBanner1:
        cropRect = sf::IntRect({272, 400}, {48, 16});
        break;
      case BannerType::WhiteBanner2:
        cropRect = sf::IntRect({336, 400}, {32, 16});
        break;
    }

    // Uniform scale — both axes use the same factor so it never stretches
    bannerSprite = std::make_unique<sf::Sprite>(tex);
    bannerSprite->setTextureRect(cropRect);
    bannerSprite->setScale({scaleFactor, scaleFactor});

    // Center the banner on pos
    bannerSprite->setOrigin({cropRect.size.x / 2.0f, cropRect.size.y / 2.0f});

    // Setup the text on top of the banner
    isCentered = centerAlign;
    textObj.setString(text);
    textObj.setCharacterSize(characterSize);
    textObj.setFillColor(color);
    recalculateOrigin();

    setPosition(pos);
  }

  void setPosition(sf::Vector2f pos)
  {
    m_position = pos;

    // move the banner if it exists
    if (m_hasBanner && bannerSprite)
    {
      bannerSprite->setPosition(m_position);
    }

    // move the text, applying the stored offset
    textObj.setPosition({m_position.x + m_textOffset.x, m_position.y + m_textOffset.y});
  }

  void setPosition(float x, float y)
  {
    setPosition({x, y});
  }

  void setScale(float bannerScale, unsigned int charSize)
  {
    // set the banner if it exists
    if (m_hasBanner && bannerSprite)
    {
      bannerSprite->setScale({bannerScale, bannerScale});
    }

    textObj.setCharacterSize(charSize);
    recalculateOrigin();
  }

  // Easy way to update dynamic text
  void setString(const std::string& newText)
  {
    textObj.setString(newText);
    if (isCentered)
    {
      recalculateOrigin();
    }
  }

  void recalculateOrigin()
  {
    if (isCentered)
    {
      sf::FloatRect bounds = textObj.getLocalBounds();
      textObj.setOrigin({
          bounds.position.x + bounds.size.x / 2.0f,
          bounds.position.y + bounds.size.y / 2.0f
          });
    }
    else
    {
      textObj.setOrigin({0.0f, 0.0f});
    }
  }
  void setColor(sf::Color color)
  {
    bannerSprite->setColor(color);
  }

  void draw(sf::RenderTarget& window)
  {
    if (m_hasBanner && bannerSprite)
    {
      window.draw(*bannerSprite);
    }
    window.draw(textObj);
  }
};
