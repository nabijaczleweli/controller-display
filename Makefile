# The MIT License (MIT)

# Copyright (c) 2014 nabijaczleweli

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


include configMakefile


LDDLLS := assets fmt tinyfiledialogs whereami++ yaml-cpp $(OS_LD_LIBS)
LDAR := $(OS_LD_ARGS) $(LNCXXAR) $(foreach l,$(OBJDIR)assets $(foreach l,fmt SFML/lib tinyfiledialogs whereami-cpp yaml-cpp,$(BLDDIR)$(l)),-L$(l)) $(foreach dll,$(LDDLLS),-l$(dll))
INCAR := $(foreach l,$(foreach l,cereal optional-lite whereami-cpp yaml-cpp,$(l)/include),-isystemext/$(l)) $(foreach l,fmt SFML tinyfiledialogs variant-lite,-isystem$(BLDDIR)$(l)/include) -I$(BLDDIR)include
VERAR := $(foreach l,CONTROLLER_DISPLAY CEREAL CSS_COLOR_PARSER_JS VARIANT_LITE WHEREAMI_CPP YAML_CPP,-D$(l)_VERSION='$($(l)_VERSION)')
ASSETS := $(foreach l,$(sort $(wildcard $(ASSETDIR)* $(ASSETDIR)**/* $(ASSETDIR)**/**/* $(ASSETDIR)**/**/**/*)),$(if $(findstring directory,$(shell file $(l))),,$(l)))
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))
HEADERS := $(sort $(wildcard src/*.hpp src/**/*.hpp src/**/**/*.hpp src/**/**/**/*.hpp))

.PHONY : all clean assets exe fmt sfml tinyfiledialogs variant-lite whereami-cpp yaml-cpp
.SECONDARY :


all : fmt sfml tinyfiledialogs variant-lite whereami-cpp yaml-cpp assets exe

clean :
	rm -rf $(OUTDIR)

assets : $(BLDDIR)include/assets.hpp $(OBJDIR)assets/libassets$(ARCH)
exe : assets fmt tinyfiledialogs variant-lite sfml whereami-cpp yaml-cpp $(OUTDIR)controller-display$(EXE)
fmt : $(BLDDIR)fmt/libfmt$(ARCH) $(BLDDIR)fmt/include/fmt/format.h
sfml : $(BLDDIR)SFML/lib/libsfml-system-s$(ARCH)
tinyfiledialogs : $(BLDDIR)tinyfiledialogs/include/tinyfiledialogs.h $(BLDDIR)tinyfiledialogs/libtinyfiledialogs$(ARCH)
variant-lite : $(BLDDIR)variant-lite/include/nonstd/variant.hpp
whereami-cpp : $(BLDDIR)whereami-cpp/libwhereami++$(ARCH)
yaml-cpp : $(BLDDIR)yaml-cpp/libyaml-cpp$(ARCH)


$(OUTDIR)controller-display$(EXE) : $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,$(OBJ),$(SOURCES))) $(OS_OBJS)
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) -lsfml-window-s -lsfml-graphics-s -lsfml-system-s $(shell echo $(patsubst $(BLDDIR)SFML/lib/lib%.a,-l%,$(wildcard $(BLDDIR)SFML/lib/*$(ARCH))) | sed "s/ /\\n/g" | grep -v sfml) $(LDAR)

$(BLDDIR)include/assets.hpp : $(ASSETS)
	@mkdir -p $(dir $@)
	echo "#pragma once" > $@
	echo "#include <string>" >> $@
	echo "#include <unordered_map>" >> $@
	echo "#include <SFML/Graphics.hpp>" >> $@
	echo "" >> $@
	echo "namespace assets {" >> $@
	$(foreach l,$^,echo "	extern const unsigned char $(shell echo $(subst $(ASSETDIR),,$(l)) | sed 's/[^[:alnum:]]/_/g')[$(word 1,$(shell wc -c $(l)))];" >> $@;)
	echo "" >> $@
	$(foreach l,$^,echo "	extern const char $(shell echo $(subst $(ASSETDIR),,$(l)) | sed 's/[^[:alnum:]]/_/g')_s[$(shell expr $(word 1,$(shell wc -c $(l))) + 1)];" >> $@;)
	echo "" >> $@
	echo "	extern const std::unordered_map<std::string, sf::Color> css3_keywords;" >> $@
	echo "}" >> $@

$(OBJDIR)assets/libassets$(ARCH) : $(patsubst $(ASSETDIR)%,$(OBJDIR)assets/%$(OBJ),$(ASSETS)) $(patsubst $(ASSETDIR)%,$(OBJDIR)assets/%_s$(OBJ),$(ASSETS)) $(BLDDIR)css-color-parser-js/keywords$(OBJ)
	@mkdir -p $(dir $@)
	$(AR) crs $@ $^

$(BLDDIR)css-color-parser-js/keywords.cpp : ext/css-color-parser-js/csscolorparser.js
	@mkdir -p $(dir $@)
	echo "#include \"assets.hpp\"" > $@
	echo "const std::unordered_map<std::string, sf::Color> assets::css3_keywords{" >> $@
	sed -e 's/, "/,\n  "/g' -e 's/]}/],\n}/g' -e 's/[]:[,]/ /g' $^ | awk '/CSSColorTable.*=/,/}/' | tail -n +2 | head -n -1 | awk '{print("\t{" $$1 ", {" $$2 ", " $$3 ", " $$4 ", " ($$5 * 255) "}}," )}' >> $@
	echo "};" >> $@

$(BLDDIR)fmt/libfmt$(ARCH) : $(patsubst ext/fmt/fmt/%.cc,$(BLDDIR)fmt/obj/%$(OBJ),$(wildcard ext/fmt/fmt/*.cc))
	@mkdir -p $(dir $@)
	$(AR) crs $@ $^

$(BLDDIR)fmt/include/fmt/format.h : $(wildcard ext/fmt/fmt/*.h)
	@mkdir -p $(dir $@)
	cp $^ $(dir $@)

$(BLDDIR)SFML/lib/libsfml-system-s$(ARCH) : ext/SFML/CMakeLists.txt
	@mkdir -p $(abspath $(dir $@)../build)
	cd $(abspath $(dir $@)../build) && $(INCCMAKEAR) $(LNCMAKEAR) $(CMAKE) -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX:PATH="$(abspath $(dir $@)..)" $(abspath $(dir $^)) -GNinja
	cd $(abspath $(dir $@)../build) && $(NINJA) install

$(BLDDIR)tinyfiledialogs/include/tinyfiledialogs.h : ext/tinyfiledialogs/tinyfiledialogs.h
	@mkdir -p $(dir $@)
	cp $^ $@

$(BLDDIR)tinyfiledialogs/libtinyfiledialogs$(ARCH) : $(BLDDIR)tinyfiledialogs/obj/tinyfiledialogs$(OBJ)
	@mkdir -p $(dir $@)
	$(AR) crs $@ $^

$(BLDDIR)variant-lite/include/nonstd/variant.hpp : ext/variant-lite/include/nonstd/variant.hpp
	@mkdir -p $(dir $@)
	cp $^ $@

$(BLDDIR)whereami-cpp/libwhereami++$(ARCH) : ext/whereami-cpp/Makefile
	$(MAKE) -C$(dir $^) BUILD=$(abspath $(dir $@)) stlib

$(BLDDIR)yaml-cpp/libyaml-cpp$(ARCH) : $(patsubst ext/yaml-cpp/src/%.cpp,$(BLDDIR)yaml-cpp/obj/%$(OBJ),$(wildcard ext/yaml-cpp/src/*.cpp))
	@mkdir -p $(dir $@)
	$(AR) crs $@ $^

$(OBJDIR)controller-display$(OBJ) : controller-display.rc
	@mkdir -p $(dir $@)
	(echo '#define CONTROLLER_DISPLAY_VERSION_TEXT $(CONTROLLER_DISPLAY_VERSION)' && cat $^) | windres -DCONTROLLER_DISPLAY_VERSION_RAW='"$(shell echo $(CONTROLLER_DISPLAY_VERSION) | sed s/\\./,/g),0"' -o$@


$(OBJDIR)%$(OBJ) : $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) $(VERAR) -DSFML_STATIC -c -o$@ $^

$(OBJDIR)assets/%.cpp : $(ASSETDIR)%
	@mkdir -p $(dir $@)
	echo "#include \"assets.hpp\"" > $@
	echo "const unsigned char assets::$(shell echo $(subst $(ASSETDIR),,$^) | sed 's/[^[:alnum:]]/_/g')[$(word 1,$(shell wc -c $^))] = {" >> $@
	od -An -v -tx1 $^ | perl -pe "s/\\n/,\\n/" | sed -e "s/ /0x/" -e "s/ /,0x/g" >> $@
	echo "};" >> $@

$(OBJDIR)assets/%_s.cpp : $(ASSETDIR)%
	@mkdir -p $(dir $@)
	echo "#include \"assets.hpp\"" > $@
	echo -n "const char assets::$(shell echo $(subst $(ASSETDIR),,$^) | sed 's/[^[:alnum:]]/_/g')_s[$(shell expr $(word 1,$(shell wc -c $^)) + 1)] = \"" >> $@
	od -An -v -tx1 $^ | tr -d "\n" | sed -e "s/ /\\\\x/g" >> $@
	echo "\";" >> $@

$(OBJDIR)assets/%$(OBJ) : $(OBJDIR)assets/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) -DSFML_STATIC -c -o$@ $^

$(BLDDIR)fmt/obj/%$(OBJ) : ext/fmt/fmt/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) -Iext/fmt -c -o$@ $^

$(BLDDIR)css-color-parser-js/%$(OBJ) : $(BLDDIR)css-color-parser-js/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) -DSFML_STATIC -c -o$@ $^

$(BLDDIR)tinyfiledialogs/obj/%$(OBJ) : ext/tinyfiledialogs/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCAR) -c -o$@ $^

$(BLDDIR)yaml-cpp/obj/%$(OBJ) : ext/yaml-cpp/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) -Iext/yaml-cpp/include -c -o$@ $^
