#include "renderer_direct3d.h"
#include <D3Dcompiler.h>
#include "world.h"

static DXGI_FORMAT pixel_format_to_dxgi_format(PixelFormat pf)
{
    switch(pf)
    {
    case PixelFormat::R8G8B8A8_UINT_NORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

namespace renderer
{

void check_ok(HRESULT res)
{
    if (res >= 0)
    {
        return;
    }

    static wchar_t msg[120];
    wsprintf(msg, L"Error in renderer: %0x", res);
    MessageBox(nullptr, msg, nullptr, 0);
}

void unload_geometry(RendererState* rs, unsigned geometry_handle);
RenderTarget create_back_buffer(RendererState* rs);
void set_render_target(RendererState* rs, RenderTarget* rt);

void init(RendererState* rs, HWND window_handle)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = 800;
    scd.BufferDesc.Height = 800;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = window_handle;
    scd.SampleDesc.Count = 1;
    scd.Windowed = true;

    check_ok(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        0,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &rs->swap_chain,
        &rs->device,
        nullptr,
        &rs->device_context
    ));

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 800;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    rs->device_context->RSSetViewports(1, &viewport);
    D3D11_BUFFER_DESC cbd = {0};
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    rs->device->CreateBuffer(&cbd, nullptr, &rs->constant_buffer);
    D3D11_TEXTURE2D_DESC dstd = {0};
    dstd.Width = 800;
    dstd.Height = 800;
    dstd.MipLevels = 1;
    dstd.ArraySize = 1;
    dstd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dstd.SampleDesc.Count = 1;
    dstd.Usage = D3D11_USAGE_DEFAULT;
    dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dstd.CPUAccessFlags = 0;
    dstd.MiscFlags = 0;
    rs->device->CreateTexture2D(&dstd, nullptr, &rs->depth_stencil_texture);
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;
    dsvd.Flags = 0;
    rs->device->CreateDepthStencilView(rs->depth_stencil_texture, &dsvd, &rs->depth_stencil_view);

    rs->back_buffer = create_back_buffer(rs);
    set_render_target(rs, &rs->back_buffer);
}

void shutdown(RendererState* rs)
{
    for (unsigned i = 0; i < num_resources; ++ i)
    {
        const Geometry& g = rs->geometries[i];

        if (g.vertices != nullptr)
            unload_geometry(rs, i);
    }

    rs->depth_stencil_texture->Release();
    rs->depth_stencil_view->Release();
    rs->device->Release();
    rs->device_context->Release();
}

Shader load_shader(RendererState* rs, const wchar* filename)
{
    ID3DBlob* vs_blob;
    ID3DBlob* ps_blob;
    D3DCompileFromFile(filename, 0, 0, "VShader", "vs_4_0", 0, 0, &vs_blob, 0);
    D3DCompileFromFile(filename, 0, 0, "PShader", "ps_4_0", 0, 0, &ps_blob, 0);
    Shader s = {};
    rs->device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &s.vertex_shader);
    rs->device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &s.pixel_shader);
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    rs->device->CreateInputLayout(ied, 4, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &s.input_layout);
    vs_blob->Release();
    ps_blob->Release();
    return s;
}

void unload_shader(Shader* s)
{
    s->input_layout->Release();
    s->vertex_shader->Release();
    s->pixel_shader->Release();
}

void set_shader(RendererState* rs, Shader* s)
{
    rs->device_context->VSSetShader(s->vertex_shader, 0, 0);
    rs->device_context->PSSetShader(s->pixel_shader, 0, 0);
    rs->device_context->IASetInputLayout(s->input_layout);
}

RenderTarget create_back_buffer(RendererState* rs)
{
    RenderTarget rt = {};
    ID3D11Texture2D* back_buffer_texture;
    rs->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_texture);

    D3D11_TEXTURE2D_DESC td = {};
    back_buffer_texture->GetDesc(&td);

    rs->device->CreateRenderTargetView(back_buffer_texture, nullptr, &rt.view);
    back_buffer_texture->Release();
    return rt;
}

RenderTarget create_render_texture(RendererState* rs, PixelFormat pf)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    rs->swap_chain->GetDesc(&scd);
    D3D11_TEXTURE2D_DESC rtd = {};
    rtd.Width = scd.BufferDesc.Width;
    rtd.Height = scd.BufferDesc.Height;
    rtd.MipLevels = 1;
    rtd.ArraySize = 1;
    rtd.Format = pixel_format_to_dxgi_format(pf);
    rtd.SampleDesc.Count = 1;
    rtd.Usage = D3D11_USAGE_DEFAULT;
    rtd.BindFlags = D3D11_BIND_RENDER_TARGET;
    rtd.CPUAccessFlags = 0;
    rtd.MiscFlags = 0;
    ID3D11Texture2D* texture;
    rs->device->CreateTexture2D(&rtd, NULL, &texture);
    D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
    rtvd.Format = rtd.Format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D.MipSlice = 0;
    RenderTarget rt = {};
    rt.texture = texture;
    rt.pixel_format = pf;
    rs->device->CreateRenderTargetView(texture, &rtvd, &rt.view);
    return rt;
}

void set_constant_buffers(RendererState* rs, const ConstantBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE ms_constant_buffer;
    rs->device_context->Map(rs->constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms_constant_buffer);
    memcpy(ms_constant_buffer.pData, &data, sizeof(ConstantBuffer));
    rs->device_context->Unmap(rs->constant_buffer, 0);
}

int find_free_geometry_handle(const RendererState& rs)
{
    for (unsigned i = 0; i < num_resources; ++i)
    {
        if (rs.geometries[i].vertices == nullptr)
        {
            return i;
        }
    }
    return -1;
}

unsigned load_geometry(RendererState* rs, Vertex* vertices, unsigned num_vertices, unsigned* indices, unsigned num_indices)
{
    unsigned handle = renderer::find_free_geometry_handle(*rs);

    if (handle == -1)
        return InvalidHandle;

    ID3D11Buffer* vertex_buffer;
    {
        D3D11_BUFFER_DESC bd = {0};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * num_vertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        D3D11_SUBRESOURCE_DATA srd = {0};
        srd.pSysMem = vertices;
        rs->device->CreateBuffer(&bd, &srd, &vertex_buffer);
    }

    ID3D11Buffer* index_buffer;
    {
        D3D11_BUFFER_DESC bd = {0};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(unsigned) * num_indices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        D3D11_SUBRESOURCE_DATA srd = {0};
        srd.pSysMem = indices;
        rs->device->CreateBuffer(&bd, &srd, &index_buffer);
    }

    Geometry g = {0};
    g.vertices = vertex_buffer;
    g.indices = index_buffer;
    g.num_indices = num_indices;
    rs->geometries[handle] = g;
    return handle;
}

void unload_geometry(RendererState* rs, unsigned geometry_handle)
{
    if (geometry_handle < 0 || geometry_handle >= num_resources)
        return;

    Geometry* geometry = &rs->geometries[geometry_handle];

    if (geometry->vertices == nullptr)
        return;

    geometry->vertices->Release();
    geometry->indices->Release();
    memset(rs->geometries + geometry_handle, 0, sizeof(Geometry));
}

void set_render_target(RendererState* rs, RenderTarget* rt)
{
    rs->device_context->OMSetRenderTargets(1, &rt->view, rs->depth_stencil_view);
}

void draw(RendererState* rs, unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix)
{
    auto geometry = rs->geometries[geometry_handle];
    ConstantBuffer constant_buffer_data = {};
    constant_buffer_data.model_view_projection = world_transform_matrix * view_matrix * projection_matrix;
    constant_buffer_data.model = world_transform_matrix;
    constant_buffer_data.projection = projection_matrix;
    constant_buffer_data.sun_position = {290, 200, -300, 1};
    renderer::set_constant_buffers(rs, constant_buffer_data);
    rs->device_context->VSSetConstantBuffers(0, 1, &rs->constant_buffer);
    rs->device_context->PSSetConstantBuffers(0, 1, &rs->constant_buffer);
    unsigned stride = sizeof(Vertex);
    unsigned offset = 0;
    rs->device_context->IASetVertexBuffers(0, 1, &geometry.vertices, &stride, &offset);
    rs->device_context->IASetIndexBuffer(geometry.indices, DXGI_FORMAT_R32_UINT, 0);
    rs->device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    rs->device_context->DrawIndexed(geometry.num_indices, 0, 0);
}

void clear_depth_stencil(RendererState* rs)
{
    rs->device_context->ClearDepthStencilView(rs->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void clear_render_target(RendererState* rs, RenderTarget* sc, const Color& color)
{
    rs->device_context->ClearRenderTargetView(sc->view, &color.r);
}

void present(RendererState* rs)
{
    rs->swap_chain->Present(0, 0);
}

Image read_back_texture(Allocator* alloc, RendererState* rs, const RenderTarget& rt)
{
    D3D11_TEXTURE2D_DESC rtd = {};
    rt.texture->GetDesc(&rtd);
    rtd.Usage = D3D11_USAGE_STAGING;
    rtd.BindFlags = 0;
    rtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ID3D11Texture2D* staging_texture;
    rs->device->CreateTexture2D(&rtd, NULL, &staging_texture);
    rs->device_context->CopyResource(staging_texture, rt.texture);
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    rs->device_context->Map(staging_texture, 0, D3D11_MAP_READ, 0, &mapped_resource);
    unsigned size = image::size(rt.pixel_format, rtd.Width, rtd.Height);
    unsigned char* p = (unsigned char*)alloc->alloc(size);
    memcpy(p, mapped_resource.pData, size);
    rs->device_context->Unmap(staging_texture, 0);
    Image i = {};
    i.pixel_format = rt.pixel_format;
    i.width = rtd.Width;
    i.height = rtd.Height;
    i.data = p;
    return i;
}

void draw_frame(RendererState* rs, const World& world, const Camera& camera)
{
    Color r = {0.2f, 0, 0, 1};
    clear_depth_stencil(rs);
    clear_render_target(rs, &rs->back_buffer, r);

    for (unsigned i = 0; i < world.num_objects; ++i)
    {
        if (world.objects[i].valid)
            renderer::draw(rs, world.objects[i].geometry_handle, world.objects[i].world_transform, camera::calc_view_matrix(camera), camera.projection_matrix);
    }

    renderer::present(rs);
}

} // namespace renderer
