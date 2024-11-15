#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include "defines.h"
#include "window_win32.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main(int argc, char** argv)
{
	printf("sdkljn\n");

	create_window();

	printf("no\n");

	return 0;
}