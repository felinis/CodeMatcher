#include "Console.h"
#define WIN32_LEAN_AND_MEAN
#define NOUSER
#define NOGDI
#include <Windows.h>

static HANDLE s_console_handle;
namespace Console
{
	void Initialise()
	{
		s_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void SetColor(Color color)
	{
		switch (color)
		{
		case Color::White:
			SetConsoleTextAttribute(s_console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case Color::Red:
			SetConsoleTextAttribute(s_console_handle, FOREGROUND_RED);
			break;
		case Color::Orange:
			SetConsoleTextAttribute(s_console_handle, FOREGROUND_RED | FOREGROUND_GREEN);
			break;
		case Color::Green:
			SetConsoleTextAttribute(s_console_handle, FOREGROUND_GREEN);
			break;
		}
	}

	void ResetColor()
	{
		SetColor(Color::White);
	}
}
