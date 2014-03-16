.POSIX:
.PHONY: all clean
.SUFFIXES: .cxx .o .xcf .png

SDL2_PKG = sdl2 SDL2_image SDL2_ttf

LIBS = $$(pkg-config --libs $(SDL2_PKG))
MY_CXXFLAGS = -Wall $$(pkg-config --cflags $(SDL2_PKG))
EXEEXT = .exe
OBJ = main.o game_mode.o menu_game_mode.o one_player_game_mode.o
RASTERS = \
	pakman.png \
	Angry_Chase.png \
	menuSelectBar.png \
	Map.png \
	playerOne.png \
	fear.png \
	anger.png \
	yinAndYang.png \
	Wall.png \
	menuBack.png \
	watermelon.png \
	

MY_GIMP = $${GIMP-gimp}

all: testsdl$(EXEEXT) $(RASTERS)
testsdl$(EXEEXT): $(OBJ)
	$(CXX) $(MY_CXXFLAGS) $(CXXFLAGS) -o '$(@)' $(OBJ) $(LIBS)

.cxx.o:
	$(CXX) -c $(MY_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o '$(@)' '$(<)'
.xcf.png:
	$(MY_GIMP) -i -b '(let* ((image (car (gimp-file-load RUN-NONINTERACTIVE "$(<)" "$(<)")))) (file-png-save-defaults RUN-NONINTERACTIVE image (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)) "$(@)" "$(@)"))' -b '(gimp-quit FALSE)'

$(OBJ) $(RASTERS): Makefile

clean:
	rm -f $(OBJ) $(RASTERS) testsdl$(EXEEXT)
