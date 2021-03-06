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
#include "assets.hpp"
#include "string.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>


static std::uint8_t parse_css3_int(const std::string & str);
static double parse_css3_float(const std::string & str);
static double css3_hue_to_rgb(double m1, double m2, double h);


// Based on https://github.com/deanm/css-color-parser-js
nonstd::optional<sf::Color> parse_colour(std::string from) {
	if(from.empty())
		return nonstd::nullopt;

	from.erase(std::remove_if(from.begin(), from.end(), [](auto c) { return std::isspace(c); }), from.end());
	std::transform(from.begin(), from.end(), from.begin(), [](auto c) { return std::tolower(c); });

	const auto keyword_itr = assets::css3_keywords.find(from);
	if(keyword_itr != assets::css3_keywords.end())
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
		from.erase(rparen);
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
