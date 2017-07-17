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


#include "application.hpp"
#include "../data/container.hpp"
#include "../util/monitor.hpp"
#include <SFML/System.hpp>
#include <algorithm>
#include <thread>


unsigned int application::effective_FPS() {
	static const auto vsync_fps = refresh_rate();
	return app_configuration.vsync ? vsync_fps : app_configuration.FPS;
}

int application::run() {
	window.create({600, 300}, app_name, sf::Style::None);
	if(app_configuration.vsync)
		window.setVerticalSyncEnabled(true);
	else
		window.setFramerateLimit(app_configuration.FPS);

	//{
	//	sf::Image icon;
	//	if(icon.loadFromFile(textures_root + "/gui/general/window_main.png"))
	//		window.setIcon(icon.getSize().x, icon.getSize().x, icon.getPixelsPtr());
	//}

	return loop();
}

int application::loop() {
	while(window.isOpen()) {
		while(temp_screen) {
			current_screen = move(temp_screen);
			current_screen->setup();
		}

		if(const int i = current_screen->loop())
			return i;

		if(const int i = draw())
			return i;

		sf::Event event;
		while(window.pollEvent(event))
			if(const int i = current_screen->handle_event(event))
				return i;
	}
	return 0;
}

int application::draw() {
	window.clear(sf::Color::Black);

	if(const int i = current_screen->draw())
		return i;

	window.display();
	return 0;
}