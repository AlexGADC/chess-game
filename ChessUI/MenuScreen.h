#pragma once

#include "Button.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

enum class EMenuAction
{
	None,
	Play,
	Settings,
	Back
};

struct MenuEntry
{
	std::string label;
	EMenuAction action;
};

// A screen made of a title and a vertical list of buttons, on a dirt-like background.
// Used for both the main menu and the settings screen.
class MenuScreen
{
public:
	MenuScreen(const sf::Font& font, std::string title, const std::vector<MenuEntry>& entries, sf::Vector2u windowSize);

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

	void BuildBackground(sf::Vector2u windowSize);

	const sf::Font& m_font;
	std::string m_title;
	sf::Vector2f m_titleCenter;
	std::vector<MenuItem> m_items;
	sf::VertexArray m_background;
};