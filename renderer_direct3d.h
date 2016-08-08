#pragma once

#include "image.h"
#include "renderer.h"
#include "world.h"
#include "camera.h"
#include "memory.h"

struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11RasterizerState;
struct IDXGISwapChain;

struct ConstantBuffer
{
    Matrix4x4 model_view_projection;
    Matrix4x4 model;
    Matrix4x4 projection;
};

struct Geometry {
    ID3D11Buffer* vertices;
    ID3D11Buffer* indices;
    unsigned num_indices;
};

struct RenderTarget
{
    PixelFormat pixel_format;
    unsigned width;
    unsigned height;
    bool clear_depth_stencil : 1;
    bool clear : 1;
    Color clear_color;
    RRHandle render_resource;
};

struct RenderTargetResource
{
    ID3D11Texture2D* texture;
    ID3D11RenderTargetView* view;
};

struct Texture
{
    ID3D11Texture2D* resource;
    ID3D11ShaderResourceView* view;
};

struct Shader
{
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11InputLayout* input_layout;
};

enum struct RenderResourceType
{
    Unused,
    Geometry,
    Texture,
    Shader,
    RenderTarget,
    MappedTexture
};

struct RenderResource
{
    RenderResourceType type;
    union 
    {
        Geometry geometry;
        Texture texture;
        Shader shader;
        RenderTargetResource render_target;
    };
};

struct MappedTexture
{
    void* data;
    RRHandle texture;
};

struct Rect;

enum struct DrawLights { DrawLights, DoNotDrawLights };

struct Renderer
{
    void init(void* window_handle);
    void shutdown();
    RRHandle load_shader(const wchar* filename);
    void set_shader(RRHandle shader);
    RenderTarget create_back_buffer();
    RenderTarget create_render_texture(PixelFormat pf, unsigned width, unsigned height);
    void set_constant_buffers(const ConstantBuffer& data);
    unsigned find_free_resource_handle() const;
    RRHandle load_geometry(Vertex* vertices, unsigned num_vertices, unsigned* indices, unsigned num_indices);
    void unload_resource(RRHandle handle);
    void set_render_target(RenderTarget* rt);
    void set_render_targets(RenderTarget** rt, unsigned num);
    void draw(const Object& object, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix);
    void clear_depth_stencil();
    void clear_render_target(RenderTarget* sc, const Color& color);
    void present();
    MappedTexture map_texture(const RenderTarget& rt);
    void unmap_texture(const MappedTexture& m);
    void pre_draw_frame();
    void set_scissor_rect(const Rect& r);
    void disable_scissor();
    void draw_frame(const World& world, const Camera& camera, DrawLights draw_lights);
    RRHandle load_texture(Allocator* allocator, wchar* filename);
    RenderResource& get_resource(RRHandle r);

    static const unsigned num_resources = 4096;
    static const unsigned max_render_targets = 4;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11Buffer* constant_buffer;
    ID3D11Texture2D* depth_stencil_texture;
    ID3D11DepthStencilView* depth_stencil_view;
    ID3D11RasterizerState* raster_state;
    IDXGISwapChain* swap_chain;
    RenderTarget back_buffer;
    RenderResource resources[num_resources];
    RenderTarget* render_targets[max_render_targets];
};
