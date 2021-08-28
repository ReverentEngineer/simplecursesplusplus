#include "simplecurses++.hh"

using namespace SimpleCurses;

int main() {
  try {
    Screen screen;
    screen.add("field", std::make_unique<Text>(10, 10, "My awesome app"));
    screen.update();
    getchar();
  } catch (const CursesException &ex) {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}
