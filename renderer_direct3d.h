#pragma once

#include <d3d11.h>
#include "image.h"
#include "renderer.h"
#include "world.h"
#include "camera.h"
#include "memory.h"

struct ConstantBuffer
{
    Matrix4x4 model_view_projection;
    Matrix4x4 model;
    Matrix4x4 projection;
    Vector4 sun_position;
};

struct Geometry {
    ID3D11Buffer* vertices;
    ID3D11Buffer* indices;
    unsigned num_indices;
};

struct RenderTarget
{
    ID3D11Texture2D* texture;
    PixelFormat pixel_format;
    unsigned width;
    unsigned height;
    ID3D11RenderTargetView* view;
};

struct Shader
{
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11InputLayout* input_layout;
};

enum struct DrawLights { DrawLights, DoNotDrawLights };

struct Renderer
{
    void init(HWND window_handle);
    void shutdown();
    Shader load_shader(const wchar* filename);
    void unload_shader(Shader* s);
    void set_shader(Shader* s);
    RenderTarget create_back_buffer();
    RenderTarget create_render_texture(PixelFormat pf);
    void set_constant_buffers(const ConstantBuffer& data);
    int find_free_geometry_handle() const;
    unsigned load_geometry(Vertex* vertices, unsigned num_vertices, unsigned* indices, unsigned num_indices);
    void unload_geometry(unsigned geometry_handle);
    void set_render_target(RenderTarget* rt);
    void set_render_targets(RenderTarget** rt, unsigned num);
    void draw(unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix, const Object** lights, unsigned num_lights);
    void clear_depth_stencil();
    void clear_render_target(RenderTarget* sc, const Color& color);
    void present();
    void read_back_texture(Image* out, const RenderTarget& rt);
    void draw_frame(const World& world, const Camera& camera, DrawLights draw_lights);

    static const unsigned num_resources = 4096;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11Buffer* constant_buffer;
    ID3D11Texture2D* depth_stencil_texture;
    ID3D11DepthStencilView* depth_stencil_view;
    IDXGISwapChain* swap_chain;
    RenderTarget back_buffer;
    Geometry geometries[num_resources];
};
