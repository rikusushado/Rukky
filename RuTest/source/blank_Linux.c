#include <rukky/core.h>

int main(int argc, char** argv)
{
	int framebuffer_attributes[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        GLX_DOUBLEBUFFER,  True,
        None
    };

	uptr* window = NULL;
	bool success = RukkyNewWindow(
	    &window,
		framebuffer_attributes,
	    0, 0,
	    144, 144,
	    0,
	    "Rukky | Blank Window Test"
	);

	if (!success) return -1;

	RukkyApplicationLoop(
		&window,
		1000,
		0,
		NULL,
		-1
	);

	return 0;
}

