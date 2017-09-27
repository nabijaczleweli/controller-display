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


#include "parse.hpp"
#include "string.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unordered_map>


static std::uint8_t parse_css3_int(const std::string & str);
static double parse_css3_float(const std::string & str);
static double css3_hue_to_rgb(double m1, double m2, double h);

static const std::unordered_map<std::string, sf::Color> keywords{
    {"transparent", {0, 0, 0, 0}},
    {"aliceblue", {240, 248, 255}},
    {"antiquewhite", {250, 235, 215}},
    {"aqua", {0, 255, 255}},
    {"aquamarine", {127, 255, 212}},
    {"azure", {240, 255, 255}},
    {"beige", {245, 245, 220}},
    {"bisque", {255, 228, 196}},
    {"black", {0, 0, 0}},
    {"blanchedalmond", {255, 235, 205}},
    {"blue", {0, 0, 255}},
    {"blueviolet", {138, 43, 226}},
    {"brown", {165, 42, 42}},
    {"burlywood", {222, 184, 135}},
    {"cadetblue", {95, 158, 160}},
    {"chartreuse", {127, 255, 0}},
    {"chocolate", {210, 105, 30}},
    {"coral", {255, 127, 80}},
    {"cornflowerblue", {100, 149, 237}},
    {"cornsilk", {255, 248, 220}},
    {"crimson", {220, 20, 60}},
    {"cyan", {0, 255, 255}},
    {"darkblue", {0, 0, 139}},
    {"darkcyan", {0, 139, 139}},
    {"darkgoldenrod", {184, 134, 11}},
    {"darkgray", {169, 169, 169}},
    {"darkgreen", {0, 100, 0}},
    {"darkgrey", {169, 169, 169}},
    {"darkkhaki", {189, 183, 107}},
    {"darkmagenta", {139, 0, 139}},
    {"darkolivegreen", {85, 107, 47}},
    {"darkorange", {255, 140, 0}},
    {"darkorchid", {153, 50, 204}},
    {"darkred", {139, 0, 0}},
    {"darksalmon", {233, 150, 122}},
    {"darkseagreen", {143, 188, 143}},
    {"darkslateblue", {72, 61, 139}},
    {"darkslategray", {47, 79, 79}},
    {"darkslategrey", {47, 79, 79}},
    {"darkturquoise", {0, 206, 209}},
    {"darkviolet", {148, 0, 211}},
    {"deeppink", {255, 20, 147}},
    {"deepskyblue", {0, 191, 255}},
    {"dimgray", {105, 105, 105}},
    {"dimgrey", {105, 105, 105}},
    {"dodgerblue", {30, 144, 255}},
    {"firebrick", {178, 34, 34}},
    {"floralwhite", {255, 250, 240}},
    {"forestgreen", {34, 139, 34}},
    {"fuchsia", {255, 0, 255}},
    {"gainsboro", {220, 220, 220}},
    {"ghostwhite", {248, 248, 255}},
    {"gold", {255, 215, 0}},
    {"goldenrod", {218, 165, 32}},
    {"gray", {128, 128, 128}},
    {"green", {0, 128, 0}},
    {"greenyellow", {173, 255, 47}},
    {"grey", {128, 128, 128}},
    {"honeydew", {240, 255, 240}},
    {"hotpink", {255, 105, 180}},
    {"indianred", {205, 92, 92}},
    {"indigo", {75, 0, 130}},
    {"ivory", {255, 255, 240}},
    {"khaki", {240, 230, 140}},
    {"lavender", {230, 230, 250}},
    {"lavenderblush", {255, 240, 245}},
    {"lawngreen", {124, 252, 0}},
    {"lemonchiffon", {255, 250, 205}},
    {"lightblue", {173, 216, 230}},
    {"lightcoral", {240, 128, 128}},
    {"lightcyan", {224, 255, 255}},
    {"lightgoldenrodyellow", {250, 250, 210}},
    {"lightgray", {211, 211, 211}},
    {"lightgreen", {144, 238, 144}},
    {"lightgrey", {211, 211, 211}},
    {"lightpink", {255, 182, 193}},
    {"lightsalmon", {255, 160, 122}},
    {"lightseagreen", {32, 178, 170}},
    {"lightskyblue", {135, 206, 250}},
    {"lightslategray", {119, 136, 153}},
    {"lightslategrey", {119, 136, 153}},
    {"lightsteelblue", {176, 196, 222}},
    {"lightyellow", {255, 255, 224}},
    {"lime", {0, 255, 0}},
    {"limegreen", {50, 205, 50}},
    {"linen", {250, 240, 230}},
    {"magenta", {255, 0, 255}},
    {"maroon", {128, 0, 0}},
    {"mediumaquamarine", {102, 205, 170}},
    {"mediumblue", {0, 0, 205}},
    {"mediumorchid", {186, 85, 211}},
    {"mediumpurple", {147, 112, 219}},
    {"mediumseagreen", {60, 179, 113}},
    {"mediumslateblue", {123, 104, 238}},
    {"mediumspringgreen", {0, 250, 154}},
    {"mediumturquoise", {72, 209, 204}},
    {"mediumvioletred", {199, 21, 133}},
    {"midnightblue", {25, 25, 112}},
    {"mintcream", {245, 255, 250}},
    {"mistyrose", {255, 228, 225}},
    {"moccasin", {255, 228, 181}},
    {"navajowhite", {255, 222, 173}},
    {"navy", {0, 0, 128}},
    {"oldlace", {253, 245, 230}},
    {"olive", {128, 128, 0}},
    {"olivedrab", {107, 142, 35}},
    {"orange", {255, 165, 0}},
    {"orangered", {255, 69, 0}},
    {"orchid", {218, 112, 214}},
    {"palegoldenrod", {238, 232, 170}},
    {"palegreen", {152, 251, 152}},
    {"paleturquoise", {175, 238, 238}},
    {"palevioletred", {219, 112, 147}},
    {"papayawhip", {255, 239, 213}},
    {"peachpuff", {255, 218, 185}},
    {"peru", {205, 133, 63}},
    {"pink", {255, 192, 203}},
    {"plum", {221, 160, 221}},
    {"powderblue", {176, 224, 230}},
    {"purple", {128, 0, 128}},
    {"rebeccapurple", {102, 51, 153}},
    {"red", {255, 0, 0}},
    {"rosybrown", {188, 143, 143}},
    {"royalblue", {65, 105, 225}},
    {"saddlebrown", {139, 69, 19}},
    {"salmon", {250, 128, 114}},
    {"sandybrown", {244, 164, 96}},
    {"seagreen", {46, 139, 87}},
    {"seashell", {255, 245, 238}},
    {"sienna", {160, 82, 45}},
    {"silver", {192, 192, 192}},
    {"skyblue", {135, 206, 235}},
    {"slateblue", {106, 90, 205}},
    {"slategray", {112, 128, 144}},
    {"slategrey", {112, 128, 144}},
    {"snow", {255, 250, 250}},
    {"springgreen", {0, 255, 127}},
    {"steelblue", {70, 130, 180}},
    {"tan", {210, 180, 140}},
    {"teal", {0, 128, 128}},
    {"thistle", {216, 191, 216}},
    {"tomato", {255, 99, 71}},
    {"turquoise", {64, 224, 208}},
    {"violet", {238, 130, 238}},
    {"wheat", {245, 222, 179}},
    {"white", {255, 255, 255}},
    {"whitesmoke", {245, 245, 245}},
    {"yellow", {255, 255, 0}},
    {"yellowgreen", {154, 205, 5}},
};


// Based on https://github.com/deanm/css-color-parser-js
nonstd::optional<sf::Color> parse_colour(std::string from) {
	if(from.empty())
		return nonstd::nullopt;

	from.erase(std::remove_if(from.begin(), from.end(), [](auto c) { return std::isspace(c); }), from.end());
	std::transform(from.begin(), from.end(), from.begin(), [](auto c) { return std::tolower(c); });

	const auto keyword_itr = keywords.find(from);
	if(keyword_itr != keywords.end())
		return nonstd::make_optional(keyword_itr->second);

	// #abc and #abc123 syntax.
	if(from[0] == '#') {
		const auto iv = std::strtoll(from.c_str() + 1, nullptr, 16);
		if(from.size() == 4) {
			if(iv < 0 || iv > 0xfff)
				return nonstd::nullopt;  // Covers NaN.
			return {{static_cast<std::uint8_t>(((iv & 0xf00) >> 4) | ((iv & 0xf00) >> 8)), static_cast<std::uint8_t>((iv & 0xf0) | ((iv & 0xf0) >> 4)),
			         static_cast<std::uint8_t>((iv & 0xf) | ((iv & 0xf) << 4)), 255}};
		} else if(from.size() == 7) {
			if(iv < 0 || iv > 0xffffff)
				return nonstd::nullopt;  // Covers NaN.
			return {{static_cast<std::uint8_t>((iv & 0xff0000) >> 16), static_cast<std::uint8_t>((iv & 0xff00) >> 8), static_cast<std::uint8_t>(iv & 0xff), 255}};
		}

		return nonstd::nullopt;
	}

	auto parsed = parse_function_notation(std::move(from));
	if(parsed) {
		auto alpha = 1.f;
		if(parsed->first.size() == 4 && parsed->first[3] == 'a') {
			if(parsed->second.size() != 4)
				return nonstd::nullopt;
			alpha = parse_css3_float(parsed->second.back());
			parsed->second.pop_back();
			parsed->first.pop_back();
		}
		if(parsed->second.size() != 3)
			return nonstd::nullopt;

		if(parsed->first == "rgb")
			return {
			    {parse_css3_int(parsed->second[0]), parse_css3_int(parsed->second[1]), parse_css3_int(parsed->second[2]), static_cast<std::uint8_t>(alpha * 255)}};
		else if(parsed->first == "hsl") {
			const auto h  = std::fmod(std::fmod(std::strtod(parsed->second[0].c_str(), nullptr), 360.) + 360., 360.) / 360.;  // 0 .. 1
			const auto s  = parse_css3_float(parsed->second[1]);
			const auto l  = parse_css3_float(parsed->second[2]);
			const auto m2 = l <= 0.5 ? l * (s + 1) : l + s - l * s;
			const auto m1 = l * 2 - m2;
			return {{static_cast<std::uint8_t>(std::min(1., std::max(0., css3_hue_to_rgb(m1, m2, h + 1 / 3))) * 255),
			         static_cast<std::uint8_t>(std::min(1., std::max(0., css3_hue_to_rgb(m1, m2, h))) * 255),
			         static_cast<std::uint8_t>(std::min(1., std::max(0., css3_hue_to_rgb(m1, m2, h - 1 / 3))) * 255), static_cast<std::uint8_t>(alpha * 255)}};
		}
	}

	return nonstd::nullopt;
}

nonstd::optional<std::pair<std::string, std::vector<std::string>>> parse_function_notation(std::string from) {
	const auto lparen = from.find('(');
	const auto rparen = from.find(')');

	if(lparen == 0)
		return nonstd::nullopt;
	else if(lparen != std::string::npos && rparen != std::string::npos && rparen > lparen) {
		auto func = from.substr(0, lparen);

		std::vector<std::string> params;
		for(auto tkn = std::strtok(&from[0] + lparen + 1, ","); tkn; tkn = std::strtok(NULL, ","))
			params.emplace_back(tkn);

		return {{func, params}};
	} else
		return {{from.substr(0, lparen), {}}};
}


static std::uint8_t parse_css3_int(const std::string & str) {
	long long ll;
	if(str[str.size() - 1] == '%')
		ll = std::strtod(str.c_str(), nullptr) / 100. * 255;
	else
		ll = std::strtoll(str.c_str(), nullptr, 10);
	return std::min(255ll, std::max(0ll, ll));
}

static double parse_css3_float(const std::string & str) {
	auto f = std::strtod(str.c_str(), nullptr);
	if(str[str.size() - 1] == '%')
		f /= 100.;
	return std::min(1., std::max(0., f));
}

static double css3_hue_to_rgb(double m1, double m2, double h) {
	if(h < 0)
		h += 1;
	else if(h > 1)
		h -= 1;

	if(h * 6 < 1)
		return m1 + (m2 - m1) * h * 6;
	if(h * 2 < 1)
		return m2;
	if(h * 3 < 2)
		return m1 + (m2 - m1) * (2 / 3 - h) * 6;

	return m1;
}
