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


LDDLLS := $(OS_LD_LIBS) assets fmt whereami++
LDAR := $(LNCXXAR) $(foreach l,$(OBJDIR)assets $(foreach l,fmt SFML/lib whereami-cpp,$(BLDDIR)$(l)),-L$(l)) $(foreach dll,$(LDDLLS),-l$(dll))
INCAR := $(foreach l,$(foreach l,cereal whereami-cpp,$(l)/include),-isystemext/$(l)) $(foreach l,fmt SFML,-isystem$(BLDDIR)$(l)/include) -I$(BLDDIR)include
VERAR := $(foreach l,CONTROLLER_DISPLAY CEREAL WHEREAMI_CPP,-D$(l)_VERSION='$($(l)_VERSION)')
ASSETS := $(foreach l,$(sort $(wildcard $(ASSETDIR)* $(ASSETDIR)**/* $(ASSETDIR)**/**/* $(ASSETDIR)**/**/**/*)),$(if $(findstring directory,$(shell file $(l))),,$(l)))
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))
HEADERS := $(sort $(wildcard src/*.hpp src/**/*.hpp src/**/**/*.hpp src/**/**/**/*.hpp))

.PHONY : all clean assets exe fmt sfml whereami-cpp
.SECONDARY :


all : assets fmt sfml whereami-cpp exe

clean :
	rm -rf $(OUTDIR)

assets : $(BLDDIR)include/assets.hpp $(OBJDIR)assets/libassets$(ARCH)
exe : fmt whereami-cpp $(OUTDIR)controller-display$(EXE)
fmt : $(BLDDIR)fmt/libfmt$(ARCH) $(BLDDIR)fmt/include/fmt/format.h
sfml : $(BLDDIR)SFML/lib/libsfml-system-s$(ARCH)
whereami-cpp : $(BLDDIR)whereami-cpp/libwhereami++$(ARCH)


$(OUTDIR)controller-display$(EXE) : $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,$(OBJ),$(SOURCES))) $(OS_OBJS)
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) -lsfml-window-s -lsfml-graphics-s -lsfml-system-s $(shell echo $(patsubst $(BLDDIR)SFML/lib/lib%.a,-l%,$(wildcard $(BLDDIR)SFML/lib/*$(ARCH))) | sed "s/ /\\n/g" | grep -v sfml) $(LDAR)

$(BLDDIR)include/assets.hpp : $(ASSETS)
	@mkdir -p $(dir $@)
	echo "#pragma once" > $@
	echo "namespace assets {" >> $@
	$(foreach l,$^,echo "	extern const unsigned char $(shell echo $(subst $(ASSETDIR),,$(l)) | sed 's/[^[:alnum:]]/_/g')[$(word 1,$(shell wc -c $(l)))];" >> $@;)
	echo "" >> $@
	$(foreach l,$^,echo "	extern const char $(shell echo $(subst $(ASSETDIR),,$(l)) | sed 's/[^[:alnum:]]/_/g')_s[$(shell expr $(word 1,$(shell wc -c $(l))) + 1)];" >> $@;)
	echo "}" >> $@

$(OBJDIR)assets/libassets$(ARCH) : $(patsubst $(ASSETDIR)%,$(OBJDIR)assets/%$(OBJ),$(ASSETS)) $(patsubst $(ASSETDIR)%,$(OBJDIR)assets/%_s$(OBJ),$(ASSETS))
	@mkdir -p $(dir $@)
	$(AR) crs $@ $^

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

$(BLDDIR)whereami-cpp/libwhereami++$(ARCH) : ext/whereami-cpp/Makefile
	$(MAKE) -C$(dir $^) BUILD=$(abspath $(dir $@)) stlib

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
	$(CXX) $(CXXAR) -I$(BLDDIR)include -c -o$@ $^

$(BLDDIR)fmt/obj/%$(OBJ) : ext/fmt/fmt/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) -Iext/fmt -c -o$@ $^
