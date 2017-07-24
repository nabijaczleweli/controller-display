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


#include "key.hpp"
#include "../data/container.hpp"
#include "../data/layout.hpp"
#include <algorithm>


key::key(const std::string & key_id) : key(key_id, colour_theme::classic) {}

key::key(const std::string & key_id, const colour_theme & thm)
      : data(&keyboard_key_data().at(key_id)), theme(&thm), label(data->label, font_default, theme->character_size) {
	auto && label_bounds          = label.getGlobalBounds();
	const float background_height = theme->character_size + theme->character_size / 2;
	const float background_width  = data->proportional ? background_height : theme->character_size + label_bounds.width;

	background.setSize({background_width, background_height});
	background.setOutlineThickness(-1);
	background.setOutlineColor(theme->outline);

	label.setFillColor(theme->label);
	label.setPosition(background_width / 2 - label_bounds.width / 2, theme->character_size / 4 / 2);
}

void key::tick() {
	bool shift_ok = !data->need_shift && !data->need_no_shift;
	if(!shift_ok) {
		const auto shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		shift_ok         = (data->need_shift && shift) || (data->need_no_shift && !shift);
	}

	if(shift_ok && std::any_of(data->keycodes.begin(), data->keycodes.end(), sf::Keyboard::isKeyPressed))
		background.setFillColor(theme->clicked);
	else
		background.setFillColor(theme->unclicked);
}

sf::FloatRect key::global_bounds() const {
	auto bgbounds = background.getGlobalBounds();
	auto && pos   = getPosition();
	bgbounds.left = pos.x;
	bgbounds.top  = pos.y;
	return bgbounds;
}

void key::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(background, states);
	target.draw(label, states);
}
