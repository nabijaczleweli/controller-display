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


#include "dialog.hpp"
#include <cstring>
#include <iterator>
#include <string>
#include <tinyfiledialogs.h>


nonstd::optional<sf::String> pick_file_dialog(const char * exts_name, const std::vector<const char *> & exts) {
#ifdef _WIN32
	tinyfd_winUtf8 = 1;
#endif

	if(const auto res = tinyfd_openFileDialog(nullptr, nullptr, 1, exts.data(), exts_name, false)) {
		std::basic_string<sf::Uint32> buf;
		sf::Utf8::toUtf32(res, res + std::strlen(res), std::back_inserter(buf));
		return nonstd::optional<sf::String>(nonstd::in_place, buf);
	} else
		return nonstd::nullopt;
}
