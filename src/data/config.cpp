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


#include "config.hpp"
#include "container.hpp"
#include <SFML/System.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <fmt/format.h>
#include <fstream>
#include <nonstd/optional.hpp>
#include <sstream>
#include <tinyfiledialogs.h>


using namespace std::literals;


namespace config_subcategories {
	struct display {
		bool & vsync;
		unsigned int & FPS;

		template <class Archive>
		void serialize(Archive & archive) {
			archive(cereal::make_nvp("FPS", FPS), cereal::make_nvp("vsync", vsync));
		}
	};

	struct cosmetics {
		float & new_layout_time;

		template <class Archive>
		void serialize(Archive & archive) {
			archive(cereal::make_nvp("new_layout_time", new_layout_time));
		}
	};

	struct dependencies {
		template <class Archive>
		void save(Archive & archive) const {
			std::ostringstream fmt, SFML;
			fmt << "fmt version " << (FMT_VERSION - (FMT_VERSION % 10000)) / 10000 << '.' << ((FMT_VERSION - (FMT_VERSION % 100)) / 100) % 100 << '.'
			    << FMT_VERSION % 100 << ", found at http://fmtlib.net";
			SFML << "SFML version " << SFML_VERSION_MAJOR << '.' << SFML_VERSION_MINOR << '.' << SFML_VERSION_PATCH << " found at https://sfml-dev.org";
			archive(cereal::make_nvp("cereal", "version "s + CEREAL_VERSION + ", found at https://uscilab.github.io/cereal"), cereal::make_nvp("fmt", fmt.str()),
			        cereal::make_nvp("fmt", fmt.str()),
			        cereal::make_nvp("optional-lite", "version "s + optional_lite_VERSION + ", found at https://github.com/martinmoene/optional-lite"),
			        cereal::make_nvp("SFML", SFML.str()),
			        cereal::make_nvp("tinyfiledialogs", "version "s + tinyfd_version + ", found at https://sourceforge.net/p/tinyfiledialogs"),
			        cereal::make_nvp("variant-lite", "commit "s + VARIANT_LITE_VERSION + ", found at https://github.com/martinmoene/variant-lite"),
			        cereal::make_nvp("whereami-cpp", "version "s + WHEREAMI_CPP_VERSION + ", found at https://github.com/nabijaczleweli/whereami-cpp"),
			        cereal::make_nvp("yaml-cpp", "commit "s + YAML_CPP_VERSION + ", found at https://github.com/jbeder/yaml-cpp"));
		}

		template <class Archive>
		void load(Archive &) {}
	};
}

template <class Archive>
void serialize(Archive & archive, config & cc) {
	archive(
	    cereal::make_nvp("display", config_subcategories::display{cc.vsync, cc.FPS}),
	    cereal::make_nvp("cosmetics", config_subcategories::cosmetics{cc.new_layout_time}),
	    cereal::make_nvp("‌controller-display", "version "s + CONTROLLER_DISPLAY_VERSION + ", found at https://github.com/nabijaczleweli/controller-display"),
	    cereal::make_nvp("‌‌dependencies", config_subcategories::dependencies{}));
}


config::config(std::string && ppath) : path(move(ppath)) {
	std::ifstream configfile(path);
	if(configfile.is_open())
		try {
			cereal::JSONInputArchive{configfile}(*this);
		} catch(...) {
		}

	if(!FPS)
		FPS = 60;
}

config::~config() {
	std::ofstream configfile(path);
	cereal::JSONOutputArchive{configfile}(cereal::make_nvp(app_name + " configuration", *this));
}
