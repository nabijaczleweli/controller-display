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


#include "controller_button.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"


controller_button::variant_t controller_button::variant_from_button(xbox_controller_button butt) {
	switch(butt) {
		case xbox_controller_button::A:
		case xbox_controller_button::B:
		case xbox_controller_button::X:
		case xbox_controller_button::Y:
			return variant_t::xyab;
		case xbox_controller_button::LB:
		case xbox_controller_button::RB:
			return variant_t::bumper;
		case xbox_controller_button::Back:
		case xbox_controller_button::Start:
			return variant_t::control;
		case xbox_controller_button::LS:
		case xbox_controller_button::RS:
			return variant_t::stick;
		default:
			throw "Unknown controller_button in variant_from_button()";
	}
}


controller_button::controller_button(std::size_t controller_num, const std::string & button_id)
      : controller_button(controller_num, button_id, colour_theme::classic) {}

controller_button::controller_button(std::size_t controller_num, const std::string & button_id, const colour_theme & thm)
      : data(&controller_button_data().at(button_id)), theme(&thm), variant(variant_from_button(data->keycodes)), controller(controller_num),
        label(data->label, font_default, theme->character_size), circle(0, 900) {
	const float vertical_size = theme->character_size + theme->character_size / 2;
	const float radius        = vertical_size / 2;
	const float outline_size  = radius / 6;

	label.setFillColor(theme->label);

	switch(variant) {
		case variant_t::xyab: {
			circle.setRadius(radius);
			circle.setOutlineThickness(-outline_size);
			circle.setOutlineColor(theme->outline);

			const auto label_bounds = label.getLocalBounds();
			label.setPosition(vertical_size / 2 - label_bounds.width / 2, theme->character_size / 4 / 2);
		} break;
		case variant_t::stick:
		case variant_t::control:
		case variant_t::bumper:
			// TODO
			break;
	}
}

void controller_button::tick() {
	const auto pressed = sf::Joystick::isButtonPressed(controller, static_cast<std::uint8_t>(data->keycodes));

	switch(variant) {
		case variant_t::xyab:
			switch(data->keycodes) {
				case xbox_controller_button::A:
					circle.setFillColor(pressed ? theme->xbox_A_button_clicked : theme->xbox_A_button_unclicked);
					break;
				case xbox_controller_button::B:
					circle.setFillColor(pressed ? theme->xbox_B_button_clicked : theme->xbox_B_button_unclicked);
					break;
				case xbox_controller_button::X:
					circle.setFillColor(pressed ? theme->xbox_X_button_clicked : theme->xbox_X_button_unclicked);
					break;
				case xbox_controller_button::Y:
					circle.setFillColor(pressed ? theme->xbox_Y_button_clicked : theme->xbox_Y_button_unclicked);
					break;
				default:
					break;
			}
			break;
		case variant_t::stick:
		case variant_t::control:
		case variant_t::bumper:
			// TODO
			break;
	}
}

sf::FloatRect controller_button::global_bounds() const {
	sf::FloatRect bgbounds;
	switch(variant) {
		case variant_t::xyab:
		case variant_t::stick:
			bgbounds = circle.getLocalBounds();
			break;
		case variant_t::control:
		case variant_t::bumper:
			bgbounds = background_rectangle.getLocalBounds();
			break;
	}

	auto && pos   = getPosition();
	bgbounds.left = pos.x;
	bgbounds.top  = pos.y;
	return bgbounds;
}

void controller_button::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();

	switch(variant) {
		case variant_t::xyab:
		case variant_t::stick:
			target.draw(circle, states);
			target.draw(label, states);
			break;
		case variant_t::control:
			target.draw(background_rectangle, states);
			target.draw(foreground_triangle, sizeof(foreground_triangle) / sizeof(*foreground_triangle), sf::Triangles, states);
			break;
		case variant_t::bumper:
			target.draw(background_rectangle, states);
			target.draw(label, states);
			break;
	}
}
