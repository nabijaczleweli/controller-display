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


using namespace std::literals;


const std::unordered_map<std::string, mouse_button_data_t> & mouse_button_data() {
	static const std::unordered_map<std::string, mouse_button_data_t> keys = []() {
		std::unordered_map<std::string, mouse_button_data_t> kys;

		{
			mouse_button_data_t data{{sf::Mouse::Left, mouse_button_direction::left}, "M1", false, false, true};
			kys.emplace("Left", data);
			kys.emplace("lmb", data);
			kys.emplace("LMB", data);
			kys.emplace("m1", data);
			kys.emplace("M1", data);
		}

		{
			mouse_button_data_t data{{sf::Mouse::Right, mouse_button_direction::right}, "M2", false, false, true};
			kys.emplace("Right", data);
			kys.emplace("rmb", data);
			kys.emplace("RMB", data);
			kys.emplace("m2", data);
			kys.emplace("M2", data);
		}

		{
			mouse_button_data_t data{{sf::Mouse::Middle, mouse_button_direction::other}, "MM", false, false, true};
			kys.emplace("Middle", data);
			kys.emplace("mmb", data);
			kys.emplace("MMB", data);
			kys.emplace("m3", data);
			kys.emplace("M3", data);
		}

		{
			mouse_button_data_t data{{sf::Mouse::XButton1, mouse_button_direction::other}, "X1", false, false, true};
			kys.emplace("Extra1", data);
			kys.emplace("xmb1", data);
			kys.emplace("XMB1", data);
			kys.emplace("x1", data);
			kys.emplace("X1", data);
		}

		{
			mouse_button_data_t data{{sf::Mouse::XButton2, mouse_button_direction::other}, "X2", false, false, true};
			kys.emplace("Extra2", data);
			kys.emplace("xmb2", data);
			kys.emplace("XMB2", data);
			kys.emplace("x2", data);
			kys.emplace("X2", data);
		}

		return kys;
	}();
	return keys;
}
