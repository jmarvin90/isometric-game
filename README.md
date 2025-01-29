# isometric-game
A two-dimensional, isometric projection game project inspired by the 2D Game Engine with C++ course on Pikuma.com

### Development notes

#### GLM types

| Variable | Description | Type | Reason |
|----------|-------------|------|--------|
|Camera::mouse_screen_position|Capture the pixel coordinate of the mouse relative to the screen|ivec2|Pixels are whole numbers|
|Tilemap::grid_to_pixel()|Get the pixel coordinate of the origin (e.g. top left) of a specified (x, y) tile in the tilemap relative to the world space|ivec2|Pixels are whole numbers|
|Mouse::window_previous_position / window_current_position / world_position / grid_position|Capture the current and previous mouse position (pixel) relative to the window; capture the mouse pixel position relative to the world; capture the mouse position relative to the tilemap|ivec2|Pixel and grid coordinates consist of whole numbers|
|MouseMap::pixel_colour_vector() / tile_walk() / pixel_to_grid()|Get the mouse position relative to the tilemap by comparing the current mouse position to the "first" tile position, and to the mousemap|ivec2|All of the relevant coordinate values are comprised of whole numbers|
|RigidBody::velocity|The velocity of a rigid body|**vec2**|Velocity is applied over time intervals (millis per frame) with fractional portions|
|Sprite::offset|An offset dictating where a sprite should be rendered relative to a transform position|**vec2**|**Not sure!** - potentially because __Transform::position__ is a vec2, and addition between a ivec2 and a vec2 is not supported|
|Transform::position|The position of an entity in world space|**vec2**|Transforms are updated over time intervals (millis per frame) with fractional portions|


