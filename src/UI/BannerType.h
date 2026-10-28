#pragma once
#include <SFML/Graphics.hpp>

class BannerType
{
public:
    sf::IntRect cropRect;

    // Constructor
    BannerType(sf::IntRect rect) : cropRect(rect) {}

    // C++17 inline static variables (keeps it header-only without linker errors)
    inline static const BannerType Banner1      = BannerType(sf::IntRect({0, 100}, {96, 26}));
    inline static const BannerType Banner2      = BannerType(sf::IntRect({100, 100}, {88, 28}));
    inline static const BannerType Banner3      = BannerType(sf::IntRect({261, 210}, {88, 44}));
    inline static const BannerType Banner4      = BannerType(sf::IntRect({417, 129}, {90, 44}));
    inline static const BannerType WhiteBanner1 = BannerType(sf::IntRect({272, 400}, {48, 16}));
    inline static const BannerType WhiteBanner2 = BannerType(sf::IntRect({336, 400}, {32, 16}));
};
