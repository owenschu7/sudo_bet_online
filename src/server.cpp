#include "network/server/ServerApplication.h"

int main() 
{
  ServerApplication app("8080"); 
  app.run(); 
  return 0;
}
