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
#include <algorithm>


static const sf::Color label_colour(0x10, 0x10, 0x10);
static const sf::Color background_colour_clicked(0xF2, 0xF2, 0xF2);
static const sf::Color background_colour_unclicked(0xD8, 0xD8, 0xD8);


key::key(const std::string & key_id) : data(keyboard_key_data().at(key_id)), label(data.label, font_default, 40) {
	auto && label_bounds          = label.getGlobalBounds();
	const float background_height  = label.getCharacterSize() + label.getCharacterSize() / 2;
	const float background_width   = data.proportional ? background_height : label.getCharacterSize() + label_bounds.width;

	background.setSize({background_width, background_height});
	background.setOutlineThickness(-1);
	background.setOutlineColor(sf::Color::Black);

	label.setFillColor(label_colour);
	label.setPosition(background_width / 2 - label_bounds.width / 2, label.getCharacterSize() / 4 / 2);
}

void key::tick() {
	bool shift_ok = !data.need_shift && !data.need_no_shift;
	if(!shift_ok) {
		const auto shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		shift_ok         = (data.need_shift && shift) || (data.need_no_shift && !shift);
	}

	if(shift_ok && std::any_of(data.keycodes.begin(), data.keycodes.end(), sf::Keyboard::isKeyPressed))
		background.setFillColor(background_colour_clicked);
	else
		background.setFillColor(background_colour_unclicked);
}

void key::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(background, states);
	target.draw(label, states);
}
