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
#include "../../application.hpp"


void main_app_screen::setup() {
	screen::setup();
}

int main_app_screen::loop() {
	for_each(keys.begin(), keys.end(), [](auto && k) { k.tick(); });
	return 0;
}

int main_app_screen::draw() {
	for_each(keys.begin(), keys.end(), [&](auto && k) { app.window.draw(k); });
	return 0;
}

int main_app_screen::handle_event(const sf::Event & event) {
	return screen::handle_event(event);
}

main_app_screen::main_app_screen(application & theapp) : screen(theapp), keys{key("a"), key("Num+"), key("Up"), key("~"), key("Control")} {
	keys[1].setPosition(70, 0);
	keys[2].setPosition(140, 0);
	keys[3].setPosition(210, 0);
	keys[4].setPosition(280, 0);
}