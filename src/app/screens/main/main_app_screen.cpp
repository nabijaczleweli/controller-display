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


#include "main_app_screen.hpp"
#include "../../../data/container.hpp"
#include "../../../util/dialog.hpp"
#include "../../application.hpp"
#include "assets.hpp"
#include "config_screen.hpp"
#include "help_screen.hpp"
#include <fstream>
#include <iterator>
#include <sstream>


static std::unique_ptr<layout> default_layout() {
	std::istringstream ss(assets::presets_layout_default_yml_s);
	auto loaded = layout::load_stream(ss);

	std::unique_ptr<layout> out;
	nonstd::get<std::unique_ptr<layout>>(loaded).swap(out);
	return out;
}


void main_app_screen::load_layout() {
	if(layout_src) {
		auto file = *layout_src;
		std::ifstream infile(file);

		auto new_layout = layout::load_stream(infile);
		if(nonstd::holds_alternative<std::string>(new_layout))
			layout_info.first.setString(nonstd::get<std::string>(new_layout));
		else {
			file.erase(
			    0,
			    std::find_if(std::make_reverse_iterator(file.end()), std::make_reverse_iterator(file.begin()), [](auto c) { return c == '/' || c == '\\'; }).base() -
			        file.begin());
			const auto last_dot = std::find(std::make_reverse_iterator(file.end()), std::make_reverse_iterator(file.begin()), '.').base();
			if(last_dot != file.begin())
				file.erase(last_dot - file.begin() - 1, file.getSize());

			layout_info.first.setString(file);
			nonstd::get<std::unique_ptr<layout>>(new_layout).swap(cur_layout);
			app.resize(cur_layout->size());
		}
	} else {
		cur_layout = default_layout();
		layout_info.first.setString("default");
	}
	app.resize(cur_layout->size());
	layout_info.second = app_configuration.new_layout_time * application::effective_FPS();
}

void main_app_screen::setup() {
	screen::setup();
	load_layout();
}

int main_app_screen::loop() {
	cur_layout->tick();

	if(layout_info.second) {
		auto && text_bounds    = layout_info.first.getGlobalBounds();
		auto && window_size    = app.window.getSize();
		const auto full_length = app_configuration.new_layout_time * application::effective_FPS();
		const auto fade_length = full_length / 5;

		if(layout_info.second >= full_length - fade_length) {
			const auto progress = static_cast<double>(full_length - layout_info.second) / fade_length;
			layout_info.first.setPosition(window_size.x - text_bounds.width, (progress * progress - 1.) * text_bounds.height);
		} else if(layout_info.second <= fade_length) {
			const auto progress = static_cast<double>(layout_info.second) / fade_length;
			layout_info.first.setPosition(window_size.x - text_bounds.width + (1. - progress * progress) * text_bounds.width, 0);
		}

		--layout_info.second;
	}

	return 0;
}

int main_app_screen::draw() {
	app.window.draw(*cur_layout);
	if(layout_info.second)
		app.window.draw(layout_info.first);
	return 0;
}

static const char * axis_str(sf::Joystick::Axis axis) {
	switch(axis) {
		case sf::Joystick::Axis::X:
			return "X";
		case sf::Joystick::Axis::Y:
			return "Y";
		case sf::Joystick::Axis::Z:
			return "Z";
		case sf::Joystick::Axis::R:
			return "R";
		case sf::Joystick::Axis::U:
			return "U";
		case sf::Joystick::Axis::V:
			return "V";
		case sf::Joystick::Axis::PovX:
			return "PovX";
		case sf::Joystick::Axis::PovY:
			return "PovY";
	}
	return "ERR";
}

int main_app_screen::handle_event(const sf::Event & event) {
	if(const auto i = screen::handle_event(event))
		return i;

	if(event.type == sf::Event::KeyPressed)
		switch(event.key.code) {
			case sf::Keyboard::O: {
				auto file = pick_file_dialog("YAML files", {"*.YML", "*.YAML"});
				if(file) {
					layout_src = file;
					load_layout();
				}
			} break;

			case sf::Keyboard::Slash:
				if(!event.key.shift)
					break;
				[[fallthrough]];
			case sf::Keyboard::H:
				app.schedule_screen<help_screen>(std::move(layout_src));
				break;

			case sf::Keyboard::C:
				app.schedule_screen<config_screen>(std::move(layout_src));
				break;

			default:
				break;
		}

	return 0;
}

main_app_screen::main_app_screen(application & theapp, nonstd::optional<sf::String> layout_file)
      : screen(theapp), layout_src(layout_file), layout_info({"", font_default, 15}, 0) {
	layout_info.first.setOutlineThickness(1);
	layout_info.first.setOutlineColor(sf::Color::Black);
}
