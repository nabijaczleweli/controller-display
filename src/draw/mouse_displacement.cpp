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


#include "mouse_displacement.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"
#include <fmt/format.h>
#include <numeric>
#include <vector>


mouse_displacement::mouse_displacement(std::size_t history_length) : mouse_displacement(history_length, colour_theme::classic) {}

mouse_displacement::mouse_displacement(std::size_t history_length, const colour_theme & thm)
      : theme(&thm), past_deltas(history_length), past_deltas_cur_idx(0), last_position(sf::Mouse::getPosition()),
        label("", font_default, theme->character_size * 3 / 9) {
	const float horizontal_size = theme->character_size + theme->character_size / 2;
	const float separator_size  = horizontal_size / 6.f;
	const float mid_pos         = horizontal_size + separator_size / 2.f;

	vector[0] = vector[1] = {{mid_pos, mid_pos}, theme->unclicked};
	label.setFillColor(theme->label);
}

void mouse_displacement::tick() {
	const float horizontal_size = theme->character_size + theme->character_size / 2;
	const float separator_size  = horizontal_size / 6.f;
	const int mid_pos           = horizontal_size + separator_size / 2.f;

	auto && new_pos                  = sf::Mouse::getPosition();
	past_deltas_cur_idx              = (past_deltas_cur_idx + 1) % past_deltas.size();
	past_deltas[past_deltas_cur_idx] = new_pos - last_position;

	const auto sum_delta  = std::accumulate(past_deltas.begin(), past_deltas.end(), sf::Vector2i{});
	auto normalised_delta = sum_delta;
	normalised_delta.x    = std::min(std::max(normalised_delta.x / 2, -mid_pos), mid_pos);
	normalised_delta.y    = std::min(std::max(normalised_delta.y / 2, -mid_pos), mid_pos);
	vector[1].position    = static_cast<sf::Vector2f>(static_cast<sf::Vector2i>(vector[0].position) + normalised_delta);

	label.setString(fmt::format("({:+}, {:+})", sum_delta.x, sum_delta.y));
	const auto && label_size = label.getGlobalBounds();
	auto new_label_pos = vector[0].position;
	new_label_pos.x -= label_size.width / 2;
	label.setPosition(static_cast<sf::Vector2f>(static_cast<sf::Vector2i>(new_label_pos)));

	last_position = new_pos;
}

sf::FloatRect mouse_displacement::global_bounds() const {
	const float horizontal_size = theme->character_size + theme->character_size / 2;
	const float separator_size  = horizontal_size / 6.f;
	const float w_h             = horizontal_size * 2.f + separator_size;

	auto && pos = getPosition();
	return {pos.x, pos.y, w_h, w_h};
}

void mouse_displacement::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();

	target.draw(vector, sizeof(vector) / sizeof(*vector), sf::Lines, states);
	vector[1].position.x += 1;
	target.draw(vector, sizeof(vector) / sizeof(*vector), sf::Lines, states);
	vector[1].position.x -= 2;
	target.draw(vector, sizeof(vector) / sizeof(*vector), sf::Lines, states);
	vector[1].position.x += 1;
	vector[1].position.y += 1;
	target.draw(vector, sizeof(vector) / sizeof(*vector), sf::Lines, states);
	vector[1].position.y -= 2;
	target.draw(vector, sizeof(vector) / sizeof(*vector), sf::Lines, states);

	target.draw(label, states);
}
