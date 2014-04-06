#ifndef _TESTSDL_CAMERA_HXX
#define _TESTSDL_CAMERA_HXX

extern "C" {
#include <SDL.h>
}







class camera {
public:
	camera(int levelWidth, int levelHeight, int displayWidth, int displayHeight);
	void clear();
	void considerObject(int x, int y, int clearRadius);
	void transform(SDL_Rect *r);
	virtual ~camera();
private:
	// minimum game area to display
	static const int width_min;
	static const int height_min;

	// display
	double aspect;
	int displayHeight;

	// in game units
	int levelWidth;
	int levelHeight;

	// again, in game units
	int leftMost;
	int rightMost;
	int topMost;
	int bottMost;

	// the transformation to apply
	double scale;
	double offsetX;
	double offsetY;
};

#endif 
