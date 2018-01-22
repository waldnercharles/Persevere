#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "allocators/allocator.c"
#include "array.c"
#include "bitset.c"
#include "ecs.c"
#include "engine.c"
#include "file.c"
#include "map.c"
#include "mixer.c"
#include "renderer.c"
#include "shader.c"
#include "sparse_set.c"
#include "vec.c"

#include "asset.c"
#include "assets/texture_asset.c"
#include "assets/shader_asset.c"
#include "assets/sound_asset.c"

#include "components/body_component.h"
#include "components/movement_component.h"
#include "components/render_component.h"

#include "systems/movement_system.c"
#include "systems/render_system.c"
