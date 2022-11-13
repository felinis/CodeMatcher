#pragma once

enum class Color
{
	White,
	Red,
	Orange,
	Green,
};

namespace Console
{
	void Initialise();

	void SetColor(Color color);
	void ResetColor();
};
