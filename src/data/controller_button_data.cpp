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
#include <tuple>


using namespace std::literals;


const std::unordered_map<std::string, controller_button_data_t> & controller_button_data() {
	static const std::unordered_map<std::string, controller_button_data_t> buttons = []() {
		std::unordered_map<std::string, controller_button_data_t> bts;

		for(auto && pr : std::initializer_list<std::tuple<xbox_controller_button, const char *, bool>>{{xbox_controller_button::A, "A", true},
		                                                                                               {xbox_controller_button::B, "B", true},
		                                                                                               {xbox_controller_button::X, "X", true},
		                                                                                               {xbox_controller_button::Y, "Y", true},
		                                                                                               {xbox_controller_button::LB, "LB", false},
		                                                                                               {xbox_controller_button::RB, "RB", false},
		                                                                                               {xbox_controller_button::Back, "Back", true},
		                                                                                               {xbox_controller_button::Start, "Start", true},
		                                                                                               {xbox_controller_button::LS, "LS", true},
		                                                                                               {xbox_controller_button::RS, "RS", true}}) {
			std::string label = std::get<1>(pr);
			controller_button_data_t data{std::get<0>(pr), label, false, false, std::get<2>(pr)};
			bts.emplace(label, data);

			std::transform(label.begin(), label.end(), label.begin(), [](char c) { return std::tolower(c); });
			bts.emplace(label, data);
		}

		return bts;
	}();
	return buttons;
}
