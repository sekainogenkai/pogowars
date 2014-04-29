.POSIX:
.PHONY: all clean
.SUFFIXES: .cxx .o .xcf .png .svg

SDL2_PKG = sdl2 SDL2_image SDL2_ttf

LIBS = $$(pkg-config --libs $(SDL2_PKG))
MY_CXXFLAGS = -Wall $$(pkg-config --cflags $(SDL2_PKG))
EXEEXT = .exe
OBJ = camera.o main.o game_mode.o menu_game_mode.o one_player_game_mode.o
RASTERS = \
	menuBack.png \
	startGame.png \
	settings.png \
	credits.png \
	quit.png \
	menuRod.png \
	menuGear.png \
	lightForeground.png \
	menuP.png \
	shade.png \
	startGame_clickToJoin.png \
	startGame_blinkingLights.png \
	keyboardInstructions.png \
	inCredits.png \
	redJoin.png \
	blueJoin.png \
	READY.png \
	READY2.png \
	BACK.png \
	BACK2.png \
	inQuit.png \
	NAMES_MAPS.png \
	THUMB_SkyBridge.png \
	THUMB_SmokeRoom.png \
	inMapChoose.png \
	greenDown.png \
	greenUp.png \
	smokeRoom.png \
	smoke_Room_Foreground.png \
	smoke_Room_Furnace.png \
	redBody.png \
	blueBody.png \
	

	

MY_GIMP = $${GIMP-gimp}

all: testsdl$(EXEEXT) $(RASTERS)
testsdl$(EXEEXT): $(OBJ)
	$(CXX) $(MY_CXXFLAGS) $(CXXFLAGS) -o '$(@)' $(OBJ) $(LIBS)

.cxx.o:
	$(CXX) -c $(MY_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o '$(@)' '$(<)'
.xcf.png:
	$(MY_GIMP) -i -b '(let* ((image (car (gimp-file-load RUN-NONINTERACTIVE "$(<)" "$(<)")))) (file-png-save-defaults RUN-NONINTERACTIVE image (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)) "$(@)" "$(@)"))' -b '(gimp-quit FALSE)'
.svg.png:
	inkscape -e '$(@)' -d 120 '$(<)'

$(OBJ) $(RASTERS): Makefile

clean:
	rm -f $(OBJ) $(RASTERS) testsdl$(EXEEXT)
