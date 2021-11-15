#include "MovieBrowser.h"

int main() {
  MovieBrowser movieBrowser;
  std::cout << movieBrowser.scanDirectory("/media/lucas/BAT-external disk/video/films/", 0) << std::endl;
  movieBrowser.Run();

  return 0;
}
