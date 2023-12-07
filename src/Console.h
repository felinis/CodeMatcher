#pragma once

enum class Color
{
	White,
	Red,
	Yellow,
	Green,
};

namespace Console
{
	void Initialise();

	void SetColor(Color color);
	void ResetColor();
};
