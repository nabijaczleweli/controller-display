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


#include "config_screen.hpp"
#include "../../../data/container.hpp"
#include "../../../util/utf.hpp"
#include "../../application.hpp"
#include "assets.hpp"
#include "main_app_screen.hpp"
#include <cmath>
#include <fmt/format.h>


using namespace fmt::literals;


static sf::Color selected_colour(0xEE, 0x11, 0x11);


void config_screen::click(selection_verb what) {
	switch(butt) {
		case selected_button::vsync:
			app_configuration.vsync ^= 1;
			break;

		case selected_button::FPS:
			if(what == selection_verb::forward) {
				app_configuration.FPS += 30;
				if(app_configuration.FPS > 240)
					app_configuration.FPS = 0;
			} else {
				if(app_configuration.FPS < 30)
					app_configuration.FPS = 240;
				else
					app_configuration.FPS -= 30;
			}
			break;

		case selected_button::new_layout_time:
			if(what == selection_verb::forward)
				app_configuration.new_layout_time += .25;
			else
				app_configuration.new_layout_time -= .25;

			if(app_configuration.new_layout_time > 10)
				app_configuration.new_layout_time = 0;
			else if(app_configuration.new_layout_time < 0)
				app_configuration.new_layout_time = 10;
			break;

		case selected_button::end:
			return;
	}

	re_text();
}

void config_screen::re_text() {
	auto config_txt = from_utf8(fmt::format(assets::text_config_txt_s,
	                                        "vsync"_a           = app_configuration.vsync,  //
	                                        "FPS"_a             = app_configuration.FPS,    //
	                                        "new_layout_time"_a = app_configuration.new_layout_time));
	config_text.setString(config_txt);
}

void config_screen::setup() {
	screen::setup();
	re_text();

	auto && config_text_bounds = config_text.getGlobalBounds();
	app.resize({static_cast<unsigned int>(std::ceil(config_text_bounds.width)), static_cast<unsigned int>(std::ceil(config_text_bounds.height))});

	const auto char_height = config_text.getCharacterSize();
	const auto line_height = config_text.findCharacterPos(config_text.getString().find('\n') + 1).y;
	sf::RectangleShape shape;
	shape.setFillColor(selected_colour);
	shape.setSize({config_text_bounds.width, char_height * 1.2f});
	for(auto i = static_cast<selected_button>(0); i < selected_button::end; i = static_cast<selected_button>(static_cast<std::uint8_t>(i) + 1)) {
		options.emplace_back(i == static_cast<selected_button>(0), shape);
		shape.move(0, line_height);
	}
}

int config_screen::loop() {
	return 0;
}

int config_screen::draw() {
	app.window.draw(config_text);
	for(auto && option : options)
		if(std::get<bool>(option))
			app.window.draw(std::get<sf::RectangleShape>(option), sf::BlendMultiply);
	return 0;
}

int config_screen::handle_event(const sf::Event & event) {
	if(const auto i = screen::handle_event(event))
		return i;

	if(event.type == sf::Event::MouseMoved) {
		butt   = selected_button::end;
		auto i = static_cast<selected_button>(0);
		for(auto && option : options) {
			if((std::get<bool>(option) = std::get<sf::RectangleShape>(option).getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)))
				butt = i;

			i = static_cast<selected_button>(static_cast<std::uint8_t>(i) + 1);
		}

		if(butt == selected_button::end)
			app.set_cursor(cursor::type::normal);
		else
			app.set_cursor(cursor::type::hand);
	} else if(event.type == sf::Event::MouseButtonPressed) {
		if(event.mouseButton.button == sf::Mouse::Left)
			click(selection_verb::forward);
		else if(event.mouseButton.button == sf::Mouse::Right)
			click(selection_verb::back);
	} else if(event.type == sf::Event::KeyPressed)
		switch(event.key.code) {
			case sf::Keyboard::Escape:
				app.schedule_screen<main_app_screen>(std::move(saved_layout));
				break;

			case sf::Keyboard::Right:
			case sf::Keyboard::Space:
			case sf::Keyboard::Return:
				click(selection_verb::forward);
				break;

			case sf::Keyboard::Left:
				click(selection_verb::back);
				break;

			case sf::Keyboard::Up:
				if(butt != static_cast<selected_button>(0)) {
					std::get<bool>(options[static_cast<std::uint8_t>(butt)]) = false;
					butt                                                     = static_cast<selected_button>(static_cast<std::uint8_t>(butt) - 1);
					std::get<bool>(options[static_cast<std::uint8_t>(butt)]) = true;
				}
				break;

			case sf::Keyboard::Down:
				if(butt != static_cast<selected_button>(static_cast<std::uint8_t>(selected_button::end) - 1)) {
					std::get<bool>(options[static_cast<std::uint8_t>(butt)]) = false;
					butt                                                     = static_cast<selected_button>(static_cast<std::uint8_t>(butt) + 1);
					std::get<bool>(options[static_cast<std::uint8_t>(butt)]) = true;
				}
				break;

			default:
				break;
		}

	return 0;
}

config_screen::config_screen(application & theapp, nonstd::optional<sf::String> slayout)
      : screen(theapp), saved_layout(slayout), config_text("", font_default, 20), butt(selected_button::vsync) {
	config_text.setFillColor({0xE5, 0xE5, 0xE5});
	config_text.setOutlineThickness(1);
	config_text.setOutlineColor({0x1A, 0x1A, 0x1A});
}

config_screen::~config_screen() {
	if(app_configuration.FPS == 0)
		app_configuration.vsync = true;

	app.load_settings();
	app.set_cursor(cursor::type::normal);
}
