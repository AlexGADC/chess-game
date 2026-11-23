#include "TiledBackground.h"

#include <cmath>
#include <cstdint>
#include <initializer_list>

namespace
{
	constexpr float TILE_SIZE = 32.f;

	constexpr unsigned int NOISE_COLUMN_FACTOR = 73856093u;
	constexpr unsigned int NOISE_ROW_FACTOR = 19349663u;
	constexpr unsigned int NOISE_RANGE = 24u;

	constexpr unsigned int BASE_RED = 60;
	constexpr unsigned int BASE_GREEN = 42;
	constexpr unsigned int BASE_BLUE = 30;

	// Deterministic pseudo-random brightness, so a tile looks the same on every run and in every window size.
	sf::Color GetTileColor(int col, int row)
	{
		const unsigned int noise =
			((static_cast<unsigned int>(col) * NOISE_COLUMN_FACTOR) ^ (static_cast<unsigned int>(row) * NOISE_ROW_FACTOR))
			% NOISE_RANGE;
		const auto channel = [noise](unsigned int base) { return static_cast<std::uint8_t>(base + noise); };

		return sf::Color(channel(BASE_RED), channel(BASE_GREEN), channel(BASE_BLUE));
	}

	void AppendTile(sf::VertexArray& vertices, sf::Vector2f position, sf::Color color)
	{
		const sf::Vector2f topLeft = position;
		const sf::Vector2f topRight = position + sf::Vector2f(TILE_SIZE, 0.f);
		const sf::Vector2f bottomLeft = position + sf::Vector2f(0.f, TILE_SIZE);
		const sf::Vector2f bottomRight = position + sf::Vector2f(TILE_SIZE, TILE_SIZE);

		for (const sf::Vector2f& corner : { topLeft, topRight, bottomLeft, topRight, bottomRight, bottomLeft })
		{
			vertices.append(sf::Vertex{ corner, color });
		}
	}

	int GetTileIndex(float coordinate)
	{
		return static_cast<int>(std::floor(coordinate / TILE_SIZE));
	}
}

TiledBackground::TiledBackground()
	: m_tiles(sf::PrimitiveType::Triangles)
{
}

void TiledBackground::Cover(sf::FloatRect area)
{
	m_tiles.clear();

	// The area can start at negative coordinates, so the tile indices can be negative too.
	const int firstCol = GetTileIndex(area.position.x);
	const int lastCol = GetTileIndex(area.position.x + area.size.x);
	const int firstRow = GetTileIndex(area.position.y);
	const int lastRow = GetTileIndex(area.position.y + area.size.y);

	for (int row = firstRow; row <= lastRow; ++row)
	{
		for (int col = firstCol; col <= lastCol; ++col)
		{
			AppendTile(m_tiles, { col * TILE_SIZE, row * TILE_SIZE }, GetTileColor(col, row));
		}
	}
}

void TiledBackground::Draw(sf::RenderTarget& target) const
{
	target.draw(m_tiles);
}
