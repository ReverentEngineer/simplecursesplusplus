#include "simplecurses++.hh"

using namespace SimpleCurses;

int main() {
  try {
    Screen screen;
    Window &child =
        screen.add("test", std::make_unique<Window>(3, 3, 20, 20, true));
    child.add("field", std::make_unique<Text>(0, 0, "window"));
    Window &subchild =
        child.add("test2", std::make_unique<Window>(4, 4, 10, 10, true));
    subchild.add("field", std::make_unique<Text>(0, 0, "sub"));
    subchild.update();
    child.update();
    screen.update();
    getchar();
    subchild.remove("field");
    subchild.update();
    child.update();
    screen.update();
    getchar();
  } catch (const CursesException &ex) {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}
