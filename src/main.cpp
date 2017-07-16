// The MIT License (MIT)

// Copyright (c) 2016 nabijaczleweli

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


#include <SFML/System.hpp>
#include <fmt/format.h>
#include <iostream>
#include <whereami++.hpp>


int main() {
	sf::err() << "Config file location: \"" << whereami::executable_dir() << "/controller-display.toml\"\n\n"
	          << "cereal version " << CEREAL_VERSION << "\nfmt version " << (FMT_VERSION - (FMT_VERSION % 10000)) / 10000 << '.'
	          << ((FMT_VERSION - (FMT_VERSION % 100)) / 100) % 100 << '.' << FMT_VERSION % 100 << "\nSFML version " << SFML_VERSION_MAJOR << '.'
	          << SFML_VERSION_MINOR << '.' << SFML_VERSION_PATCH << "\nwhereami-cpp version " << WHEREAMI_CPP_VERSION << "\n";
}
