#include <SDL2/SDL_image.h>
#include <archive.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <components/highlighted_entity_component.h>
#include <components/junction_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
#include <components/segment_manager_component.h>
#include <components/selected_entity_component.h>
#include <components/spatialmapcell_component.h>
#include <components/spatialmapcell_span_component.h>
#include <components/tilespec_component.h>
#include <constants.h>
#include <entt/entt.hpp>
#include <game.h>
#include <grid.h>
#include <imgui.h>
#include <projection.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <string>
#include <systems/camera_system.h>
#include <systems/mouse_system.h>
#include <systems/render_system.h>
#include <systems/segment_system.h>
#include <systems/spatialmap_system.h>
#include <systems/tilemap_system.h>
#include <utility.h>

Game::Game() { spdlog::info("Game constructor called."); }

Game::~Game() { spdlog::info("Game destructor called."); }

void Game::initialise()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GetDesktopDisplayMode(0, &display_mode);

    // Create the SDL Window
    window = std::unique_ptr<SDL_Window, Utility::SDLDestroyer>(
        SDL_CreateWindow(
            NULL, // title - leave blank for now
            SDL_WINDOWPOS_CENTERED, // Window xconstant position (centred)
            SDL_WINDOWPOS_CENTERED, // Window y position (centred)
            display_mode.w, // X res from current display mode
            display_mode.h, // Y res from current display mode
            SDL_WINDOW_FULLSCREEN // | SDL_WINDOW_ALLOW_HIGHDPI // Input grabbed flag
        )
    );

    if (!window) {
        spdlog::error("Could not initialise SDL Window.");
    }

    registry = entt::registry();

    Grid<entt::entity, TileMapProjection>& tilemap {
        registry.ctx().emplace<Grid<entt::entity, TileMapProjection>>()
    };

    Grid<entt::entity, SpatialMapProjection>& spatial_map {
        registry.ctx().emplace<Grid<entt::entity, SpatialMapProjection>>()
    };

    // TODO: move this somewhere smart under some smart condition
    // Needs to happen before spritesheet
    renderer = std::unique_ptr<SDL_Renderer, Utility::SDLDestroyer>(
        SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
    );

    // Needs to happen before load
    registry.ctx().emplace<SpriteSheet>(
        std::string { "assets/spritesheet_scaled.png" },
        std::string { "assets/spritesheet.json" },
        renderer
    );

    registry.ctx().emplace<SelectedEntityComponent>();
    registry.ctx().emplace<HighlightedEntityComponent>();

    load_from(registry, constants::SAVE_FILE_PATH);

    registry.on_construct<NavigationComponent>().connect<&TileMapSystem::connect>();
    registry.on_update<NavigationComponent>().connect<&TileMapSystem::connect>();
    registry.on_construct<SpriteComponent>().connect<&SpatialMapSystem::emplace_entity>();
    registry.on_destroy<SpriteComponent>().connect<&SpatialMapSystem::remove_entity>();
    registry.on_construct<SegmentComponent>().connect<&SegmentSystem::connect>();
    registry.on_construct<SegmentComponent>().connect<&SpatialMapSystem::emplace_segment>();
    registry.on_destroy<SegmentComponent>().connect<&SegmentSystem::disconnect>();
    registry.on_destroy<SegmentComponent>().connect<&SpatialMapSystem::remove_segment>();

    [[maybe_unused]] const TileSpecComponent& tilespec { registry.ctx().emplace<TileSpecComponent>(glm::ivec2 { 256, 128 }, 68) };

    registry.ctx().emplace<MouseComponent>();

    registry.ctx().emplace<std::vector<Renderable>>();
    assert(tilemap.area == spatial_map.area);
    registry.ctx().emplace<SegmentManagerComponent>();
    registry.ctx().emplace<CameraComponent>(display_mode);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
    ImGui_ImplSDLRenderer2_Init(renderer.get());
}

void Game::process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Always let ImGui see the event first
        ImGui_ImplSDL2_ProcessEvent(&event);
        ImGuiIO& io = ImGui::GetIO();
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }

            if (event.key.keysym.sym == SDLK_d) {
                debug_mode = !debug_mode;
            }

            break;

        case SDL_MOUSEBUTTONDOWN:
            if (!io.WantCaptureMouse)
                MouseSystem::select_entity(registry);
            break;
        }
    }
}

void Game::update([[maybe_unused]] const float delta_time)
{
    MouseSystem::update(registry);
    CameraSystem::update(registry);
    SegmentSystem::update(registry);
    RenderSystem::update(registry, debug_mode);
}

void Game::render()
{
    // SDL_RenderSetClipRect(renderer, &camera.camera_rect);
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
    SDL_RenderClear(renderer.get());
    RenderSystem::render(registry, renderer.get(), debug_mode);
    if (debug_mode) {
        RenderSystem::render_imgui_ui(registry, renderer.get());
        RenderSystem::render_segments(registry, renderer.get());
    }
    SDL_RenderPresent(renderer.get());
}

void Game::run()
{
    is_running = true;
    while (is_running) {
        // The start point (in ticks), the delta to the last frame in s/ms
        const uint64_t start { SDL_GetTicks64() };
        const uint64_t since_last_frame { start - _last_time };
        const float delta_time = { since_last_frame / 1'000.f };

        process_input();
        update(delta_time);
        render();

        // How many millis have elapsed this frame
        const uint64_t elapsed_this_frame { SDL_GetTicks64() - start };

        // Delay until the START of the next frame
        const float time_to_delay { constants::MILLIS_PER_FRAME - elapsed_this_frame };

        if (time_to_delay > 0 && time_to_delay <= constants::MILLIS_PER_FRAME) {
            SDL_Delay(time_to_delay);
        }

        _last_time = start;
    }
}

void Game::destroy()
{
    save_to(registry, constants::SAVE_FILE_PATH);
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_Quit();
}

void Game::load_from(entt::registry& registry, const std::string input_path)
{
    InputArchive my_archive(input_path, registry.ctx().get<const SpriteSheet>());
    entt::snapshot_loader { registry }
        .get<entt::entity>(my_archive)
        .get<TransformComponent>(my_archive)
        .get<SpriteComponent>(my_archive)
        .get<GridPositionComponent>(my_archive)
        .get<NavigationComponent>(my_archive)
        .get<SegmentComponent>(my_archive)
        .get<JunctionComponent>(my_archive)
        .get<SpatialMapCellComponent>(my_archive)
        .get<SpatialMapCellSpanComponent>(my_archive)
        .orphans();

    my_archive.load_context_element("tilemap", registry.ctx().get<Grid<entt::entity, TileMapProjection>>());
    my_archive.load_context_element("spatialmap", registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>());
}

void Game::save_to(entt::registry& registry, const std::string output_path)
{
    OutputArchive my_archive;
    entt::basic_snapshot(registry)
        .get<entt::entity>(my_archive)
        .get<TransformComponent>(my_archive)
        .get<SpriteComponent>(my_archive)
        .get<GridPositionComponent>(my_archive)
        .get<NavigationComponent>(my_archive)
        .get<SegmentComponent>(my_archive)
        .get<JunctionComponent>(my_archive)
        .get<SpatialMapCellComponent>(my_archive)
        .get<SpatialMapCellSpanComponent>(my_archive);

    my_archive.save_context_element("tilemap", registry.ctx().get<Grid<entt::entity, TileMapProjection>>());
    my_archive.save_context_element("spatialmap", registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>());
    my_archive.to_file(output_path);
}