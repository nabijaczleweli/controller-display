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


#include "../screen.hpp"
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <functional>
#include <nonstd/optional.hpp>
#include <utility>
#include <vector>


class config_screen : public screen {
private:
	enum class selection_verb : bool { forward, back };
	enum class selected_button : std::uint8_t { vsync, FPS, new_layout_time, end };

	nonstd::optional<sf::String> saved_layout;
	sf::Text config_text;
	std::vector<std::pair<bool, sf::RectangleShape>> options;
	selected_button butt;

	void click(selection_verb what);
	void re_text();


public:
	virtual void setup() override;
	virtual int loop() override;
	virtual int draw() override;
	virtual int handle_event(const sf::Event & event) override;

	config_screen(application & theapp, nonstd::optional<sf::String> layout);
	virtual ~config_screen();
};
