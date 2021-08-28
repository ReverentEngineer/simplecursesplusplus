/**
 * @file simplecurses++.hh
 * @brief An opinionated, simple C++ wrapper around curses
 *
 * This is a header-only library. All one needs to do is to include the
 * header in the code:
 *
 * @code
 * #include <simplecurses++.hh>
 * @endcode
 */
#ifndef SIMPLECURSESPP_HH
#define SIMPLECURSESPP_HH

#include <curses.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

/**
 * @example example.cpp
 *
 * A brief example of how to use SimpleCurses++
 */

/**
 * @brief The namespace for SimpleCurses++
 */
namespace SimpleCurses {

/**
 * @brief An a SimpleCurses exception
 */
class CursesException : public std::exception {
public:
  CursesException(const std::string &message) : m_message(message) {}

  const char *what() const throw() { return m_message.c_str(); }

private:
  std::string m_message;
};

/**
 * @brief Represents an element in the interface
 */
class Element {
public:
  /**
   * @brief Constructor
   *
   * @param x The x coordinate relative to its container
   * @param y The y coordinate relative to its container
   */
  Element(int x, int y) : m_x(x), m_y(y) {}

  /**
   * @brief Destructor
   */
  virtual ~Element() = default;

  /** @brief The x coordinate of the element **/
  int x() const { return m_x; }

  /** @brief The y coordinate of the element **/
  int y() const { return m_y; }

  /** @brief The number of rows occupied by the element **/
  virtual int rows() const = 0;

  /** @brief The number of cols occupied by the element **/
  virtual int cols() const = 0;

  /** @brief Draw element onto window.
   *
   * @param The parent window.
   */
  virtual void draw(WINDOW *parent) = 0;

private:
  /** @brief The x coordinate **/
  int m_x;

  /** @brief The y coordinate **/
  int m_y;
};

class Text : public Element {
public:
  Text(int x, int y, const std::string &text) : Element(x, y), m_text(text) {}

  virtual ~Text() = default;

  /**
   * @brief Gets the text of the element
   */
  const std::string &text() const { return m_text; }

  /**
   * @copydoc Element::rows()
   */
  int rows() const { return 1; }

  /**
   * @copydoc Element::cols()
   */
  int cols() const { return m_text.size(); }

  void draw(WINDOW *parent) { mvwaddstr(parent, y(), x(), m_text.c_str()); }

private:
  std::string m_text;
};

/**
 * @brief A window in the interface
 */
class Window : public Element {
public:
  Window(int x, int y, int rows, int cols, bool border)
      : Element(x, y), m_rows(rows), m_cols(cols), m_border(border),
        m_window(NULL) {
    if (m_border) {
      m_rows--;
      m_cols--;
    }
  }

  /**
   * @brief Destructor
   */
  virtual ~Window() { delwin(m_window); }

  /**
   * @brief Updates the screen with the changes in the window
   */
  void update() {
    // std::for_each(m_subwindows.begin(), m_subwindows.end(), [&](WindowPtr&
    // window) { 		window->update();
    //});
    touchwin(m_window);
    wrefresh(m_window);
  }

  /**
   * @copydoc Element::rows()
   */
  int rows() const { return m_rows; }

  /**
   * @copydoc Element::cols()
   */
  int cols() const { return m_cols; }

  void draw(WINDOW *parent) {
    if (!m_window) {
      m_window = derwin(parent, m_cols, m_rows, y(), x());
    }
    if (m_border) {
      box(m_window, 0, 0);
    }
  }

  template <typename WindowElement>
  WindowElement &add(const std::string &name,
                     std::unique_ptr<WindowElement> element) {
    if (!element) {
      throw CursesException("No element provided.");
    }

    if ((element->x() + element->cols() > m_cols) ||
        (element->y() + element->rows() > m_rows)) {
      throw CursesException("Element doesn't fit in window");
    }

    WindowElement &ref = *element.get();
    ref.draw(m_window);
    m_elements[name] = std::move(element);
    return ref;
  }

  /**
   * @brief Remove an element in the window
   *
   * @param name The name of the element to remove.
   */
  void remove(const std::string &name) {
    const ElementPtr &element = m_elements.at(name);
    for (size_t i = element->y(); i < element->rows(); i++) {
      wmove(this->m_window, i, element->x());
      for (int j = 0; j < element->cols(); j++) {
        waddch(this->m_window, ' ');
      }
    }
    if (m_border) {
      if (element->x() == 0 || element->y() == 0 ||
          (element->x() + element->cols()) == (m_cols - 1) ||
          (element->y() + element->rows()) == (m_rows - 1)) {
        draw(NULL); // Parent not necessary, just redrawing border.
      }
    }
    m_elements.erase(name);
  }

protected:
  /**
   * @brief Protected constructor
   *
   * This exists solely to allow Screen to be a special type of Window.
   */
  Window(int x, int y, WINDOW *window) : Element(x, y), m_window(window) {
    getmaxyx(window, m_cols, m_rows);
  }

private:
  typedef std::unique_ptr<Window> WindowPtr;
  typedef std::unique_ptr<Element> ElementPtr;

  WINDOW *m_window;

  bool m_border;

  int m_rows;

  int m_cols;

  std::map<std::string, ElementPtr> m_elements;
};

/**
 * @brief Represents a curses screen
 */
class Screen : public Window {
public:
  Screen() : Window(0, 0, initscr()) {}

  virtual ~Screen() { endwin(); }

  virtual void draw(WINDOW *window) {
    throw CursesException("Screen should never be drawn.");
  }
};

} // namespace SimpleCurses

#endif /* SIMPLECURSESPP_HH */
