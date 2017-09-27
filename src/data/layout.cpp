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


#include "layout.hpp"
#include "../app/application.hpp"
#include "../util/parse.hpp"
#include "../util/string.hpp"
#include "assets.hpp"
#include "container.hpp"
#include "key_data.hpp"
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <string>
#include <utility>
#include <yaml-cpp/yaml.h>


using namespace std::literals;


static nonstd::optional<std::string> parse_colour_theme(colour_theme & into, YAML::Node from);
static nonstd::optional<std::string> parse_grid_size(std::pair<std::size_t &, std::size_t &> into, YAML::Node from);
static nonstd::optional<std::string> parse_keyset(std::vector<key> & keys_into, std::vector<mouse_button> & mouse_into,
                                                  std::vector<mouse_displacement> & mouse_deltas_into, std::vector<controller_button> & controller_buttons_into,
                                                  std::vector<controller_dpad> & controller_dpads_into,
                                                  std::vector<controller_analog> & controller_analogs_into, YAML::Node from, std::size_t grid_width,
                                                  const colour_theme & theme);
static bool try_preset(colour_theme & thm, const std::string & preset);
static colour_theme load_preset_theme(const char * preset);


const colour_theme colour_theme::classic = load_preset_theme(assets::presets_colour_theme_default_yml_s);
const colour_theme colour_theme::dark    = load_preset_theme(assets::presets_colour_theme_dark_yml_s);


nonstd::variant<std::unique_ptr<layout>, std::string> layout::load_stream(std::istream & strim) {
	try {
		const auto doc = YAML::Load(strim);
		std::unique_ptr<layout> res(new layout);
		res->theme = colour_theme::classic;

		const auto theme = doc["theme"];
		if(theme) {
			const auto result = parse_colour_theme(res->theme, theme);
			if(result)
				return {*result};
		}

		const auto grid = doc["grid"];
		if(!grid)
			return nonstd::variant<std::unique_ptr<layout>, std::string>(nonstd::in_place<std::string>, "Missing required grid key");
		const auto grid_result = parse_grid_size({res->grid_width, res->grid_height}, grid);
		if(grid_result)
			return {*grid_result};

		const auto keys = doc["keys"];
		if(!keys)
			return nonstd::variant<std::unique_ptr<layout>, std::string>(nonstd::in_place<std::string>, "Missing required keys key");
		const auto keys_result = parse_keyset(res->keyboard_keys, res->mouse_buttons, res->mouse_displacements, res->controller_buttons, res->controller_dpads,
		                                      res->controller_analogs, keys, res->grid_width, res->theme);
		if(keys_result)
			return {*keys_result};

		return {std::move(res)};
	} catch(const YAML::ParserException & pe) {
		return nonstd::variant<std::unique_ptr<layout>, std::string>(nonstd::in_place<std::string>, pe.what());
	}
}

sf::Vector2u layout::size() {
	const auto key_size = theme.character_size + theme.character_size / 2;
	return {static_cast<unsigned int>(std::floor(grid_width * key_size + (grid_width - 1) * key_size * (1. / 6.))),
	        static_cast<unsigned int>(std::floor(grid_height * key_size + (grid_height - 1) * key_size * (1. / 6.)))};
}

void layout::tick() {
	for_each(keyboard_keys.begin(), keyboard_keys.end(), [](auto && k) { k.tick(); });
	for_each(mouse_buttons.begin(), mouse_buttons.end(), [](auto && k) { k.tick(); });
	for_each(mouse_displacements.begin(), mouse_displacements.end(), [](auto && k) { k.tick(); });
	for_each(controller_buttons.begin(), controller_buttons.end(), [](auto && k) { k.tick(); });
	for_each(controller_dpads.begin(), controller_dpads.end(), [](auto && k) { k.tick(); });
	for_each(controller_analogs.begin(), controller_analogs.end(), [](auto && k) { k.tick(); });
}

void layout::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	for_each(keyboard_keys.begin(), keyboard_keys.end(), [&](auto && k) { target.draw(k, states); });
	for_each(mouse_buttons.begin(), mouse_buttons.end(), [&](auto && k) { target.draw(k, states); });
	for_each(mouse_displacements.begin(), mouse_displacements.end(), [&](auto && k) { target.draw(k, states); });
	for_each(controller_buttons.begin(), controller_buttons.end(), [&](auto && k) { target.draw(k, states); });
	for_each(controller_dpads.begin(), controller_dpads.end(), [&](auto && k) { target.draw(k, states); });
	for_each(controller_analogs.begin(), controller_analogs.end(), [&](auto && k) { target.draw(k, states); });
}


static nonstd::optional<std::string> parse_colour_theme(colour_theme & into, YAML::Node from) {
	switch(from.Type()) {
		case YAML::NodeType::Scalar:
			if(!try_preset(into, from.Scalar()))
				return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid theme preset name");
			break;

		case YAML::NodeType::Map: {
			const auto base = from["base"];
			if(base && base.IsScalar() && !try_preset(into, base.Scalar()))
				return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid theme preset name");

			const auto char_size = from["character-size"];
			if(char_size) {
				if(!char_size.IsScalar())
					return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid theme.character-size key type");
				errno         = 0;
				const auto cs = std::strtoull(char_size.Scalar().c_str(), nullptr, 0);
				if(!cs || errno == ERANGE)
					return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Key theme.character-size not an integer");
				into.character_size = cs;
			}

			for(auto && pr : std::initializer_list<std::pair<const char *, sf::Color &>>{
			        {"label", into.label}, {"unclicked", into.unclicked}, {"clicked", into.clicked}, {"outline", into.outline}}) {
				const auto clr = from[pr.first];
				if(!clr)
					continue;
				if(!clr.IsScalar())
					return {"Invalid theme."s + pr.first + " key type"};
				const auto parsed = parse_colour(clr.Scalar());
				if(!parsed)
					return {"Invalid CSS3 colour in theme."s + pr.first};
				pr.second = *parsed;
			}

			const auto xbox = from["xbox"];
			if(xbox && xbox.IsMap())
				for(auto && tpl : std::initializer_list<std::pair<const char *, std::pair<sf::Color &, sf::Color &>>>{
				        {"a", {into.xbox_A_button_unclicked, into.xbox_A_button_clicked}},
				        {"b", {into.xbox_B_button_unclicked, into.xbox_B_button_clicked}},
				        {"x", {into.xbox_X_button_unclicked, into.xbox_X_button_clicked}},
				        {"y", {into.xbox_Y_button_unclicked, into.xbox_Y_button_clicked}}}) {
					const auto btn = xbox[tpl.first];
					if(!btn)
						continue;
					if(!btn.IsMap())
						return {"Invalid theme.xbox."s + tpl.first + " key type"};

					for(auto && pr : std::initializer_list<std::pair<const char *, sf::Color &>>{{"unclicked", tpl.second.first}, {"clicked", tpl.second.second}}) {
						const auto clr = btn[pr.first];
						if(!clr)
							continue;
						if(!clr.IsScalar())
							return {"Invalid theme.xbox."s + tpl.first + "." + pr.first + " key type"};
						const auto parsed = parse_colour(clr.Scalar());
						if(!parsed)
							return {"Invalid CSS3 colour in theme.xbox."s + tpl.first + "." + pr.first};
						pr.second = *parsed;
					}
				}
		} break;

		default:
			return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid theme key type");
	}

	return nonstd::nullopt;
}

static nonstd::optional<std::string> parse_grid_size(std::pair<std::size_t &, std::size_t &> into, YAML::Node from) {
	switch(from.Type()) {
		case YAML::NodeType::Scalar: {
			char * h_s = nullptr;
			errno      = 0;
			into.first = std::strtoull(from.Scalar().c_str(), &h_s, 10);
			if(!into.first || errno == ERANGE)
				return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Grid key not in WIDTHxHEIGHT format -- unparsable width");

			if(*h_s != 'x')
				return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Grid key not in WIDTHxHEIGHT format -- missing separator");

			errno       = 0;
			into.second = std::strtoull(h_s + 1, nullptr, 10);
			if(!into.second || errno == ERANGE)
				return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Grid key not in WIDTHxHEIGHT format -- unparsable height");
		} break;

		case YAML::NodeType::Map:
			for(auto && wh : std::initializer_list<std::pair<const char *, std::size_t &>>{{"width", into.first}, {"height", into.second}}) {
				const auto size = from[wh.first];
				if(!size)
					return {"Missing grid."s + wh.first + " key"};
				if(!size.IsScalar())
					return {"Invalid grid."s + wh.first + " key type"};
				errno     = 0;
				wh.second = std::strtoull(size.Scalar().c_str(), nullptr, 0);
				if(!wh.second || errno == ERANGE)
					return {"Key grid."s + wh.first + " not an integer"};
			}
			break;

		default:
			return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid grid key type");
	}

	return nonstd::nullopt;
}

static nonstd::optional<std::string> parse_keyset(std::vector<key> & keys_into, std::vector<mouse_button> & mouse_into,
                                                  std::vector<mouse_displacement> & mouse_deltas_into, std::vector<controller_button> & controller_buttons_into,
                                                  std::vector<controller_dpad> & controller_dpads_into,
                                                  std::vector<controller_analog> & controller_analogs_into, YAML::Node from, std::size_t grid_width,
                                                  const colour_theme & theme) {
	if(!from.IsSequence())
		return nonstd::optional<std::string>(nonstd::in_place<std::string>, "Invalid keys key type");

	const auto key_size = theme.character_size + theme.character_size / 2;

	std::size_t idx   = 0;
	std::size_t x_pos = 0;
	std::size_t y_pos = 0;
	for(auto && key_desc : from) {
		if(!key_desc.IsScalar())
			return {"Invalid keys[" + std::to_string(idx) + "] key type"};

		if(key_desc.Scalar().empty())
			return {"Empty keys[" + std::to_string(idx) + "] name"};
		else if(key_desc.Scalar() == "spacer")
			;  // Completely skip it
		else {
			auto sc               = key_desc.as<std::string>();
			const std::string pfx = std::strtok(&sc[0], ".");
			const auto name       = std::strtok(nullptr, ".");
			if(!name)
				return {"Missing keys[" + std::to_string(idx) + "] key name"};

			if(pfx == "keyboard") {
				const auto itr = keyboard_key_data().find(name);
				if(itr == keyboard_key_data().end())
					return {"Invalid keys[" + std::to_string(idx) + "] key name"};
				keys_into.emplace_back(name, theme);
				keys_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
				if(!itr->second.proportional)
					++x_pos;
			} else if(pfx == "mouse") {
				if(std::strcmp(name, "delta") == 0) {
					mouse_deltas_into.emplace_back(app_configuration.mouse_delta_accumulation_time * application::effective_FPS(), theme);
					mouse_deltas_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
				} else {
					const auto itr = mouse_button_data().find(name);
					if(itr == mouse_button_data().end())
						return {"Invalid keys[" + std::to_string(idx) + "] mouse button name"};
					mouse_into.emplace_back(name, theme);
					mouse_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
				}
			} else if(pfx == "xbox") {
				const auto parsed = parse_function_notation(name);
				if(!parsed)
					return {"Missing keys[" + std::to_string(idx) + "] argument list"};
				if(parsed->second.empty())
					return {"Missing keys[" + std::to_string(idx) + "] controller ID"};

				if(string_eq_caseless(parsed->first, "DPad")) {
					controller_dpads_into.emplace_back(std::strtoll(parsed->second[0].c_str(), nullptr, 10), theme);
					controller_dpads_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
					++x_pos;
				} else if(string_eq_caseless(parsed->first, "Stick")) {
					if(parsed->second.size() < 2)
						return {"Missing keys[" + std::to_string(idx) + "] controller stick name"};
					controller_analogs_into.emplace_back(std::strtoll(parsed->second[0].c_str(), nullptr, 10), ltrim(std::move(parsed->second[1])), theme);
					controller_analogs_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
					++x_pos;
				} else {
					const auto itr = controller_button_data().find(parsed->first);
					if(itr == controller_button_data().end())
						return {"Invalid keys[" + std::to_string(idx) + "] controller button name"};
					controller_buttons_into.emplace_back(std::strtoll(parsed->second[0].c_str(), nullptr, 10), parsed->first, theme);
					controller_buttons_into.back().setPosition(std::floor(x_pos * key_size * (7. / 6.)), std::floor(y_pos * key_size * (7. / 6.)));
					if(!itr->second.proportional)
						++x_pos;
				}
			} else
				return {"Invalid keys[" + std::to_string(idx) + "] category"};
		}

		if(++x_pos >= grid_width) {
			x_pos = 0;
			++y_pos;
		}
		++idx;
	}

	return nonstd::nullopt;
}

static bool try_preset(colour_theme & thm, const std::string & preset) {
	if(preset == "default" || preset == "classic")
		thm = colour_theme::classic;
	else if(preset == "dark")
		thm = colour_theme::dark;
	else
		return false;

	return true;
}

static colour_theme load_preset_theme(const char * preset) {
	colour_theme theme;
	parse_colour_theme(theme, YAML::Load(preset));
	return theme;
}
