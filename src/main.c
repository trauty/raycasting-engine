#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include "defines.h"
#include "platform.h"

/*#if PLATFORM_WIN
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
*/

static bool should_close = false;

void on_window_destroy()
{
	printf("WINDOW CLOSED!\n");
	should_close = true;
}

int main(int argc, char** argv)
{
	printf("STARTED!\n");

	Framebuffer* framebuffer = create_window();
	if(framebuffer == NULL)
	{
		return -1;
	}

	register_window_destroy_cb(on_window_destroy);

	while (!should_close)
	{
		for (int y = 0; y < framebuffer->width; y++)
        {
            for (int x = 0; x < framebuffer->height; x++)
            {
                framebuffer->pixels[framebuffer->height * y + x] = 0x00ff0000;
            }
        }

		update_window();
	}

	return 0;
}