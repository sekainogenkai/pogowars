.POSIX:
.PHONY: all
.SUFFIXES: .cxx .o

SDL2_PKG = sdl2 SDL2_image

LIBS = $$(pkg-config --libs $(SDL2_PKG))
MY_CXXFLAGS = -Wall $$(pkg-config --cflags $(SDL2_PKG))
EXEEXT = .exe
OBJ = main.o

all: testsdl$(EXEEXT)
testsdl$(EXEEXT): $(OBJ)
	$(CXX) $(MY_CXXFLAGS) $(CXXFLAGS) -o '$(@)' $(OBJ) $(LIBS)

.cxx.o:
	$(CXX) -c $(MY_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o '$(@)' '$(<)'

$(OBJ): Makefile
