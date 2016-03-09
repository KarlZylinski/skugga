#pragma once

#include <d3d11.h>
#include "image.h"
#include "renderer.h"
#include "world.h"
#include "camera.h"
#include "memory.h"

namespace renderer
{
    static const unsigned num_resources = 4096;
}

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
    ID3D11RenderTargetView* view;
};

struct RendererState
{
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11Buffer* constant_buffer;
    ID3D11Texture2D* depth_stencil_texture;
    ID3D11DepthStencilView* depth_stencil_view;
    IDXGISwapChain* swap_chain;
    RenderTarget back_buffer;
    Geometry geometries[renderer::num_resources];
};

struct Shader
{
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11InputLayout* input_layout;
};

namespace renderer
{

void init(RendererState* rs, HWND window_handle);
void unload_geometry(RendererState* rs, unsigned geometry_handle);
RenderTarget create_back_buffer(RendererState* rs);
void set_render_target(RendererState* rs, RenderTarget* rt);
void init(RendererState* rs, HWND window_handle);
void shutdown(RendererState* rs);
Shader load_shader(RendererState* rs, const wchar* filename);
void unload_shader(Shader* s);
void set_shader(RendererState* rs, Shader* s);
RenderTarget create_back_buffer(RendererState* rs);
RenderTarget create_render_texture(RendererState* rs, PixelFormat pf);
void set_constant_buffers(RendererState* rs, const ConstantBuffer& data);
int find_free_geometry_handle(const RendererState& rs);
unsigned load_geometry(RendererState* rs, Vertex* vertices, unsigned num_vertices, unsigned* indices, unsigned num_indices);
void unload_geometry(RendererState* rs, unsigned geometry_handle);
void set_render_target(RendererState* rs, RenderTarget* rt);
void draw(RendererState* rs, unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix);
void clear_depth_stencil(RendererState* rs);
void clear_render_target(RendererState* rs, RenderTarget* sc, const Color& color);
void present(RendererState* rs);
Image read_back_texture(Allocator* alloc, RendererState* rs, const RenderTarget& rt);
void draw_frame(RendererState* rs, const World& world, const Camera& camera);

}