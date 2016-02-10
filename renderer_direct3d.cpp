#include <d3d11.h>
#include <D3Dcompiler.h>

struct Geometry {
    ID3D11Buffer* mesh;
    unsigned num_vertices;
};

struct ConstantBuffer
{
    Matrix4x4 model_view_projection;
};

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
    static const unsigned num_geometries = 4096;
    Geometry geometries[num_geometries];
};

namespace renderer
{
void init(RendererState* rs, HWND output_window_handle)
{
    DXGI_SWAP_CHAIN_DESC scd = {0};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = output_window_handle;
    scd.SampleDesc.Count = 4;
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
    rs->device_context->OMSetRenderTargets(1, &rs->back_buffer, nullptr);
    D3D11_VIEWPORT viewport = {0};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 800;
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
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    rs->device->CreateInputLayout(ied, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &rs->input_layout);
    rs->device_context->IASetInputLayout(rs->input_layout);
    D3D11_BUFFER_DESC cbd;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    rs->device->CreateBuffer(&cbd, nullptr, &rs->constant_buffer);
}

void shutdown(RendererState* rs)
{
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
    memcpy(ms_constant_buffer.pData, &data, sizeof(data));
    rs->device_context->Unmap(rs->constant_buffer, 0);
}

int find_free_geometry_handle(const RendererState& rs)
{
    for (unsigned i = 0; i < rs.num_geometries; ++i)
    {
        if (rs.geometries[i].mesh == nullptr)
        {
            return i;
        }
    }
    return -1;
}

unsigned load_geometry(RendererState* rs, Vertex* vertices, unsigned num_vertices)
{
    unsigned handle = renderer::find_free_geometry_handle(*rs);

    if (handle == -1)
        return InvalidHandle;

    ID3D11Buffer* vertex_buffer;
    D3D11_BUFFER_DESC bd = {0};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * num_vertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    rs->device->CreateBuffer(&bd, nullptr, &vertex_buffer);
    D3D11_MAPPED_SUBRESOURCE ms;
    rs->device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, vertices, num_vertices * sizeof(Vertex));
    rs->device_context->Unmap(vertex_buffer, 0);
    Geometry g = {0};
    g.mesh = vertex_buffer;
    g.num_vertices = num_vertices;
    rs->geometries[handle] = g;
    return handle;
}

void unload_geometry(RendererState* rs, unsigned geometry_handle)
{
    if (geometry_handle < 0 || geometry_handle >= rs->num_geometries)
        return;

    Geometry* geometry = &rs->geometries[geometry_handle];

    if (geometry->mesh == nullptr)
        return;

    geometry->mesh->Release();
    memset(rs->geometries + geometry_handle, 0, sizeof(Geometry));
}

void draw(RendererState* rs, unsigned geometry_handle, const Matrix4x4& world_transform_matrix, const Matrix4x4& view_matrix, const Matrix4x4& projection_matrix)
{
    auto geometry = rs->geometries[geometry_handle];
    ConstantBuffer constant_buffer_data = {0};
    constant_buffer_data.model_view_projection = world_transform_matrix * view_matrix * projection_matrix;
    renderer::set_constant_buffers(rs, constant_buffer_data);
    rs->device_context->VSSetConstantBuffers(0, 1, &rs->constant_buffer);
    rs->device_context->PSSetConstantBuffers(0, 1, &rs->constant_buffer);
    unsigned stride = sizeof(Vertex);
    unsigned offset = 0;
    rs->device_context->IASetVertexBuffers(0, 1, &geometry.mesh, &stride, &offset);
    rs->device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    rs->device_context->Draw(geometry.num_vertices, 0);
}

void clear(RendererState*rs, const Color& color)
{
    rs->device_context->ClearRenderTargetView(rs->back_buffer, &color.r);
}

void present(RendererState* rs)
{
    rs->swapchain->Present(0, 0);
}
} // namespace renderer