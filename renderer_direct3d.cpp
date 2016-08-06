#include "renderer_direct3d.h"
#include <D3Dcompiler.h>
#include "world.h"

namespace
{
    DXGI_FORMAT pixel_format_to_dxgi_format(PixelFormat pf)
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
}

void Renderer::init(HWND window_handle)
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
        &swap_chain,
        &device,
        nullptr,
        &device_context
    ));

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 800;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    device_context->RSSetViewports(1, &viewport);
    D3D11_BUFFER_DESC cbd = {0};
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    device->CreateBuffer(&cbd, nullptr, &constant_buffer);
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
    device->CreateTexture2D(&dstd, nullptr, &depth_stencil_texture);
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;
    dsvd.Flags = 0;
    device->CreateDepthStencilView(depth_stencil_texture, &dsvd, &depth_stencil_view);

    back_buffer = create_back_buffer();
    set_render_target(&back_buffer);
}

void Renderer::shutdown()
{
    for (unsigned i = 0; i < num_resources; ++ i)
    {
        const Geometry& g = geometries[i];

        if (g.vertices != nullptr)
            unload_geometry(i);
    }

    depth_stencil_texture->Release();
    depth_stencil_view->Release();
    device->Release();
    device_context->Release();
}

Shader Renderer::load_shader(const wchar* filename)
{
    ID3DBlob* vs_blob;
    ID3DBlob* ps_blob;
    D3DCompileFromFile(filename, 0, 0, "VShader", "vs_4_0", 0, 0, &vs_blob, 0);
    D3DCompileFromFile(filename, 0, 0, "PShader", "ps_4_0", 0, 0, &ps_blob, 0);
    Shader s = {};
    device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &s.vertex_shader);
    device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &s.pixel_shader);
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    device->CreateInputLayout(ied, 4, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &s.input_layout);
    vs_blob->Release();
    ps_blob->Release();
    return s;
}

void Renderer::unload_shader(Shader* s)
{
    s->input_layout->Release();
    s->vertex_shader->Release();
    s->pixel_shader->Release();
}

void Renderer::set_shader(Shader* s)
{
    device_context->VSSetShader(s->vertex_shader, 0, 0);
    device_context->PSSetShader(s->pixel_shader, 0, 0);
    device_context->IASetInputLayout(s->input_layout);
}

RenderTarget Renderer::create_back_buffer()
{
    RenderTarget rt = {};
    ID3D11Texture2D* back_buffer_texture;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_texture);

    D3D11_TEXTURE2D_DESC td = {};
    back_buffer_texture->GetDesc(&td);

    device->CreateRenderTargetView(back_buffer_texture, nullptr, &rt.view);
    back_buffer_texture->Release();
    return rt;
}

RenderTarget Renderer::create_render_texture(PixelFormat pf)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    swap_chain->GetDesc(&scd);
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
    device->CreateTexture2D(&rtd, NULL, &texture);
    D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
    rtvd.Format = rtd.Format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D.MipSlice = 0;
    RenderTarget rt = {};
    rt.texture = texture;
    rt.pixel_format = pf;
    device->CreateRenderTargetView(texture, &rtvd, &rt.view);
    return rt;
}

void Renderer::set_constant_buffers(const ConstantBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE ms_constant_buffer;
    device_context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms_constant_buffer);
    memcpy(ms_constant_buffer.pData, &data, sizeof(ConstantBuffer));
    device_context->Unmap(constant_buffer, 0);
}

int Renderer::find_free_geometry_handle() const
{
    for (unsigned i = 0; i < num_resources; ++i)
    {
        if (geometries[i].vertices == nullptr)
        {
            return i;
        }
    }
    return -1;
}

unsigned Renderer::load_geometry(Vertex* vertices, unsigned num_vertices, unsigned* indices, unsigned num_indices)
{
    unsigned handle = find_free_geometry_handle();

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
        device->CreateBuffer(&bd, &srd, &vertex_buffer);
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
        device->CreateBuffer(&bd, &srd, &index_buffer);
    }

    Geometry g = {0};
    g.vertices = vertex_buffer;
    g.indices = index_buffer;
    g.num_indices = num_indices;
    geometries[handle] = g;
    return handle;
}

void Renderer::unload_geometry(unsigned geometry_handle)
{
    if (geometry_handle < 0 || geometry_handle >= num_resources)
        return;

    Geometry* geometry = &geometries[geometry_handle];

    if (geometry->vertices == nullptr)
        return;

    geometry->vertices->Release();
    geometry->indices->Release();
    memset(geometries + geometry_handle, 0, sizeof(Geometry));
}

void Renderer::set_render_target(RenderTarget* rt)
{
    device_context->OMSetRenderTargets(1, &rt->view, depth_stencil_view);
}

void Renderer::set_render_targets(RenderTarget** rts, unsigned num)
{
    ID3D11RenderTargetView* targets[5];
    for (unsigned i = 0; i < num; ++i)
    {
        targets[i] = rts[i]->view;
    }
    device_context->OMSetRenderTargets(num, targets, depth_stencil_view);
}

void Renderer::draw(unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix)
{
    auto geometry = geometries[geometry_handle];
    ConstantBuffer constant_buffer_data = {};
    constant_buffer_data.model_view_projection = world_transform_matrix * view_matrix * projection_matrix;
    constant_buffer_data.model = world_transform_matrix;
    constant_buffer_data.projection = projection_matrix;
    constant_buffer_data.sun_position = {290, 200, -300, 1};
    set_constant_buffers(constant_buffer_data);
    device_context->VSSetConstantBuffers(0, 1, &constant_buffer);
    device_context->PSSetConstantBuffers(0, 1, &constant_buffer);
    unsigned stride = sizeof(Vertex);
    unsigned offset = 0;
    device_context->IASetVertexBuffers(0, 1, &geometry.vertices, &stride, &offset);
    device_context->IASetIndexBuffer(geometry.indices, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device_context->DrawIndexed(geometry.num_indices, 0, 0);
}

void Renderer::clear_depth_stencil()
{
    device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::clear_render_target(RenderTarget* sc, const Color& color)
{
    device_context->ClearRenderTargetView(sc->view, &color.r);
}

void Renderer::present()
{
    swap_chain->Present(0, 0);
}

Image Renderer::read_back_texture(Allocator* alloc, const RenderTarget& rt)
{
    D3D11_TEXTURE2D_DESC rtd = {};
    rt.texture->GetDesc(&rtd);
    rtd.Usage = D3D11_USAGE_STAGING;
    rtd.BindFlags = 0;
    rtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ID3D11Texture2D* staging_texture;
    device->CreateTexture2D(&rtd, NULL, &staging_texture);
    device_context->CopyResource(staging_texture, rt.texture);
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    device_context->Map(staging_texture, 0, D3D11_MAP_READ, 0, &mapped_resource);
    unsigned size = image::size(rt.pixel_format, rtd.Width, rtd.Height);
    unsigned char* p = (unsigned char*)alloc->alloc(size);
    memcpy(p, mapped_resource.pData, size);
    device_context->Unmap(staging_texture, 0);
    Image i = {};
    i.pixel_format = rt.pixel_format;
    i.width = rtd.Width;
    i.height = rtd.Height;
    i.data = p;
    return i;
}

void Renderer::draw_frame(const World& world, const Camera& camera)
{
    Color r = {0.2f, 0, 0, 1};
    clear_depth_stencil();
    clear_render_target(&back_buffer, r);

    for (unsigned i = 0; i < world.num_objects; ++i)
    {
        if (world.objects[i].valid)
            draw(world.objects[i].geometry_handle, world.objects[i].world_transform, camera::calc_view_matrix(camera), camera.projection_matrix);
    }

    present();
}
