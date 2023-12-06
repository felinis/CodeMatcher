#pragma once

enum class Color : int
{
	White,
	Red,
	Orange,
	Green,
};

namespace Console
{
	void Initialise();
	void Shutdown();

	void SetColor(Color color);
	void ResetColor();
};
