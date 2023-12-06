#include "Console.h"
#include <ncurses.h>

namespace Console
{
    void Initialise()
    {
        initscr(); // Initialise curses library
        start_color(); // Start color functionality

		// Initialize color pairs
        init_pair(static_cast<int>(Color::White), COLOR_WHITE, COLOR_BLACK);
        init_pair(static_cast<int>(Color::Red), COLOR_RED, COLOR_BLACK);
        init_pair(static_cast<int>(Color::Orange), COLOR_YELLOW, COLOR_BLACK); // NCurses doesn't have orange, closest is yellow
        init_pair(static_cast<int>(Color::Green), COLOR_GREEN, COLOR_BLACK);
    }

	void Shutdown()
	{
		endwin(); // End curses mode
	}

    void SetColor(Color color)
    {
        attron(COLOR_PAIR(static_cast<int>(color)));
    }

    void ResetColor()
    {
        SetColor(Color::White);
    }
}
