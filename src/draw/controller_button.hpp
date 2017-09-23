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


#include "../data/key_data.hpp"
#include <SFML/Graphics.hpp>
#include <nonstd/optional.hpp>


struct colour_theme;

// TODO: Chuck various different data into a variant (the class is just under 2.5kB as is, can be reduced to the minimal 1kB with a union)
class controller_button : public sf::Drawable, public sf::Transformable {
private:
	enum class variant_t : std::uint8_t { xyab, stick, control, bumper };

	static variant_t variant_from_button(xbox_controller_button butt);


	const controller_button_data_t * data;
	const colour_theme * theme;
	variant_t variant;

	std::size_t controller;
	sf::Text label;

	sf::CircleShape circle;

	sf::Sprite bumper;
	sf::Sprite bumper_border;

	sf::Sprite control;
	sf::Sprite control_border;
	sf::Sprite control_triangle;


public:
	controller_button(std::size_t controller_num, const std::string & button_id);
	controller_button(std::size_t controller_num, const std::string & button_id, const colour_theme & theme);

	void tick();

	sf::FloatRect global_bounds() const;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
};
