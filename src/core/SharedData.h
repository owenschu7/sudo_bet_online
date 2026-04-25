#pragma once
#include <string>
#include <queue>
#include "GameEvents.h"

struct SharedData
{
  bool s_settingsChanged = false; //flag to update screen when changes occur (this should turn true when any change has been made, and false when we update the changes)
  
  bool s_isOnline = false;
  bool s_isLoggedIn = false;

  float s_musicVolume = 0.5f;
  float s_sfxVolume = 0.3f;
  bool s_videoQuality = 0; // 0 for low 1 for high
  bool s_videoHigh = false; // FIX: remove these later
  bool s_videoLow = true;
  int s_currentLanguage = 0;

  bool s_lightMode = false; // true for lightmode false for dark mode

  bool s_needUsername;
  std::string s_currentUUID = "";
  std::string s_currentUsername = "";

  bool s_demoWindow = false; // displays the demo window for debugging
  bool s_debugMode = false; // displays additional debug info in the UI

  //GAME EVENTS
  //when an event occurs such as login, join table, or ready up the screen creates a GameEvent object
  // then pushes that GameEvent object onto the m_sharedData.s_outboundEvents queue which will

  // OUTBOX: Screens put events here. The clientApplication reads them and sends them to the server.
  std::queue<GameEvent> s_outboundEvents; 

  // INBOX: clientApplication receives packets, turns them into events, and puts them here for screens to read.
  std::queue<GameEvent> s_inboundEvents;
};


