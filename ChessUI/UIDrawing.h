#pragma once

#include <SFML/Graphics.hpp>

#include <string>

// Small drawing helpers shared by the menu widgets.

void DrawRectangle(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size, sf::Color color);

// Draws text centered on `center`, with a dark drop shadow (Minecraft style).
void DrawShadowedText(
	sf::RenderTarget& target,
	const sf::Font& font,
	const std::string& content,
	unsigned int characterSize,
	sf::Vector2f center,
	sf::Color fillColor);