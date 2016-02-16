#include <d3d11.h>
#include <D3Dcompiler.h>

struct Geometry {
    ID3D11Buffer* vertices;
    ID3D11Buffer* indices;
    uint32 num_indices;
};

struct ConstantBuffer
{
    Matrix4x4 model_view_projection;
    Vector4 sun_position;
};

namespace renderer
{
    static const unsigned num_resources = 4096;
}

struct RendererState
{
    IDXGISwapChain* swapchain;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11RenderTargetView* back_buffer;
    ID3D11InputLayout* input_layout;
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11Buffer* constant_buffer;
    ID3D11Texture2D* depth_stencil_texture;
    ID3D11DepthStencilView* depth_stencil_view;
    Geometry geometries[renderer::num_resources];
};

namespace renderer
{

void unload_geometry(RendererState* rs, unsigned geometry_handle);

void init(RendererState* rs, HWND output_window_handle)
{
    DXGI_SWAP_CHAIN_DESC scd = {0};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = 800;
    scd.BufferDesc.Height = 800;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = output_window_handle;
    scd.SampleDesc.Count = 1;
    scd.Windowed = true;
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        0,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &rs->swapchain,
        &rs->device,
        nullptr,
        &rs->device_context);
    ID3D11Texture2D* back_buffer_texture;
    rs->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_texture);
    rs->device->CreateRenderTargetView(back_buffer_texture, nullptr, &rs->back_buffer);
    back_buffer_texture->Release();
    D3D11_VIEWPORT viewport = {0};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 800;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    rs->device_context->RSSetViewports(1, &viewport);
    ID3DBlob* vs_blob;
    ID3DBlob* ps_blob;
    D3DCompileFromFile(L"shader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &vs_blob, 0);
    D3DCompileFromFile(L"shader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &ps_blob, 0);
    rs->device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &rs->vertex_shader);
    rs->device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &rs->pixel_shader);
    rs->device_context->VSSetShader(rs->vertex_shader, 0, 0);
    rs->device_context->PSSetShader(rs->pixel_shader, 0, 0);
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    rs->device->CreateInputLayout(ied, 4, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &rs->input_layout);
    rs->device_context->IASetInputLayout(rs->input_layout);
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
    rs->device_context->OMSetRenderTargets(1, &rs->back_buffer, rs->depth_stencil_view);
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
    rs->swapchain->Release();
    rs->input_layout->Release();
    rs->vertex_shader->Release();
    rs->pixel_shader->Release();
    rs->swapchain->Release();
    rs->back_buffer->Release();
    rs->device->Release();
    rs->device_context->Release();
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

unsigned load_geometry(RendererState* rs, Vertex* vertices, unsigned num_vertices, uint32* indices, unsigned num_indices)
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
        bd.ByteWidth = sizeof(uint32) * num_indices;
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

void draw(RendererState* rs, unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix)
{
    auto geometry = rs->geometries[geometry_handle];
    ConstantBuffer constant_buffer_data = {};
    constant_buffer_data.model_view_projection = world_transform_matrix * view_matrix * projection_matrix;
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

void clear(RendererState* rs, const Color& color)
{
    rs->device_context->ClearRenderTargetView(rs->back_buffer, &color.r);
    rs->device_context->ClearDepthStencilView(rs->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void present(RendererState* rs)
{
    rs->swapchain->Present(0, 0);
}
} // namespace renderer
