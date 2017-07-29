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


#pragma once


#include "screens/screen.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <nonstd/optional.hpp>
#include <memory>


class application {
private:
	friend screen;
	friend class help_screen;
	friend class main_app_screen;

	std::unique_ptr<screen> current_screen;
	std::unique_ptr<screen> temp_screen;

	nonstd::optional<sf::Vector2u> new_size;

	sf::RenderWindow window;

	int loop();
	int draw();


public:
	static unsigned int effective_FPS();


	int run();
	void resize(sf::Vector2u to);

	template <class T, class... A>
	inline void schedule_screen(A &&... args) {
		temp_screen = std::make_unique<T>(*this, std::forward<A>(args)...);
	}
};
