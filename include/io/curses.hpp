#ifndef _CURSES_HPP_
#define _CURSES_HPP_
#include <iostream>
#include <algorithm>
#include <ncurses.h>

#define GAME_WINDOW_WIDTH 100
#define GAME_WINDOW_HEIGHT 40
#define GAME_WINDOW_STARTX 2
#define GAME_WINDOW_STARTY 1

#define CONSOLE_WINDOW_WIDTH 100
#define CONSOLE_WINDOW_HEIGHT 10
#define CONSOLE_WINDOW_STARTX 2
#define CONSOLE_WINDOW_STARTY 42



namespace curses {
    class streambuf : public std::streambuf {
        private:
        WINDOW *window;
        std::vector<char> buf;
        const int bufsize;
        char *base;

        public:
        streambuf(WINDOW *w = nullptr, int bufsize = 128) :
            window(w), buf(std::max(bufsize, 1) + 2),
            bufsize(bufsize), base(&buf.front())
        {
            setp(base, base + bufsize);
        }

        int_type overflow(int_type ch) {
            *pptr() = ch;
            pbump(1);
            sync();
            return ch;
        }

        int sync() {
            *pptr() = '\0';
            wprintw(window, "%s", pbase());
            wrefresh(window);
            setp(base, base + bufsize);
            return 0;
        }
        void set_window(WINDOW *w) {
            window = w;
        }
    };

    void simple_start();
    void simple_stop();

    extern WINDOW *game_window;
    extern WINDOW *console_window;
    extern std::ostream cout;
    extern std::ostream &(&endl)(std::ostream &os);
};
#endif
