extern "C" {
#include <math.h>
#include <SDL.h>
}

#include "camera.hxx"

camera::camera(int levelWidth, int levelHeight, int displayWidth, int displayHeight)
: aspect((double)displayWidth/(double)displayHeight)
, displayHeight(displayHeight)
, levelWidth(levelWidth)
, levelHeight(levelHeight)
{
}

void camera::clear()
{
	leftMost = levelWidth;
	rightMost = 0;
	topMost = levelHeight;
	bottMost = 0;
}

const int camera::width_min = 1440/3; 
const int camera::height_min = 900/3; 

void camera::considerObject(int x, int y, int clearRadius)
{
	if (leftMost > x - clearRadius)
		leftMost = x - clearRadius;
	if (rightMost < x + clearRadius)
		rightMost = x + clearRadius;
	if (topMost > y - clearRadius)
		topMost = y - clearRadius;
	if (bottMost < y + clearRadius)
		bottMost = y + clearRadius;

	// Calculate the transformation stuff for fun even though we probably
	// don't need it yet. This assumes that considerObject() will be called
	// for a small number of objects to uncomplicate transform().

	// Respect minwidth/height
	int width_min_correction = width_min - (rightMost - leftMost);
	if (width_min_correction > 0)
	{
		leftMost -= width_min_correction / 2;
		rightMost += width_min_correction / 2;
	}
	int height_min_correction = height_min - (bottMost - topMost);
	if (height_min_correction > 0)
	{
		topMost -= height_min_correction / 2;
		bottMost += height_min_correction / 2;
	}

	// Fix aspect of the left/right/top/bottMost rectangle to match display aspect.
	// aspect_correction is the amount by which the width isn't wide enough
	int aspect_correction =
		aspect * (bottMost - topMost) // target width
		- (rightMost - leftMost); // actual object-encompassing width at the moment
	if (aspect_correction > 0)
	{
		rightMost += aspect_correction / 2;
		leftMost -= aspect_correction / 2 + aspect_correction % 2;
	}
	else
	{
		// apparently we're lacking in height instead.
		aspect_correction = (rightMost - leftMost) / aspect - (bottMost - topMost);
		bottMost += aspect_correction / 2;
		topMost -= aspect_correction / 2 + aspect_correction % 2;
	}

	scale = displayHeight / (double)(bottMost - topMost);
	offsetX = -leftMost;
	offsetY = -topMost;
}

void camera::transform(SDL_Rect *r)
{
	r->x = (r->x + offsetX) * scale;
	r->y = (r->y + offsetY) * scale;
	r->w = r->w * scale;
	r->h = r->h * scale;
}

camera::~camera()
{
}
