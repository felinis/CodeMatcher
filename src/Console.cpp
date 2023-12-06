#include "Console.h"
#include <stdio.h>
#ifdef WINDOWS
#include <windows.h>
#endif

namespace Console
{
    void Initialise()
    {
#ifdef WINDOWS
        // Set console to support ANSI escape codes
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#endif
    }

    void SetColor(Color color)
    {
        switch (color)
        {
        case Color::White:
            printf("\033[0m");
            break;
        case Color::Red:
            printf("\033[0;31m");
            break;
        case Color::Orange:
            printf("\033[0;33m");
            break;
        case Color::Green:
            printf("\033[0;32m");
            break;
        }
    }

    void ResetColor()
    {
        SetColor(Color::White);
    }
}
