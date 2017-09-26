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


#include "controller_dpad.hpp"
#include "../data/layout.hpp"
#include "../data/container.hpp"
#include "assets.hpp"
#include <algorithm>
#include <cmath>


static const sf::Texture & dpad_texture() {
	static sf::Texture dsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_png, sizeof assets::buttons_xbox_dpad_png);
		return t;
	}();
	return dsp;
}

static const sf::Texture & dpad_border_texture() {
	static sf::Texture dbsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_border_png, sizeof assets::buttons_xbox_dpad_border_png);
		return t;
	}();
	return dbsp;
}

static const sf::Texture & dpad_up_texture() {
	static sf::Texture dusp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_up_png, sizeof assets::buttons_xbox_dpad_up_png);
		return t;
	}();
	return dusp;
}

static const sf::Texture & dpad_right_texture() {
	static sf::Texture drsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_right_png, sizeof assets::buttons_xbox_dpad_right_png);
		return t;
	}();
	return drsp;
}

static const sf::Texture & dpad_down_texture() {
	static sf::Texture ddsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_down_png, sizeof assets::buttons_xbox_dpad_down_png);
		return t;
	}();
	return ddsp;
}

static const sf::Texture & dpad_left_texture() {
	static sf::Texture dlsp = []() {
		sf::Texture t;
		t.loadFromMemory(assets::buttons_xbox_dpad_left_png, sizeof assets::buttons_xbox_dpad_left_png);
		return t;
	}();
	return dlsp;
}


controller_dpad::controller_dpad(std::size_t controller_num) : controller_dpad(controller_num, colour_theme::classic) {}

controller_dpad::controller_dpad(std::size_t controller_num, const colour_theme & thm)
      : theme(&thm), controller(controller_num), dpad(dpad_texture()), dpad_outline(dpad_border_texture()), dpad_up(false, dpad_up_texture()),
        dpad_right(false, dpad_right_texture()), dpad_down(false, dpad_down_texture()), dpad_left(false, dpad_left_texture()) {
	const float cell_size      = theme->character_size + theme->character_size / 2;
	const float separator_size = cell_size / 6.f;
	const float xy_size        = cell_size * 2 + separator_size;

	// Assumes dpad size = dpad_outline size = dpad_up size = dpad_right size = dpad_down size = dpad_left size
	const auto && border_size   = dpad_border_texture().getSize();
	const auto horizontal_scale = xy_size / border_size.x;
	const auto vertical_scale   = xy_size / border_size.y;
	const auto scale            = std::min(horizontal_scale, vertical_scale);

	dpad.setScale(scale, scale);
	dpad_outline.setScale(scale, scale);
	dpad_up.second.setScale(scale, scale);
	dpad_right.second.setScale(scale, scale);
	dpad_down.second.setScale(scale, scale);
	dpad_left.second.setScale(scale, scale);

	dpad.setColor(theme->unclicked);
	dpad_outline.setColor(theme->outline);
	dpad_up.second.setColor(theme->label);
	dpad_right.second.setColor(theme->label);
	dpad_down.second.setColor(theme->label);
	dpad_left.second.setColor(theme->label);
}

void controller_dpad::tick() {
	const auto horizontal_pos = sf::Joystick::getAxisPosition(controller, sf::Joystick::Axis::PovX);
	const auto vertical_pos   = sf::Joystick::getAxisPosition(controller, sf::Joystick::Axis::PovY);

	dpad_up.first    = vertical_pos > app_configuration.controller_axis_epsilon;
	dpad_down.first  = vertical_pos < -app_configuration.controller_axis_epsilon;
	dpad_left.first  = horizontal_pos < -app_configuration.controller_axis_epsilon;
	dpad_right.first = horizontal_pos > app_configuration.controller_axis_epsilon;
}

sf::FloatRect controller_dpad::global_bounds() const {
	auto bgbounds = dpad.getGlobalBounds();
	auto && pos   = getPosition();
	bgbounds.left = pos.x;
	bgbounds.top  = pos.y;
	return bgbounds;
}

void controller_dpad::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(dpad, states);

	if(dpad_up.first)
		target.draw(dpad_up.second, states);
	if(dpad_right.first)
		target.draw(dpad_right.second, states);
	if(dpad_down.first)
		target.draw(dpad_down.second, states);
	if(dpad_left.first)
		target.draw(dpad_left.second, states);

	target.draw(dpad_outline, states);
}
