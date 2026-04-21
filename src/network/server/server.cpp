#include "ServerApplication.h"

#include "../../core/Debug.h"

int main() 
{
  ServerApplication app("8080"); 
  app.run(); 
  return 0;
}
