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


LDDLLS := $(OS_LD_LIBS) fmt whereami++
LDAR := $(LNCXXAR) $(foreach l,fmt SFML/lib whereami-cpp,-L$(BLDDIR)$(l)) $(foreach dll,$(LDDLLS),-l$(dll))
INCAR := $(foreach l,$(foreach l,cereal whereami-cpp,$(l)/include),-isystemext/$(l)) $(foreach l,fmt SFML,-isystem$(BLDDIR)$(l)/include)
VERAR := $(foreach l,CONTROLLER_DISPLAY CEREAL WHEREAMI_CPP,-D$(l)_VERSION='$($(l)_VERSION)')
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))
HEADERS := $(sort $(wildcard src/*.hpp src/**/*.hpp src/**/**/*.hpp src/**/**/**/*.hpp))

.PHONY : all clean assets exe fmt sfml whereami-cpp


all : assets fmt sfml whereami-cpp exe

clean :
	rm -rf $(OUTDIR)

assets :
	@rm -rf $(OUTDIR)assets
	@cp -r $(ASSETDIR) $(OUTDIR)

exe : fmt whereami-cpp $(OUTDIR)controller-display$(EXE)
fmt : $(BLDDIR)fmt/libfmt$(ARCH) $(BLDDIR)fmt/include/fmt/format.h
sfml : $(BLDDIR)SFML/lib/libsfml-system-s$(ARCH)
whereami-cpp : $(BLDDIR)whereami-cpp/libwhereami++$(ARCH)


$(OUTDIR)controller-display$(EXE) : $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,$(OBJ),$(SOURCES))) $(OS_OBJS)
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) $(LDAR) $(shell grep '<SFML/' $(HEADERS) $(SOURCES) | sed -r 's:.*#include <SFML/(.*).hpp>:-lsfml-\1-s:' | tr '[:upper:]' '[:lower:]' | sort | uniq)
# '

$(BLDDIR)fmt/libfmt$(ARCH) : $(patsubst ext/fmt/fmt/%.cc,$(BLDDIR)fmt/obj/%$(OBJ),$(wildcard ext/fmt/fmt/*.cc))
	$(AR) crs $@ $^

$(BLDDIR)fmt/include/fmt/format.h : $(wildcard ext/fmt/fmt/*.h)
	@mkdir -p $(dir $@)
	cp $^ $(dir $@)

$(BLDDIR)SFML/lib/libsfml-system-s$(ARCH) : ext/SFML/CMakeLists.txt
	@mkdir -p $(abspath $(dir $@)../build)
	cd $(abspath $(dir $@)../build) && $(INCCMAKEAR) $(LNCMAKEAR) $(CMAKE) -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX:PATH="$(abspath $(dir $@)..)" $(abspath $(dir $^)) -GNinja
	cd $(abspath $(dir $@)../build) && $(NINJA) install

$(BLDDIR)whereami-cpp/libwhereami++$(ARCH) : ext/whereami-cpp/Makefile
	$(MAKE) -C$(dir $^) BUILD=$(abspath $(dir $@)) stlib

$(OBJDIR)controller-display$(OBJ) : controller-display.rc
	@mkdir -p $(dir $@)
	(echo '#define CONTROLLER_DISPLAY_VERSION_TEXT $(CONTROLLER_DISPLAY_VERSION)' && cat $^) | windres -DCONTROLLER_DISPLAY_VERSION_RAW='"$(shell echo $(CONTROLLER_DISPLAY_VERSION) | sed s/\\./,/g),0"' -o$@


$(OBJDIR)%$(OBJ) : $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) $(VERAR) -DSFML_STATIC -c -o$@ $^

$(BLDDIR)fmt/obj/%$(OBJ) : ext/fmt/fmt/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) -Iext/fmt -c -o$@ $^
