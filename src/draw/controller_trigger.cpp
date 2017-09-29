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


#include "controller_trigger.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"
#include <cmath>


controller_trigger::controller_trigger(std::size_t controller_num, const std::string & button_id)
      : controller_trigger(controller_num, button_id, colour_theme::classic) {}

controller_trigger::controller_trigger(std::size_t controller_num, const std::string & button_id, const colour_theme & thm)
      : data(&controller_trigger_data().at(button_id)), theme(&thm), controller(controller_num), label(data->label, font_default, theme->character_size) {
	auto && label_bounds        = label.getGlobalBounds();
	const float vertical_size   = theme->character_size + theme->character_size / 2;
	const float separator_size  = vertical_size / 6.f;
	const float horizontal_size = vertical_size * 2 + separator_size;
	const float label_pos_x     = vertical_size / 2 - label_bounds.width / 2;
	const float label_pos_y     = theme->character_size / 4 / 2;

	triangle[0] = {{data->keycodes.second ? 0 : horizontal_size, 0}, theme->unclicked};
	triangle[1] = {{0, vertical_size}, theme->unclicked};
	triangle[2] = {{horizontal_size, vertical_size}, theme->unclicked};

	label.setFillColor(theme->label);
	label.setPosition(data->keycodes.second ? label_pos_x : horizontal_size - label_bounds.width - label_pos_x, label_pos_y);

	max_angle = std::atan2(horizontal_size, vertical_size);
}

void controller_trigger::tick() {
	const float vertical_size = theme->character_size + theme->character_size / 2;
	const auto raw            = sf::Joystick::getAxisPosition(controller, data->keycodes.first);
	const auto pressedness    = std::max(data->keycodes.second ? -raw : raw, 0.f);

	triangle[0].position.y = std::sin(max_angle * pressedness / 100.f) * vertical_size;
}

sf::FloatRect controller_trigger::global_bounds() const {
	const float vertical_size   = theme->character_size + theme->character_size / 2;
	const float separator_size  = vertical_size / 6.f;
	const float horizontal_size = vertical_size * 2 + separator_size;

	auto && pos = getPosition();
	return {pos.x, pos.y, horizontal_size, vertical_size};
}

void controller_trigger::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();

	target.draw(triangle, sizeof(triangle) / sizeof(*triangle), sf::TrianglesFan, states);
	sf::Vertex ootline[3];
	std::transform(triangle, triangle + sizeof(triangle) / sizeof(*triangle), ootline, [&](auto from) {
		from.color = theme->outline;
		return from;
	});
	target.draw(ootline, sizeof(ootline) / sizeof(*ootline), sf::LinesStrip, states);

	target.draw(label, states);
}
