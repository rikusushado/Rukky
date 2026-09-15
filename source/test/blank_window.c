#include <rukky/rukky.h>

int main(int argc, char** argv)
{
	RukkyWindow window;
	bool success = RukkyNewWindow(
	    &window,
	    0, 0,
	    144, 144,
	    0,
	    "Rukky | Blank Window Test"
	);

	if (!success) return -1;

	RukkyApplicationLoop(
		&window,
		60,
		0,
		NULL,
		-1
	);

	return 0;
}

