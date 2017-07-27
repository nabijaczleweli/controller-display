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


#include "mouse_button.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"
#include <algorithm>
#include <cmath>
#include <vector>


static const sf::Vector2f lmb_curve_coords[]{{2, 111}, {-6, -10}, {41, 6}};
static const sf::Vector2f mmb_curve_coords[]{{40, 127}, {-40, 48}, {40, 0}};
static const std::vector<sf::Vector2f> rmb_curve_coords = []() {
	std::vector<sf::Vector2f> crds(3);
	const auto max_x = std::max_element(std::begin(lmb_curve_coords), std::end(lmb_curve_coords), [](auto && lhs, auto && rhs) { return lhs.x < rhs.x; })->x;
	std::transform(std::begin(lmb_curve_coords), std::end(lmb_curve_coords), std::begin(crds), [&](auto && v) { return sf::Vector2f(max_x - v.x, v.y); });
	return crds;
}();
static const sf::Vector2f * coords(mouse_button_direction of) {
	switch(of) {
		case mouse_button_direction::left:
			return lmb_curve_coords;
		case mouse_button_direction::right:
			return rmb_curve_coords.data();
		default:
			return mmb_curve_coords;
	}
}


mouse_button::mouse_button(const std::string & button_id) : mouse_button(button_id, colour_theme::classic) {}

mouse_button::mouse_button(const std::string & button_id, const colour_theme & thm)
      : data(&mouse_button_data().at(button_id)), theme(&thm), label(data->label, font_default, theme->character_size), fill(coords(data->keycodes.second)),
        outline(coords(data->keycodes.second)) {
	auto && label_bounds        = label.getGlobalBounds();
	auto && curve_bounds        = fill.local_bounds();
	const float horizontal_size = theme->character_size + theme->character_size / 2;
	const float separator_size  = horizontal_size / 6.f;
	const float vertical_size   = horizontal_size * 2 + separator_size;

	fill.fill = true;

	const auto x_scale = horizontal_size / curve_bounds.width;
	const auto y_scale = vertical_size / curve_bounds.height;
	fill.setScale(x_scale, y_scale);
	outline.setScale(x_scale, y_scale);

	outline_fill[0].position = fill.start_point;
	switch(data->keycodes.second) {
		case mouse_button_direction::left:
			outline_fill[1].position = {curve_bounds.left + curve_bounds.width, curve_bounds.top + curve_bounds.height};
			break;
		case mouse_button_direction::right:
			outline_fill[1].position = {curve_bounds.left, curve_bounds.top + curve_bounds.height};
			break;
		case mouse_button_direction::other:
			outline_fill[1].position = fill.end_point;
			break;
	}
	outline_fill[2].position = fill.end_point;
	std::for_each(outline_fill, outline_fill + sizeof(outline_fill) / sizeof(*outline_fill), [&](auto && v) { v.color = theme->outline; });

	std::copy(outline_fill, outline_fill + sizeof(outline_fill) / sizeof(*outline_fill), fill_fill);

	const auto trans = sf::Transform().scale(x_scale, y_scale);
	std::for_each(outline_fill, outline_fill + sizeof(outline_fill) / sizeof(*outline_fill), [&](auto && op) { op.position = trans * op.position; });
	std::for_each(fill_fill, fill_fill + sizeof(fill_fill) / sizeof(*fill_fill), [&](auto && op) { op.position = trans * op.position; });

	fill.colour(theme->unclicked);
	outline.colour(theme->outline);

	label.setFillColor(theme->label);
	label.setPosition(horizontal_size / 2 - label_bounds.width / 2, horizontal_size + separator_size + theme->character_size / 4 / 2);
	if(data->keycodes.second == mouse_button_direction::other)
		label.move(0, -(horizontal_size + separator_size) / 2. - theme->character_size / 8.);
}

void mouse_button::tick() {
	if(sf::Mouse::isButtonPressed(data->keycodes.first)) {
		fill.colour(theme->clicked);
		std::for_each(fill_fill, fill_fill + sizeof(fill_fill) / sizeof(*fill_fill), [&](auto && v) { v.color = theme->clicked; });
	} else {
		fill.colour(theme->unclicked);
		std::for_each(fill_fill, fill_fill + sizeof(fill_fill) / sizeof(*fill_fill), [&](auto && v) { v.color = theme->unclicked; });
	}
}

sf::FloatRect mouse_button::global_bounds() const {
	auto bgbounds = fill.local_bounds();
	auto && pos   = getPosition();
	bgbounds.left = pos.x;
	bgbounds.top  = pos.y;
	return bgbounds;
}

void mouse_button::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();

	target.draw(fill, states);
	target.draw(fill_fill, sizeof(fill_fill) / sizeof(*fill_fill), sf::Triangles, states);
	sf::Vertex fill2[]{fill_fill[0], fill_fill[2]};
	target.draw(fill2, sizeof(fill2) / sizeof(*fill2), sf::Lines, states);

	target.draw(outline, states);
	target.draw(outline_fill, sizeof(outline_fill) / sizeof(*outline_fill), sf::LinesStrip, states);

	target.draw(label, states);
}
