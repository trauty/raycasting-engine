#pragma once

#include <stdbool.h>

#include "defines.h"
#include "framebuffer.h"

typedef void (*OnWindowDestroyCB)();

Framebuffer* create_window();
void update_window();
void register_window_destroy_cb(OnWindowDestroyCB);