#pragma once

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <string>

inline sf::Font LoadFontOrThrow(const std::string& path)
{
	sf::Font font;
	if (!font.openFromFile(path))
	{
		throw std::runtime_error("Could not load font: " + path);
	}
	return font;
}