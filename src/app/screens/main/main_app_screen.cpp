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
	A_key.tick();
	__key.tick();
	at_key.tick();
	lbracket_key.tick();
	caret_key.tick();
	return 0;
}

int main_app_screen::draw() {
	app.window.draw(A_key);
	app.window.draw(__key);
	app.window.draw(at_key);
	app.window.draw(lbracket_key);
	app.window.draw(caret_key);
	return 0;
}

int main_app_screen::handle_event(const sf::Event & event) {
	return screen::handle_event(event);
}

main_app_screen::main_app_screen(application & theapp) : screen(theapp), A_key('A'), __key('_'), at_key('@'), lbracket_key('['), caret_key('^') {
	__key.setPosition(70, 0);
	at_key.setPosition(140, 0);
	lbracket_key.setPosition(210, 0);
	caret_key.setPosition(280, 0);
}
