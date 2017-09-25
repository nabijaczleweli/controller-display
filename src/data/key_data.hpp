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


#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


enum class mouse_button_direction : std::uint8_t { left, right, other };
enum class xbox_controller_button : std::uint8_t {
	A     = 0,
	B     = 1,
	X     = 2,
	Y     = 3,
	LB    = 4,
	RB    = 5,
	Back  = 6,
	Start = 7,
	LS    = 8,
	RS    = 9,
};


template <class code_t>
struct key_data {
	code_t keycodes;
	sf::String label;

	bool need_shift;
	bool need_no_shift;

	bool proportional;
};

using keyboard_key_data_t      = key_data<std::vector<sf::Keyboard::Key>>;
using mouse_button_data_t      = key_data<std::pair<sf::Mouse::Button, mouse_button_direction>>;
using controller_button_data_t = key_data<xbox_controller_button>;
using controller_analog_data_t = key_data<std::pair<sf::Joystick::Axis, sf::Joystick::Axis>>;


const std::unordered_map<std::string, keyboard_key_data_t> & keyboard_key_data();
const std::unordered_map<std::string, mouse_button_data_t> & mouse_button_data();
const std::unordered_map<std::string, controller_button_data_t> & controller_button_data();
const std::unordered_map<std::string, controller_analog_data_t> & controller_analog_data();
