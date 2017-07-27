// The MIT License (MIT)

// Copyright (c) 2015 nabijaczleweli

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


#include "bezier_curve.hpp"
#include <algorithm>
#include <cmath>
#include <functional>


float bezier_curve::base_points    = 10000.f;
bool bezier_curve::optimize_points = true;


bezier_curve::bezier_curve(sf::Vector2f the_start_point, sf::Vector2f the_control_point, sf::Vector2f the_end_point)
      : point_colour(sf::Color::White), fill(false), start_point(the_start_point), control_point(the_control_point), end_point(the_end_point) {
	compute_vertices();
}

void bezier_curve::swap(bezier_curve & with) {
	std::swap(vertices, with.vertices);
	std::swap(point_colour, with.point_colour);
	std::swap(fill, with.fill);
	std::swap(start_point, with.start_point);
	std::swap(control_point, with.control_point);
	std::swap(end_point, with.end_point);
}

void bezier_curve::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(vertices.data(), vertices.size(), fill ? sf::PrimitiveType::TrianglesFan : sf::PrimitiveType::LinesStrip, states);
}

void bezier_curve::compute_vertices() {
	vertices.clear();
	const auto step = 1. / base_points;
	for(auto t = 0.f; t <= 1.; t += step)
		vertices.emplace_back((1 - t) * (1 - t) * start_point + 2 * (1 - t) * t * control_point + t * t * end_point, point_colour);

	if(optimize_points)
		vertices.erase(unique(vertices.begin(), vertices.end(),
		                      [&](auto && lhs, auto && rhs) {
			                      return std::round(lhs.position.x) == std::round(rhs.position.x) && std::round(lhs.position.y) == std::round(rhs.position.y);
			                    }),
		               vertices.end());
	vertices.shrink_to_fit();
}

const sf::Color & bezier_curve::colour() const {
	return point_colour;
}

void bezier_curve::colour(sf::Color new_colour) {
	if(point_colour != new_colour) {
		point_colour = std::move(new_colour);
		std::for_each(vertices.begin(), vertices.end(), [&](auto && v) { v.color = point_colour; });
	}
}

sf::FloatRect bezier_curve::local_bounds() const {
	const auto x = std::minmax_element(vertices.begin(), vertices.end(), [](auto && lhs, auto && rhs) { return lhs.position.x < rhs.position.x; });
	const auto y = std::minmax_element(vertices.begin(), vertices.end(), [](auto && lhs, auto && rhs) { return lhs.position.y < rhs.position.y; });
	return {x.first->position.x, y.first->position.y, x.second->position.x - x.first->position.x, y.second->position.y - y.first->position.y};
}
