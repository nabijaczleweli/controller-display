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


#include "string.hpp"
#include <algorithm>
#include <cctype>
#include <locale>


bool string_eq_caseless(std::string lhs, std::string rhs) {
	std::transform(lhs.begin(), lhs.end(), lhs.begin(), [](auto c) { return std::tolower(c); });
	std::transform(rhs.begin(), rhs.end(), rhs.begin(), [](auto c) { return std::tolower(c); });
	return lhs == rhs;
}


// Stolen from http://stackoverflow.com/a/217605/2851815
std::string ltrim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](auto c) { return !std::isspace(c); }));
	return s;
}

// Stolen from http://stackoverflow.com/a/217605/2851815
std::string rtrim(std::string s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](auto c) { return !std::isspace(c); }).base(), s.end());
	return s;
}

// Stolen from http://stackoverflow.com/a/217605/2851815
std::string trim(std::string s) {
	return ltrim(std::move(rtrim(std::move(s))));
}
