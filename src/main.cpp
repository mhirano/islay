#include <islay/Application.h>

#if __APPLE__ || __LINUX__
int main(int, char**)
#elif __WIN32__
int WinMain(int, char**)
#endif
{
  Application app;

  return app.run();
}
