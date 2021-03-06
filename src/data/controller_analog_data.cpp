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


#include "key_data.hpp"
#include <algorithm>
#include <utility>


using namespace std::literals;


const std::unordered_map<std::string, controller_analog_data_t> & controller_analog_data() {
	static const std::unordered_map<std::string, controller_analog_data_t> buttons = []() {
		std::unordered_map<std::string, controller_analog_data_t> bts;

		for(auto && pr : std::initializer_list<std::pair<const char *, controller_analog_keycodes>>{
		        {"RS", {sf::Joystick::Axis::R, sf::Joystick::Axis::U, xbox_controller_button::RS}},
		        {"LS", {sf::Joystick::Axis::Y, sf::Joystick::Axis::X, xbox_controller_button::LS}}}) {
			std::string label = pr.first;
			controller_analog_data_t data{pr.second, label, false, false, false};
			bts.emplace(label, data);

			std::transform(label.begin(), label.end(), label.begin(), [](char c) { return std::tolower(c); });
			bts.emplace(std::move(label), data);
		}

		return bts;
	}();
	return buttons;
}
