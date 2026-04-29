#pragma once

#include <map>
#include <vector>
#include "ChipColors.h"

class ChipStack
{
private:
  int m_totalValue;
  std::map<ChipColor, int> m_chipCount;

public:
  // 1. Returns the map directly instead of converting to a vector
  std::map<ChipColor, int> getChipCounts() const
  {
    return m_chipCount;
  }

  // 2. Get the chip count of a single color (optimized slightly)
  int getChipCount(ChipColor color) const
  {
    auto it = m_chipCount.find(color);
    if (it != m_chipCount.end())
    {
      return it->second;
    }
    return 0; // Safely returns 0 if the color isn't in the map yet
  }

  // 3. Replaces the old vector setter with a much safer map setter
  void setChips(const std::map<ChipColor, int>& chips)
  {
    m_chipCount = chips;
  }

  void setTotalValue(int totalValue)
  {
    m_totalValue = totalValue;
    distributeChipsEvenly();
  }

  // 4. Your custom distribution logic, unchanged!
  void distributeChipsEvenly()
  {
    // Clear out the old chips
    m_chipCount.clear(); 

    int remainingValue = m_totalValue;

    // Define our chips and values in strictly DESCENDING order
    std::vector<std::pair<ChipColor, int>> chipDefs = {
      {ChipColor::Blue, 100},
      {ChipColor::Yellow, 25},
      {ChipColor::Red, 5},
      {ChipColor::LightBlue, 1}
    };

    // Loop through, trying to buy "sets" of chips
    for (size_t i = 0; i < chipDefs.size(); ++i)
    {
      // Calculate the cost of a "set" from this tier downwards
      int setCost = 0;
      for (size_t j = i; j < chipDefs.size(); ++j)
      {
        setCost += chipDefs[j].second;
      }

      // Figure out how many full sets we can afford
      int numSets = remainingValue / setCost;

      if (numSets > 0)
      {
        // Give the user this many of EVERY chip in the current set
        for (size_t j = i; j < chipDefs.size(); ++j)
        {
          m_chipCount[chipDefs[j].first] += numSets;
        }

        // Deduct the cost of all those sets from our remaining value
        remainingValue -= (numSets * setCost);
      }
    }
  }
};
