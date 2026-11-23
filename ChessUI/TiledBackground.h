#pragma once

#include <SFML/Graphics.hpp>

// The dirt-like tile pattern drawn behind every screen (Minecraft style).
class TiledBackground
{
public:
	TiledBackground();

	// Rebuilds the tiles so they fill `area`, given in the same coordinates the screens are drawn in.
	void Cover(sf::FloatRect area);

	void Draw(sf::RenderTarget& target) const;

private:
	sf::VertexArray m_tiles;
};
