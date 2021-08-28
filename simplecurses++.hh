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
#include <memory>
#include <string>
#include <utility>
#include <vector>

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
 * @brief Represents a named element in the user interfaces
 */
class Element {
public:
  
  /**
   * @brief Constructor
   *
   * @param name The name of the element
   * @param x The x coordinate relative to its container
   * @param y The y coordinate relative to its container
   */
  Element(const std::string &name, int x, int y)
      : m_name(name), m_x(x), m_y(y) {}

  /**
   * @brief Destructor
   */
  virtual ~Element() = default;

  /** @brief The name of the element **/
  const std::string &name() const { return m_name; }

  /** @brief The x coordinate of the element **/
  int x() const { return m_x; }

  /** @brief The y coordinate of the element **/
  int y() const { return m_y; }

  /** @brief The number of rows occupied by the element **/
  virtual int rows() const = 0;

  /** @brief The number of cols occupied by the element **/
  virtual int cols() const = 0;

private:

  /** @brief Name of element **/
  std::string m_name;

  /** @brief The x coordinate **/
  int m_x;

  /** @brief The y coordinate **/
  int m_y;
};


/**
 * @brief A window in the interface
 */
class Window : public Element {
public:

  /** 
   * @brief Destructor
   */
  virtual ~Window() {
	delwin(m_window);
  }

  /**
   * @brief Updates the screen with the changes in the window
   */
  void update() { 
	  std::for_each(m_subwindows.begin(), m_subwindows.end(), [&](WindowPtr& window) {
	  		window->update();
	  });
	  wsyncup(m_window);
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

  /**
   * @brief Create a child Window inside the (parent) Window
   *
   * @param name The name of the window
   * @param relativex The x coordinate relative to the parent
   * @param relativey The y coordinate relative to the parent
   */
  Window &createWindow(const std::string &name, int relativex, int relativey,
                       int rows, int cols, bool border = false) {
    if (((relativex + rows) > m_rows) || ((relativey + cols) > m_cols)) {
      throw CursesException("Subwindow too large.");
    }

    WINDOW *win = derwin(m_window, cols, rows, relativey, relativex);
    if (border) {
      box(win, 0, 0);
    }
    m_subwindows.push_back(
        WindowPtr(new Window(name, relativex, relativey, border, win)));
    return *(m_subwindows.back());
  }

  /**
   * @brief Add text to the Window
   *
   * @param name The name of text field
   * @param relativex The x coordinate relative to the Window
   * @param relativey The y coordinate relative to the Window
   * @param text The text to insert
   */
  void addText(const std::string &name, int relativex, int relativey,
               const std::string &text) {

	if (m_border) {
		relativex++;
		relativey++;
	}
    
	if (((relativex + 1) > m_cols) || ((relativey + text.size()) > m_rows)) {
      	throw CursesException("Text doesn't fit in window.");
    }

    m_texts.push_back(TextPtr(new Text(name, relativex, relativey, text)));
    mvwaddstr(m_window, relativey, relativex, text.c_str());
  }

  /**
   * @brief Remove a text element in the window
   *
   * @param name The name of the text element remove.
   */
  void removeText(const std::string &name) {
	  std::vector<TextPtr>::iterator new_end = 
		  std::remove_if(m_texts.begin(), m_texts.end(), [this, &name](const TextPtr& text) {
			wmove(this->m_window, text->y(), text->x());
			for (int i = 0; i < text->text().size(); i++) {
				waddch(this->m_window, ' ');
			}

			return text->name() == name;
	 	});
  }
  

protected:
  Window(const std::string &name, int x, int y, bool border, WINDOW *window)
      : Element(name, x, y), m_border(border), m_window(window) {
    getmaxyx(m_window, m_rows, m_cols);
	if (m_border) {
		m_rows--;
		m_cols--;
	}
  }

private:
  class Text : public Element {
  public:
    Text(const std::string &name, int x, int y, const std::string &text)
        : Element(name, x, y), m_text(text) {}

    virtual ~Text() = default;

	/**
	 * @brief Gets the text of the element
	 */
	const std::string& text() const { return m_text; }

    /**
     * @copydoc Element::rows()
     */
    int rows() const { return 1; }

    /**
     * @copydoc Element::cols()
     */
    int cols() const { return m_text.size(); }

  private:
    std::string m_text;
  };

  typedef std::unique_ptr<Window> WindowPtr;
  typedef std::unique_ptr<Text> TextPtr;

  WINDOW *m_window;

  bool m_border;

  int m_rows;

  int m_cols;

  std::vector<WindowPtr> m_subwindows;

  std::vector<TextPtr> m_texts;
};

/**
 * @brief Represents a curses screen
 */
class Screen : public Window {
public:
  Screen() : Window("stdscr", 0, 0, false, initscr()) {}

  virtual ~Screen() { endwin(); }
};

}

#endif /* SIMPLECURSESPP_HH */
