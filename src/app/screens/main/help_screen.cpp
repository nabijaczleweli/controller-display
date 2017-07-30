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


#include "help_screen.hpp"
#include "../../../data/container.hpp"
#include "../../../util/fmt.hpp"
#include "../../../util/url.hpp"
#include "../../../util/utf.hpp"
#include "../../application.hpp"
#include "assets.hpp"
#include "main_app_screen.hpp"
#include <cmath>
#include <fmt/format.h>
#include <tinyfiledialogs.h>


using namespace fmt::literals;


static const sf::Color unclicked_link_colour(0x04, 0x49, 0xe7);


void help_screen::setup() {
	screen::setup();

	auto help_txt = from_utf8(fmt::format(
	    assets::text_help_txt_s,
	    "controller_display_version"_a = CONTROLLER_DISPLAY_VERSION,                                                                                            //
	    "cereal_version"_a             = CEREAL_VERSION,                                                                                                        //
	    "fmt_version"_a = format_version{(FMT_VERSION - (FMT_VERSION % 10000)) / 10000, ((FMT_VERSION - (FMT_VERSION % 100)) / 100) % 100, FMT_VERSION % 100},  //
	    "optional_lite_version"_a   = optional_lite_VERSION,                                                                                                    //
	    "SFML_version"_a            = format_version{SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH},                                               //
	    "tinyfiledialogs_version"_a = tinyfd_version,                                                                                                           //
	    "variant_lite_version"_a    = VARIANT_LITE_VERSION,                                                                                                     //
	    "whereami_cpp_version"_a    = WHEREAMI_CPP_VERSION,                                                                                                     //
	    "yaml_cpp_version"_a        = YAML_CPP_VERSION));
	help_text.setString(help_txt);

	const auto char_height = help_text.getCharacterSize();
	sf::RectangleShape shape;
	shape.setFillColor(unclicked_link_colour);

	{
		const auto first_char_pos = sf::Vector2i(help_text.findCharacterPos(0));
		const sf::IntRect rect(first_char_pos,
		                       {static_cast<int>(help_text.findCharacterPos(help_txt.find(' ')).x - first_char_pos.x), static_cast<int>(char_height)});

		auto link_shape = shape;
		link_shape.setSize(sf::Vector2f(rect.width, rect.height * 1.2));
		link_shape.setPosition(rect.left, rect.top);

		links.emplace_back(rect, link_shape, "https://github.com/nabijaczleweli/controller-display");
	}

	for(auto pfx : {"http://", "https://"})
		for(auto pfx_idx = help_txt.find(pfx); pfx_idx != sf::String::InvalidPos; pfx_idx = help_txt.find(pfx, pfx_idx + 1)) {
			const auto end_idx = help_txt.find('\n', pfx_idx);
			const auto pfx_pos = sf::Vector2i(help_text.findCharacterPos(pfx_idx));

			sf::IntRect rect(sf::Vector2i(help_text.findCharacterPos(pfx_idx)),
			                 {static_cast<int>(help_text.findCharacterPos(end_idx).x - pfx_pos.x), static_cast<int>(char_height)});

			auto link_shape = shape;
			link_shape.setSize(sf::Vector2f(rect.width, rect.height * 1.2));
			link_shape.setPosition(rect.left, rect.top);

			links.emplace_back(rect, link_shape, std::string(help_txt.begin() + pfx_idx, help_txt.begin() + end_idx));
		}

	auto && help_text_bounds = help_text.getGlobalBounds();
	app.resize({static_cast<unsigned int>(std::ceil(help_text_bounds.width)), static_cast<unsigned int>(std::ceil(help_text_bounds.height))});
}

int help_screen::loop() {
	return 0;
}

int help_screen::draw() {
	app.window.draw(help_text);
	for(auto && link : links)
		app.window.draw(std::get<sf::RectangleShape>(link), sf::BlendMultiply);
	return 0;
}

int help_screen::handle_event(const sf::Event & event) {
	if(const auto i = screen::handle_event(event))
		return i;

	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		app.schedule_screen<main_app_screen>(std::move(saved_layout));
	else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		for(auto && link : links)
			if(std::get<sf::IntRect>(link).contains(event.mouseButton.x, event.mouseButton.y))
				launch_browser(std::get<std::string>(link).c_str());

	return 0;
}

help_screen::help_screen(application & theapp, nonstd::optional<sf::String> slayout) : screen(theapp), saved_layout(slayout), help_text("", font_default, 15) {
	help_text.setFillColor({0xE5, 0xE5, 0xE5});
	help_text.setOutlineThickness(1);
	help_text.setOutlineColor({0x1A, 0x1A, 0x1A});
}
