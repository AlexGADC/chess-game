#pragma once

#include "Button.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

enum class EMenuAction
{
	None,
	Play,
	PlayBullet,
	PlayBlitz,
	PlayRapid,
	Settings,
	ToggleFullscreen,
	Back,
	Exit
};

struct MenuEntry
{
	std::string label;
	EMenuAction action;
};

// A screen made of a title and a vertical list of buttons, centered in `layoutSize`.
// Used for the main menu, the time control screen and the settings screen.
// The background behind it is drawn by the caller.
class MenuScreen
{
public:
	MenuScreen(const sf::Font& font, std::string title, const std::vector<MenuEntry>& entries, sf::Vector2u layoutSize);

	void UpdateHover(sf::Vector2i mousePosition);

	// Returns EMenuAction::None when the click did not hit any button.
	EMenuAction GetClickedAction(sf::Vector2i pixel) const;

	void Draw(sf::RenderTarget& target) const;

private:
	struct MenuItem
	{
		Button button;
		EMenuAction action;
	};

	const sf::Font& m_font;
	std::string m_title;
	sf::Vector2f m_titleCenter;
	std::vector<MenuItem> m_items;
};