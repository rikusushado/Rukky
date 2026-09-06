#include <rukky/rukky.h>

int main(int argc, char** argv)
{
	RukkyWindow* window = NULL;
	RukkyNewWindow(
	    window,
	    0, 0,
	    144, 144,
	    0,
	    "Rukky | Blank Window Test"
	);

	if(!window) return -1;

	RukkyApplication* application = NULL;
	RukkyNewApplication(
		application,
		"Rukky Blank Window Test",
		"Rukky Core"
	);

	if (!application) return -1;

	RukkyApplicationLoop(
		application,
		window,
		60,
		0,
		NULL,
		-1
	);

	return 0;
}

