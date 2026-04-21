#pragma once

#include <iostream>
#include <string>

// --- GLOBAL DEBUG SETTINGS ---
#define DEBUGMODE 1
#define TRACEMODE 0

// --- SHARED THREAD-SAFE INDENTATION ---
#if DEBUGMODE || TRACEMODE
  // 'inline' prevents duplicate symbol errors when including this in multiple .cpp files
  inline int& getDebugDepth() {
      thread_local int depth = 0; 
      return depth;
  }

  inline std::string getDebugIndent() {
      // Returns a string filled with 2 spaces per depth level
      return std::string(getDebugDepth() * 2, ' ');
  }
#endif

// --- STANDARD DEBUG PRINTING ---
#if DEBUGMODE
  // Prepends the indent string automatically before your custom message
  #define DEBUG_PRINT std::cout << getDebugIndent()
  
  // Prepends the indent for C-style printf
  #define DEBUG_PRINTF(fmt, ...) printf("%s" fmt, getDebugIndent().c_str(), ##__VA_ARGS__)
#else
  #define DEBUG_PRINT if (false) std::cout 
  #define DEBUG_PRINTF(...) do {} while(0)
#endif

// --- CALL STACK TRACING ---
#if TRACEMODE
  class FunctionTracer {
  private:
      const char* m_funcName;

  public:
      FunctionTracer(const char* funcName) : m_funcName(funcName) {
          std::cout << getDebugIndent() << "-> " << m_funcName << "\n";
          getDebugDepth()++; 
      }
      
      ~FunctionTracer() {
          getDebugDepth()--; 
          std::cout << getDebugIndent() << "<- " << m_funcName << "\n";
      }
  };

  #define TRACE_FUNCTION() FunctionTracer _tracer_(__func__)
#else
  #define TRACE_FUNCTION() do {} while(0)
#endif
