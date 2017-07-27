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


const std::unordered_map<std::string, keyboard_key_data_t> & keyboard_key_data() {
	static const std::unordered_map<std::string, keyboard_key_data_t> keys = []() {
		std::unordered_map<std::string, keyboard_key_data_t> kys;

		for(char c = 'A'; c <= 'Z'; ++c) {
			char lbl[]{c, '\0'};
			keyboard_key_data_t data{{1, static_cast<sf::Keyboard::Key>(sf::Keyboard::A + (c - 'A'))}, lbl, false, false, true};
			kys.emplace(lbl, data);
			lbl[0] += 'a' - 'A';
			kys.emplace(lbl, data);
		}

		for(int i = 0; i <= 9; ++i) {
			char lbl[]{static_cast<char>('0' + i), '\0'};
			const auto toprow_code = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + i);
			const auto numpad_code = static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad0 + i);
			keyboard_key_data_t data{{toprow_code, numpad_code}, lbl, false, true, true};
			kys.emplace(lbl, data);

			data = {{toprow_code}, lbl, false, true, true};
			kys.emplace("Top"s + lbl[0], data);

			data = {{toprow_code}, "Num"s + lbl[0], false, false, false};
			kys.emplace(data.label, data);
		}

		for(int i = 1; i <= 12; ++i) {
			auto lbl = "F" + std::to_string(i);
			keyboard_key_data_t data{{1, static_cast<sf::Keyboard::Key>(sf::Keyboard::F1 + i - 1)}, lbl, false, false, false};
			kys.emplace(lbl, data);
		}

		{
			keyboard_key_data_t data{{sf::Keyboard::Escape}, "Esc", false, false, false};
			kys.emplace("Escape", data);
			kys.emplace("Esc", data);
		}

		{
			keyboard_key_data_t data{{sf::Keyboard::Tilde}, "`", false, true, true};
			kys.emplace("`", data);
			data       = {{sf::Keyboard::Tilde}, "~", true, false, true};
			data.label = "~";
			kys.emplace("~", data);
		}

		for(auto && pr :
		    std::initializer_list<std::pair<int, char>>{{1, '!'}, {2, '@'}, {3, '#'}, {4, '$'}, {5, '%'}, {6, '^'}, {7, '&'}, {8, '*'}, {9, '('}, {0, ')'}}) {
			const char lbl[]{pr.second, '\0'};
			keyboard_key_data_t data{{static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + pr.first)}, lbl, true, false, true};
			kys.emplace(data.label, data);
		}

		{
			keyboard_key_data_t data{{sf::Keyboard::BackSpace}, "Backspace", false, false, false};
			kys.emplace(data.label, data);

			data.keycodes[0] = sf::Keyboard::Tab;
			data.label       = "Tab";
			kys.emplace("Tab", data);
			kys.emplace("Tabulation", data);
			kys.emplace("Tabulator", data);

			data.keycodes[0] = sf::Keyboard::Tab;
			data.label       = "Tab";
			kys.emplace("Tab", data);
			kys.emplace("Tabulation", data);
			kys.emplace("Tabulator", data);

			data.keycodes[0] = sf::Keyboard::Return;
			data.label       = "Enter";
			kys.emplace("Enter", data);
			kys.emplace("Return", data);

			data.keycodes[0] = sf::Keyboard::Space;
			data.label       = "Space";
			kys.emplace(data.label, data);
		}

		for(auto && pr : std::initializer_list<std::pair<sf::Keyboard::Key, std::pair<char, char>>>{
		        {sf::Keyboard::Dash, {'-', '_'}},
		        {sf::Keyboard::Equal, {'=', '+'}},
		        {sf::Keyboard::LBracket, {'[', '{'}},
		        {sf::Keyboard::RBracket, {']', '}'}},
		        {sf::Keyboard::BackSlash, {'\\', '|'}},
		        {sf::Keyboard::SemiColon, {';', ':'}},
		        {sf::Keyboard::Quote, {'\'', '"'}},
		        {sf::Keyboard::Comma, {',', '<'}},
		        {sf::Keyboard::Period, {'.', '>'}},
		        {sf::Keyboard::Slash, {'/', '?'}},
		    }) {
			char lbl[]{pr.second.first, '\0'};
			keyboard_key_data_t data{{pr.first}, lbl, false, true, true};
			kys.emplace(lbl, data);

			lbl[0] = pr.second.second;
			data   = {{pr.first}, lbl, true, false, true};
			kys.emplace(lbl, data);
		}

		{
			keyboard_key_data_t data{{sf::Keyboard::Insert}, "Ins", false, false, false};
			kys.emplace("Ins", data);
			kys.emplace("Insert", data);

			data.keycodes[0] = sf::Keyboard::Home;
			data.label       = "Home";
			kys.emplace("Home", data);

			data.keycodes[0] = sf::Keyboard::PageUp;
			data.label       = "PgUp";
			kys.emplace("PgUp", data);
			kys.emplace("PageUp", data);
			kys.emplace("Page Up", data);

			data.keycodes[0] = sf::Keyboard::Delete;
			data.label       = "Del";
			kys.emplace("Del", data);
			kys.emplace("Delete", data);

			data.keycodes[0] = sf::Keyboard::End;
			data.label       = "End";
			kys.emplace("End", data);

			data.keycodes[0] = sf::Keyboard::PageDown;
			data.label       = "PgDown";
			kys.emplace("PgDown", data);
			kys.emplace("PageDown", data);
			kys.emplace("Page Down", data);

			data.keycodes[0] = sf::Keyboard::Pause;
			data.label       = "Pause";
			kys.emplace("Pause", data);
			kys.emplace("PauseBreak", data);
			kys.emplace("Pause Break", data);


			data.proportional = true;

			data.keycodes[0] = sf::Keyboard::Up;
			data.label       = static_cast<sf::Uint32>(0x2191);  // ↑
			kys.emplace("Up", data);
			kys.emplace("UpArrow", data);
			kys.emplace("ArrowUp", data);

			data.keycodes[0] = sf::Keyboard::Down;
			data.label       = static_cast<sf::Uint32>(0x2193);  // ↓
			kys.emplace("Down", data);
			kys.emplace("DownArrow", data);
			kys.emplace("ArrowDown", data);

			data.keycodes[0] = sf::Keyboard::Left;
			data.label       = static_cast<sf::Uint32>(0x2190);  // ←
			kys.emplace("Left", data);
			kys.emplace("LeftArrow", data);
			kys.emplace("ArrowLeft", data);

			data.keycodes[0] = sf::Keyboard::Right;
			data.label       = static_cast<sf::Uint32>(0x2192);  // →
			kys.emplace("Right", data);
			kys.emplace("RightArrow", data);
			kys.emplace("ArrowRight", data);

			data.keycodes[0] = sf::Keyboard::Divide;
			data.label       = "/";
			kys.emplace("Num/", data);
			kys.emplace("NumSlash", data);

			data.keycodes[0] = sf::Keyboard::Multiply;
			data.label       = "*";
			kys.emplace("Num*", data);
			kys.emplace("NumAsterix", data);

			data.keycodes[0] = sf::Keyboard::Subtract;
			data.label       = "-";
			kys.emplace("Num-", data);
			kys.emplace("NumDash", data);

			data.keycodes[0] = sf::Keyboard::Add;
			data.label       = "+";
			kys.emplace("Num+", data);
			kys.emplace("NumPlus", data);

			data.keycodes[0] = sf::Keyboard::Add;
			data.label       = "+";
			kys.emplace("Num+", data);
			kys.emplace("NumPlus", data);
		}

		for(auto && pr : std::initializer_list<std::pair<std::pair<sf::Keyboard::Key, sf::Keyboard::Key>, std::pair<std::string, std::string>>>{
		        {{sf::Keyboard::LShift, sf::Keyboard::RShift}, {"Shift", "Shift"}},
		        {{sf::Keyboard::LAlt, sf::Keyboard::RAlt}, {"Alt", "Alt"}},
		        {{sf::Keyboard::LControl, sf::Keyboard::RControl}, {"Control", "Ctrl"}},
		        {{sf::Keyboard::LSystem, sf::Keyboard::RSystem}, {"System", "Sys"}}}) {
			keyboard_key_data_t data{{pr.first.first, pr.first.second}, pr.second.second, false, false, false};
			kys.emplace(pr.second.first, data);

			data = {{pr.first.first}, 'L' + pr.second.second, false, false, false};
			kys.emplace('L' + pr.second.first, data);
			kys.emplace("Left" + pr.second.first, data);

			data = {{pr.first.second}, 'R' + pr.second.second, false, false, false};
			kys.emplace('R' + pr.second.first, data);
			kys.emplace("Right" + pr.second.first, data);
		}

		return kys;
	}();
	return keys;
}
