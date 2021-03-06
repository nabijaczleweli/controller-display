// The MIT License (MIT)

// Copyright (c) 2017 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once


#include "../draw/controller_analog.hpp"
#include "../draw/controller_button.hpp"
#include "../draw/controller_dpad.hpp"
#include "../draw/controller_trigger.hpp"
#include "../draw/key.hpp"
#include "../draw/mouse_button.hpp"
#include "../draw/mouse_displacement.hpp"
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <istream>
#include <memory>
#include <nonstd/variant.hpp>


struct colour_theme {
	static const colour_theme classic;  // Which is a pretentious way of saying "default" which is a keyword
	static const colour_theme dark;


	std::size_t character_size;
	sf::Color label;
	sf::Color unclicked;
	sf::Color clicked;
	sf::Color outline;

	sf::Color xbox_A_button_unclicked;
	sf::Color xbox_B_button_unclicked;
	sf::Color xbox_X_button_unclicked;
	sf::Color xbox_Y_button_unclicked;
	sf::Color xbox_A_button_clicked;
	sf::Color xbox_B_button_clicked;
	sf::Color xbox_X_button_clicked;
	sf::Color xbox_Y_button_clicked;
};

class layout : public sf::Drawable {
private:
	colour_theme theme;
	std::vector<key> keyboard_keys;
	std::vector<mouse_button> mouse_buttons;
	std::vector<mouse_displacement> mouse_displacements;
	std::vector<controller_button> controller_buttons;
	std::vector<controller_dpad> controller_dpads;
	std::vector<controller_analog> controller_analogs;
	std::vector<controller_trigger> controller_triggers;
	std::size_t grid_width;
	std::size_t grid_height;

	layout() = default;


public:
	static nonstd::variant<std::unique_ptr<layout>, std::string> load_stream(std::istream & strim);

	sf::Vector2u size();

	void tick();
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
};
