#pragma once

#include <SFML/Graphics.hpp>

#include <string>

// A horizontal bar for one player: the name on the left and the clock on the right.
// It only displays the time it is given; it does not keep time itself.
class PlayerBarView
{
public:
	PlayerBarView(const sf::Font& font, std::string playerName, sf::Vector2f position, sf::Vector2f size);

	// `isActive` highlights the clock of the player who is to move.
	void Draw(sf::RenderTarget& target, int remainingSeconds, bool isActive) const;

private:
	void DrawPlayerName(sf::RenderTarget& target) const;
	void DrawClock(sf::RenderTarget& target, int remainingSeconds, bool isActive) const;

	const sf::Font& m_font;
	std::string m_playerName;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
};
