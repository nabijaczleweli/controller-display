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
#include "assets.hpp"
#include <cmath>


static const sf::Texture & bumper_texture() {
	static sf::Texture bsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_bumper_png, sizeof assets::buttons_xbox_bumper_png);
		return t;
	}();
	return bsp;
}

static const sf::Texture & bumper_border_texture() {
	static sf::Texture bbsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_bumper_border_png, sizeof assets::buttons_xbox_bumper_border_png);
		return t;
	}();
	return bbsp;
}

static const sf::Texture & control_texture() {
	static sf::Texture csp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_control_png, sizeof assets::buttons_xbox_control_png);
		return t;
	}();
	return csp;
}

static const sf::Texture & control_border_texture() {
	static sf::Texture cbsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_control_border_png, sizeof assets::buttons_xbox_control_border_png);
		return t;
	}();
	return cbsp;
}

static const sf::Texture & control_triangle_texture() {
	static sf::Texture ctsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_control_triangle_png, sizeof assets::buttons_xbox_control_triangle_png);
		return t;
	}();
	return ctsp;
}


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
      : data(&controller_button_data().at(button_id)), theme(&thm), variant(variant_from_button(data->keycodes)), controller(controller_num) {
	const float vertical_size = theme->character_size + theme->character_size / 2;
	const float radius        = vertical_size / 2;
	const float outline_size  = radius / 6;

	switch(variant) {
		case variant_t::xyab:
		case variant_t::stick: {
			unsigned int character_size = theme->character_size;
			if(variant == variant_t::stick)
				character_size = theme->character_size * 26 / 29;
			xyab_stick_variant_data dt{{data->label, font_default, character_size}, sf::CircleShape(radius, 900)};

			dt.circle.setOutlineThickness(-outline_size);
			dt.circle.setOutlineColor(theme->outline);

			const auto label_bounds = dt.label.getGlobalBounds();
			dt.label.setPosition(vertical_size / 2 - label_bounds.width / 2,
			                     theme->character_size / 4 / 2 + (theme->character_size - dt.label.getCharacterSize()) / 8);
			dt.label.setFillColor(theme->label);

			draw_data = std::move(dt);
		} break;
		case variant_t::control: {
			control_variant_data dt{sf::Sprite(control_texture()), sf::Sprite(control_border_texture()), sf::Sprite(control_triangle_texture())};

			dt.control_border.setColor(theme->outline);
			dt.control_triangle.setColor(theme->label);

			// Assumes control size = control_border size = control_triangle size
			const auto && control_size  = control_texture().getSize();
			const auto horizontal_scale = vertical_size / control_size.x;
			const auto vertical_scale   = vertical_size / control_size.y;
			const auto scale            = std::min(horizontal_scale, vertical_scale);
			dt.control.setScale(scale, scale);
			dt.control_border.setScale(scale, scale);
			dt.control_triangle.setScale(scale, scale);
			if(data->keycodes == xbox_controller_button::Start)
				dt.control_triangle.setScale(scale, scale);
			else {
				// Based on https://stackoverflow.com/a/26399604/2851815
				dt.control_triangle.setScale(-scale, scale);
				dt.control_triangle.setOrigin({dt.control_triangle.getLocalBounds().width, 0});
			}

			const auto && control_bounds = dt.control.getGlobalBounds();
			dt.control.setPosition(vertical_size / 2 - control_bounds.width / 2, vertical_size / 2 - control_bounds.height / 2);
			dt.control_border.setPosition(dt.control.getPosition());
			dt.control_triangle.setPosition(dt.control.getPosition());

			draw_data = std::move(dt);
		} break;
		case variant_t::bumper: {
			bumper_variant_data dt{
			    {data->label, font_default, static_cast<unsigned int>(theme->character_size)}, sf::Sprite(bumper_texture()), sf::Sprite(bumper_border_texture())};
			dt.bumper_border.setColor(theme->outline);

			// Assumes bumper size = bumper_border size
			const auto && bumper_size   = bumper_texture().getSize();
			const float separator_size  = vertical_size / 6.f;
			const float horizontal_size = vertical_size * 2 + separator_size;
			const float border_distance = 6.f / 40.f * theme->character_size;

			dt.label.setCharacterSize(theme->character_size * 26 / 29);
			dt.label.setPosition(data->keycodes == xbox_controller_button::LB
			                         ? static_cast<int>(horizontal_size - dt.label.getGlobalBounds().width - 1.5 * border_distance)
			                         : static_cast<int>(border_distance),
			                     0);

			const auto horizontal_scale = horizontal_size / bumper_size.x;
			const auto vertical_scale   = vertical_size / bumper_size.y;
			const auto scale            = std::min(horizontal_scale, vertical_scale);


			if(data->keycodes == xbox_controller_button::LB) {
				dt.bumper.setScale(scale, scale);
				dt.bumper_border.setScale(scale, scale);
			} else {
				// Based on https://stackoverflow.com/a/26399604/2851815
				dt.bumper.setScale(-scale, scale);
				dt.bumper_border.setScale(-scale, scale);
				const auto w = dt.bumper.getLocalBounds().width;
				dt.bumper.setOrigin({w, 0});
				dt.bumper_border.setOrigin({w, 0});
			}

			const auto && bumper_bounds = dt.bumper.getGlobalBounds();
			dt.bumper.setPosition(horizontal_size / 2 - bumper_bounds.width / 2, vertical_size / 2 - bumper_bounds.height / 2);
			dt.bumper_border.setPosition(dt.bumper.getPosition());

			draw_data = std::move(dt);
		} break;
	}
}

void controller_button::tick() {
	const auto pressed = sf::Joystick::isButtonPressed(controller, static_cast<std::uint8_t>(data->keycodes));

	switch(variant) {
		case variant_t::xyab:
			switch(data->keycodes) {
				case xbox_controller_button::A:
					nonstd::get<xyab_stick_variant_data>(draw_data).circle.setFillColor(pressed ? theme->xbox_A_button_clicked : theme->xbox_A_button_unclicked);
					break;
				case xbox_controller_button::B:
					nonstd::get<xyab_stick_variant_data>(draw_data).circle.setFillColor(pressed ? theme->xbox_B_button_clicked : theme->xbox_B_button_unclicked);
					break;
				case xbox_controller_button::X:
					nonstd::get<xyab_stick_variant_data>(draw_data).circle.setFillColor(pressed ? theme->xbox_X_button_clicked : theme->xbox_X_button_unclicked);
					break;
				case xbox_controller_button::Y:
					nonstd::get<xyab_stick_variant_data>(draw_data).circle.setFillColor(pressed ? theme->xbox_Y_button_clicked : theme->xbox_Y_button_unclicked);
					break;
				default:
					break;
			}
			break;
		case variant_t::stick:
			nonstd::get<xyab_stick_variant_data>(draw_data).circle.setFillColor(pressed ? theme->clicked : theme->unclicked);
			break;
		case variant_t::control:
			nonstd::get<control_variant_data>(draw_data).control.setColor(pressed ? theme->clicked : theme->unclicked);
			break;
		case variant_t::bumper:
			nonstd::get<bumper_variant_data>(draw_data).bumper.setColor(pressed ? theme->clicked : theme->unclicked);
			break;
	}
}

sf::FloatRect controller_button::global_bounds() const {
	sf::FloatRect bgbounds;
	switch(variant) {
		case variant_t::xyab:
		case variant_t::stick:
			bgbounds = nonstd::get<xyab_stick_variant_data>(draw_data).circle.getLocalBounds();
			break;
		case variant_t::control:
			bgbounds = nonstd::get<control_variant_data>(draw_data).control.getLocalBounds();
			break;
		case variant_t::bumper:
			bgbounds = nonstd::get<bumper_variant_data>(draw_data).bumper.getLocalBounds();
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
		case variant_t::stick: {
			const auto & dt = nonstd::get<xyab_stick_variant_data>(draw_data);
			target.draw(dt.circle, states);
			target.draw(dt.label, states);
		} break;
		case variant_t::control: {
			const auto & dt = nonstd::get<control_variant_data>(draw_data);
			target.draw(dt.control, states);
			target.draw(dt.control_border, states);
			target.draw(dt.control_triangle, states);
		} break;
		case variant_t::bumper: {
			const auto & dt = nonstd::get<bumper_variant_data>(draw_data);
			target.draw(dt.bumper, states);
			target.draw(dt.bumper_border, states);
			target.draw(dt.label, states);
		} break;
	}
}
