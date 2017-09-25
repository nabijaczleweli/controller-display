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


#include "controller_analog.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"


void controller_analog::set_positions(float hor, float ver) {
	const float cell_size      = theme->character_size + theme->character_size / 2;
	const float separator_size = cell_size / 6.f;
	const float xy_size        = cell_size * 2 + separator_size;
	const float free_space     = xy_size / 2 - cell_size / 2;
	const float center         = xy_size / 2;

	const auto rel_pos_x = free_space * hor / 100.f;
	const auto rel_pos_y = free_space * ver / 100.f;

	const auto label_bounds  = label.getGlobalBounds();
	circle.setPosition(center + rel_pos_x, center + rel_pos_y);
	label.setPosition(center + rel_pos_x, center + rel_pos_y - label_bounds.height * 2 / 5);
}

controller_analog::controller_analog(std::size_t controller_num, const std::string & button_id)
      : controller_analog(controller_num, button_id, colour_theme::classic) {}

controller_analog::controller_analog(std::size_t controller_num, const std::string & button_id, const colour_theme & thm)
      : data(&controller_analog_data().at(button_id)), theme(&thm), controller(controller_num),
        label(data->label, font_default, theme->character_size * 26 / 29), circle(0, 900) {
	const float cell_size    = theme->character_size + theme->character_size / 2;
	const float radius       = cell_size / 2;
	const float outline_size = radius / 6;

	circle.setRadius(radius);
	circle.setOutlineThickness(-outline_size);
	circle.setFillColor(theme->unclicked);
	circle.setOutlineColor(theme->outline);

	label.setFillColor(theme->label);

	const auto label_bounds  = label.getLocalBounds();
	const auto circle_bounds = circle.getLocalBounds();
	label.setOrigin(label_bounds.width / 2, label_bounds.height / 2);
	circle.setOrigin(circle_bounds.width / 2, circle_bounds.height / 2);
}

void controller_analog::tick() {
	const auto vertical   = sf::Joystick::getAxisPosition(controller, data->keycodes.first);
	const auto horizontal = sf::Joystick::getAxisPosition(controller, data->keycodes.second);

	set_positions(horizontal, vertical);
}

sf::FloatRect controller_analog::global_bounds() const {
	auto bgbounds = circle.getLocalBounds();
	auto && pos   = getPosition();
	bgbounds.left = pos.x;
	bgbounds.top  = pos.y;
	return bgbounds;
}

void controller_analog::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(circle, states);
	target.draw(label, states);
}
