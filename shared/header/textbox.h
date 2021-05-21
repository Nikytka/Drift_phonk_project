#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <sstream>

#include "debug.h"

#define DELETE_KEY 8
#define ENTER_KEY 13 
#define ESCAPE_KEY 27

class Textbox
{
	sf::Text textbox;
	std::ostringstream text;
	bool isSelected = false; // Is the typebox currently selected
	bool hasLimit = false; // If the typebox has limit
	int limit = 0; // Typebox limit

	void inputLogic(int charTyped); // Adding new symbols to the typebox
	void deleteLastChar(); // Deleting symbols from the typebox

public:
	Textbox(); // Default constructor
	Textbox(int size, sf::Color color, bool sel); // More precise constructor

	void setFont(sf::Font &font); // Set typebox font

	void setColor(sf::Color color); // Set typebox color

	void setOrigin(sf::Vector2f pos); // Set textbox origin

	void setPosition(sf::Vector2f pos); // Set typebox position

	sf::FloatRect getGlobalBounds(); // Get textbox global bounds

	void setLimit(bool ToF); // Set if the typebox has limit
	void setLimit(bool ToF, int lim); // Set if the typebox has limit and the limit itself

	void setSelected(bool sel); // Set if the typebox is selected

	std::string getText(); // Get typebox string

	void setText(std::string str); // Set text to textbox (Only to visual part!)

	void drawTo(sf::RenderWindow& window); // Draw typebox to the window

	sf::Text getDrawable();

	void typedOn(sf::Event input); // Input to typebox
};