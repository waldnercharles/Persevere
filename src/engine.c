#include "game.h"

#include "engine.h"
#include "asset.h"
#include "ecs.h"
#include "log.h"
#include "mixer.h"
#include "renderer.h"

#include "assets/sound_asset.h"
#include "assets/shader_asset.h"
#include "assets/texture_asset.h"

void
init(struct game *game)
{
    game->ecs = ecs_alloc();
    // game->mixer = mixer_new();
    game->renderer = renderer_new();
    game->assets = asset_new();

    log_debug("Starting Nerd Engine...");

    asset_init(game->assets);

    asset_add_handler(game->assets, "ogg", asset_load_sound, NULL, game->mixer);

    asset_add_handler(game->assets, "jpg", asset_load_texture, NULL, NULL);
    asset_add_handler(game->assets, "png", asset_load_texture, NULL, NULL);

    asset_add_handler(game->assets,
                      "frag",
                      asset_load_fragment_shader,
                      NULL,
                      NULL);

    asset_add_handler(game->assets,
                      "vert",
                      asset_load_vertex_shader,
                      NULL,
                      NULL);
}