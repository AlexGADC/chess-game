#pragma once

#include <SFML/Graphics.hpp>

#include <string>

// A Minecraft-style button: gray bevelled rectangle that lights up on hover.
class Button
{
public:
	Button(const sf::Font& font, std::string label, sf::Vector2f position, sf::Vector2f size);

	void UpdateHover(sf::Vector2i mousePosition);
	bool Contains(sf::Vector2i pixel) const;

	void Draw(sf::RenderTarget& target) const;

private:
	const sf::Font& m_font;
	std::string m_label;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	bool m_isHovered = false;
};