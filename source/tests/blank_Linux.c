#include <rukky/rukky.h>

int main(int argc, char** argv)
{
	uptr* window = NULL;
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
		1000,
		0,
		NULL,
		-1
	);

	return 0;
}

