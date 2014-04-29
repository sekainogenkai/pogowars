.POSIX:
.PHONY: all clean
.SUFFIXES: .cxx .o .xcf .png .svg .rc .res

-include windurrs.mk
SDL2_PKG = sdl2 SDL2_image SDL2_ttf

LIBS = $$(pkg-config --libs $(SDL2_PKG))
MY_CXXFLAGS = -Wall $$(pkg-config --cflags $(SDL2_PKG))
EXEEXT = .exe
OBJ = \
	src/camera.o \
	src/main.o \
	src/game_mode.o \
	src/menu_game_mode.o \
	src/one_player_game_mode.o

RASTERS = \
	images/favicon.png \
	images/menuBack.png \
	images/startGame.png \
	images/settings.png \
	images/credits.png \
	images/quit.png \
	images/menuRod.png \
	images/menuGear.png \
	images/lightForeground.png \
	images/menuP.png \
	images/shade.png \
	images/startGame_clickToJoin.png \
	images/startGame_blinkingLights.png \
	images/keyboardInstructions.png \
	images/inCredits.png \
	images/redJoin.png \
	images/blueJoin.png \
	images/READY.png \
	images/READY2.png \
	images/BACK.png \
	images/BACK2.png \
	images/inQuit.png \
	images/NAMES_MAPS.png \
	images/THUMB_SkyBridge.png \
	images/THUMB_SmokeRoom.png \
	images/inMapChoose.png \
	images/greenDown.png \
	images/greenUp.png \
	images/smokeRoom.png \
	images/smoke_Room_Foreground.png \
	images/smoke_Room_Furnace.png \
	images/redBody.png \
	images/blueBody.png \
	

MY_GIMP = $${GIMP-gimp}

all: testsdl$(EXEEXT) $(RASTERS)
testsdl$(EXEEXT): $(OBJ) $(RES) windurrs.mk
	$(CXX) $(MY_CXXFLAGS) $(CXXFLAGS) -o '$(@)' $(OBJ) $(RES) $(LIBS)

windurrs.mk: Makefile
	echo > windurrs.mk
	n="$$(which ls)"; test "$${n##*ls}" = ".exe" && echo 'RES = main.res' >> windurrs.mk

.cxx.o:
	$(CXX) -c $(MY_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o '$(@)' '$(<)'
.xcf.png:
	$(MY_GIMP) -i -b '(let* ((image (car (gimp-file-load RUN-NONINTERACTIVE "$(<)" "$(<)")))) (file-png-save-defaults RUN-NONINTERACTIVE image (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)) "$(@)" "$(@)"))' -b '(gimp-quit FALSE)'
.svg.png:
	inkscape -e '$(@)' -d 120 '$(<)'
.rc.res:
	windres '$(<)' -O coff -o '$(@)'

$(OBJ) $(RASTERS): Makefile

clean:
	rm -f $(OBJ) $(RASTERS) $(RES) testsdl$(EXEEXT)

