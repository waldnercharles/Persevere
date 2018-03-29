#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "allocators/allocator.c"
#include "array.c"
#include "bitset.c"
#include "ecs.c"
#include "engine.c"
#include "fbo.c"
#include "file.c"
#include "map.c"
#include "mixer.c"
#include "shader.c"
#include "sort.c"
#include "sparse_set.c"
#include "vec.c"

#include "renderer.c"
#include "renderers/sprite_renderer.c"
#include "renderers/light_renderer.c"
#include "renderers/shadowcaster_renderer.c"

#include "asset.c"
#include "assets/texture_asset.c"
#include "assets/shader_asset.c"
#include "assets/sound_asset.c"

#include "systems/movement_system.c"
#include "systems/sprite_system.c"
#include "systems/light_system.c"
#include "systems/shadowcaster_system.c"
#include "systems/mouse_follow_system.c"
