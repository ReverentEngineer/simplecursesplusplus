#include "simplecurses++.hh"

using namespace SimpleCurses;

int main() {
  try {
    Screen screen;
    Window &child = screen.createWindow("test", 3, 3, 20, 20, 1);
	child.addText("field", 0, 0, "window");
    Window &subchild = child.createWindow("test2", 4, 4, 10, 10, 1);
	subchild.addText("field", 0, 0, "sub");
    screen.update();
    getchar();
	subchild.removeText("field");
    screen.update();
    getchar();
  } catch (const CursesException &ex) {
    std::cerr << ex.what() << std::endl;
  }


  return 0;
}
