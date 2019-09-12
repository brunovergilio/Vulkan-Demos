# Vulkan Demos
These are simple (yet a bit confusing) demos I set up fast while trying to get acquainted to the Vulkan API. Also included a copy of my updated Math Engine into it. All of these projects are extremely simple, the goal was to just be able to understand the API usage, in order to better structure the wrapper in the future and throw that into my own engine.

Here's what I've worked on so far:

```
* Clear Screen - The first demo I've set up. Nothing except a blank screen.
* Simple Triangle - Basic shader a simple triangle drawn on the screen.
* Triangles and Buffers - Using descriptor sets to position the objects on the screen.
* Texture Mapping - Showing a basic texture sampling through combined separate textures / samplers
* Texture Array - Drawing 6 boxes and sampling from different texture layers on each
* Cube Mapping - Draws a skybox, as well a sphere that uses static environment mapping
* Dynamic Cube Mapping - Just wanted to be able to draw some geometry inside a cubemap using the geometry shader and doing it in a single pass
* Basic Lighting - Showing basic light types (Ambient / Directional / Point / Spot)
* Simple Fog - Testing out 2 different fog algorithms
* Deferred Rendering (Multiple Render Passes) - basic deferred scene using multiple render targets, using separate render passes for the G-Buffer phase and for the Lighting phase
* Deferred Rendering (Multiple Subpasses) - same scene as above, but this one uses a single render pass with multiple subpasses inside; also using input_attachments instead of texture objects (in the shader).
```
