#include "qui_d3d11_implementor.h"

#include "qui_d3d11_dds_texture_loader.h"

#include <string>

std::wstring GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD error_message_id = ::GetLastError();
    if (error_message_id == 0)
        return std::wstring(); //No error message has been recorded

    LPWSTR message_buffer = nullptr;
    size_t size = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error_message_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message_buffer, 0, NULL);

    std::wstring message(message_buffer, size);

    //Free the buffer.
    ::LocalFree(message_buffer);

    return message;
}

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex {
    XMFLOAT3 pos;
    XMFLOAT2 tex;
};

struct CBNeverChanges {
    XMMATRIX view;
};

struct CBChangeOnResize {
    XMMATRIX projection;
};

struct CBChangesEveryFrame {
    XMMATRIX world;
    XMFLOAT4 mesh_color;
};

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* file_name, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out)
{
    HRESULT hr = S_OK;

    DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    shader_flags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* error_blob = nullptr;
    hr = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point, shader_model,
        shader_flags, 0, blob_out, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA(reinterpret_cast<const char*>(error_blob->GetBufferPointer()));
            error_blob->Release();
        }
        return hr;
    }
    if (error_blob) error_blob->Release();

    return S_OK;
}

const char kTestFxShader[] = {
R"(
//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbNeverChanges : register(b0)
{
    matrix View;
};

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample(samLinear, input.Tex) * vMeshColor;
}
)"
};

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromAscii(const char* shader_ascii_str, int len, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out)
{
    HRESULT hr = S_OK;

    DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    shader_flags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* error_blob = nullptr;
    hr = D3DCompile(shader_ascii_str, len, 0, nullptr, nullptr, entry_point, shader_model,
        shader_flags, 0, blob_out, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            OutputDebugStringA(reinterpret_cast<const char*>(error_blob->GetBufferPointer()));
            error_blob->Release();
        }
        return hr;
    }
    if (error_blob) error_blob->Release();

    return S_OK;
}

HRESULT CreateDynamicTextureWithPureColor(
    _In_ ID3D11Device* d3d_device,
    _In_ void* pure_color_buf,
    _In_ UINT pure_color_buf_len,
    _In_ int width,
    _In_ int height,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** texture_rv,
    _Outptr_opt_ ID3D11RenderTargetView** texture_rtv = nullptr)
{
    D3D11_SUBRESOURCE_DATA init_data = { pure_color_buf, /*row bytes*/(pure_color_buf_len / height), 0 };

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
#if 1
    if (texture_rtv) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    }
    else {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }    
#else
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
#endif
    desc.MiscFlags = 0;

    ID3D11Texture2D *tex = NULL;
    HRESULT hr = d3d_device->CreateTexture2D(&desc, &init_data, &tex);

    if (SUCCEEDED(hr))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        if (texture_rv) {
            hr = d3d_device->CreateShaderResourceView(tex,
                &srv_desc, texture_rv);
        }
        if (texture) {
            *texture = tex;
        }
        else {
            tex->Release();
        }

        if (texture_rtv) {
            D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
            // Setup the description of the render target view.
            render_target_view_desc.Format = desc.Format;
            render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            render_target_view_desc.Texture2D.MipSlice = 0;

            // Create the render target view.
            d3d_device->CreateRenderTargetView(tex, &render_target_view_desc, texture_rtv);
        }
    }
    else {

    }
    return hr;
}

HRESULT CreateTextureWithPureColor(
    _In_ ID3D11Device* d3d_device,
    _In_ DWORD pure_color,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** texture_rv)
{
    const uint32_t s_pixel = pure_color;//0xffc99aff;

    D3D11_SUBRESOURCE_DATA init_data = { &s_pixel, sizeof(uint32_t), 0 };

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* tex;
    HRESULT hr = d3d_device->CreateTexture2D(&desc, &init_data, &tex);

    if (SUCCEEDED(hr))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        hr = d3d_device->CreateShaderResourceView(tex,
            &srv_desc, texture_rv);
        if (texture) {
            *texture = tex;
        }
        else {
            tex->Release();
        }
    }
    else {

    }
    return hr;
}


QuiD3D11Implementor::QuiD3D11Implementor(QuiD3D11Window * owner)
    : owner_(owner)
{

}
QuiD3D11Implementor::~QuiD3D11Implementor()
{

}

HRESULT QuiD3D11Implementor::InitDevice()
{
    HRESULT hr = S_OK;

    HWND hwnd = owner_->GetHWND();

    RECT rc;
    ::GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    if (last_width_ == width && last_height_ == height) {
        return hr;
    }

    if (!d3d_device_) {
        UINT create_device_flags = 0;
#ifdef _DEBUG
        //create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driver_types[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT num_of_driver_types = ARRAYSIZE(driver_types);

        D3D_FEATURE_LEVEL feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT num_of_feature_levels = ARRAYSIZE(feature_levels);

        for (UINT driver_type_index = 0; driver_type_index < num_of_driver_types; driver_type_index++) {
            driver_type_ = driver_types[driver_type_index];
            hr = D3D11CreateDevice(nullptr, driver_type_, nullptr, create_device_flags, feature_levels, num_of_feature_levels,
                D3D11_SDK_VERSION, &d3d_device_, &feature_level_, &immediate_context_);

            if (hr == E_INVALIDARG) {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, driver_type_, nullptr, create_device_flags, &feature_levels[1], num_of_feature_levels - 1,
                    D3D11_SDK_VERSION, &d3d_device_, &feature_level_, &immediate_context_);
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"D3D11CreateDevice Error", MB_OK);
            return hr;
        }
    }

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    if (!dxgi_factory_)
    {
        IDXGIDevice* dxgi_device = nullptr;
        hr = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgi_device->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgi_factory_));
                adapter->Release();
            }
            dxgi_device->Release();
        }

        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"ID3D11Device  QueryInterface IDXGIDevice", MB_OK);
            return hr;
        }

        // Create swap chain
        hr = dxgi_factory_->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgi_factory2_));
        if (dxgi_factory2_) {
            // DirectX 11.1 or later
            hr = d3d_device_->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&d3d_device1_));
            if (SUCCEEDED(hr)) {
                (void)immediate_context_->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&immediate_context1_));
            }
            else {
                std::wstring err_msg = GetLastErrorAsString();
                ::MessageBox(nullptr,
                    err_msg.c_str(), L"QueryInterface ID3D11DeviceContext1", MB_OK);
            }
        }
        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgi_factory_->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
    }
    if (!dxgi_factory_) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"IDXGIFactory1 Null", MB_OK);
        return hr;
    }
    if (dxgi_swapchain1_) {
        dxgi_swapchain1_->Release();
        dxgi_swapchain1_ = nullptr;
    }
    if (dxgi_swapchain_) {
        dxgi_swapchain_->Release();
        dxgi_swapchain_ = nullptr;
    }
    if (dxgi_factory2_) {
        DXGI_SWAP_CHAIN_DESC1 sd = { 0 };
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Stereo = FALSE;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_SHADER_INPUT;
        sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
        sd.BufferCount = 1;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Flags = 0;

        hr = dxgi_factory2_->CreateSwapChainForHwnd(d3d_device_, hwnd, &sd, nullptr, nullptr, &dxgi_swapchain1_);
        //hr = m_pDxgiFactory2->CreateSwapChainForComposition(g_pd3dDevice, &sd, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr)) {
            hr = dxgi_swapchain1_->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&dxgi_swapchain_));
        }
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgi_factory_->CreateSwapChain(d3d_device_, &sd, &dxgi_swapchain_);
    }

    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"CreateSwapChain Failed", MB_OK);
        return hr;
    }

    if (render_target_view_) {
        render_target_view_->Release();
        render_target_view_ = nullptr;
    }

    // Create a render target view
    ID3D11Texture2D* back_buffer = nullptr;
    hr = dxgi_swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"SwapChain GetBuffer Failed", MB_OK);
        return hr;
    }

    hr = d3d_device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_view_);
    back_buffer->Release();
    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"CreateRenderTargetView Failed", MB_OK);
        return hr;
    }

    if (depth_stencil_) {
        depth_stencil_->Release();
        depth_stencil_ = nullptr;
    }
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = d3d_device_->CreateTexture2D(&descDepth, nullptr, &depth_stencil_);
    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"DepthStencil CreateTexture2D Failed", MB_OK);
        return hr;
    }

    if (depth_stencil_view_) {
        depth_stencil_view_->Release();
        depth_stencil_view_ = nullptr;
    }
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = d3d_device_->CreateDepthStencilView(depth_stencil_, &descDSV, &depth_stencil_view_);
    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"CreateDepthStencilView Failed", MB_OK);
        return hr;
    }

    immediate_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);

    hr = InitD3DResources();
    if (FAILED(hr)) {
        std::wstring err_msg = GetLastErrorAsString();
        ::MessageBox(nullptr,
            err_msg.c_str(), L"InitD3DResources Failed", MB_OK);
        return hr;
    }

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    immediate_context_->RSSetViewports(1, &vp);

    UpdateResources();

    return S_OK;
}
void QuiD3D11Implementor::CleanupDevice()
{
    if (dxgi_factory2_) dxgi_factory2_->Release();
    if (dxgi_factory_) dxgi_factory_->Release();

    if (immediate_context_) immediate_context_->ClearState();

    CleanupD3DResources();

    if (depth_stencil_) depth_stencil_->Release();
    if (depth_stencil_view_) depth_stencil_view_->Release();
    if (render_target_view_) render_target_view_->Release();
    if (dxgi_swapchain1_) dxgi_swapchain1_->Release();
    if (dxgi_swapchain_) dxgi_swapchain_->Release();
    if (immediate_context1_) immediate_context1_->Release();
    if (immediate_context_) immediate_context_->Release();
    if (d3d_device1_) d3d_device1_->Release();
    if (d3d_device_) d3d_device_->Release();
}
void QuiD3D11Implementor::Render()
{
    if (!immediate_context_)
        return;

    immediate_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);

    //
    // Clear the back buffer
    //
    //immediate_context_->ClearRenderTargetView(render_target_view_, Colors::MidnightBlue);

    immediate_context_->ClearRenderTargetView(render_target_view_, Colors::MediumSpringGreen);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    immediate_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // detail render
    OnRender();

    //
    // Present our back buffer to our front buffer
    //
    RECT rc;
    HWND hwnd = owner_->GetHWND();
    ::GetClientRect(hwnd, &rc);
    RECT rect = { 0, 0, (rc.right - rc.left), (rc.bottom - rc.top) };
    DXGI_PRESENT_PARAMETERS params = { 1, &rect, nullptr, nullptr };
    if (dxgi_swapchain1_) {
        dxgi_swapchain1_->Present1(1, 0, &params);
    }
    else if (dxgi_swapchain_) {
        dxgi_swapchain_->Present(1, 0);
    }
    //if (d3d_device_) {
    //    d3d_device_->Commit();
    //}
}


CubeImplementor::CubeImplementor(QuiD3D11Window * owner)
    : QuiD3D11Implementor(owner)
{
}
CubeImplementor::~CubeImplementor()
{
}
HRESULT CubeImplementor::InitD3DResources()
{
    HRESULT hr = S_OK;

    if (!vertex_shader_) {
        // Compile the vertex shader
        ID3DBlob* vertex_shader_blob = nullptr;

        std::wstring d3d_res_path;
#ifdef D3D_RES_PATH
        char szAnisDuiResPath[MAX_PATH] = { 0 };
        strcpy_s(szAnisDuiResPath, D3D_RES_PATH);
        TCHAR szDuiResPath[MAX_PATH] = { 0 };
        MultiByteToWideChar(CP_ACP, 0, szAnisDuiResPath, strlen(szAnisDuiResPath), szDuiResPath, MAX_PATH - 1);
        d3d_res_path = szDuiResPath;
#endif
#if 0
        std::wstring fx_path = d3d_res_path + L"Test.fx";
        hr = CompileShaderFromFile(fx_path.c_str(), "VS", "vs_4_0", &vertex_shader_blob);
        if (FAILED(hr)) {
            fx_path = L"Test.fx";
            hr = CompileShaderFromFile(fx_path.c_str(), "VS", "vs_4_0", &vertex_shader_blob);
            if (FAILED(hr)) {
                ::MessageBox(nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
                return hr;
            }
        }
#else
        hr = CompileShaderFromAscii(kTestFxShader, strlen(kTestFxShader), "VS", "vs_4_0", &vertex_shader_blob);
        if (FAILED(hr)) {
            ::MessageBox(nullptr,
                L"The kTestFxShader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }
#endif

        // Create the vertex shader
        hr = d3d_device_->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader_);
        if (FAILED(hr)) {
            vertex_shader_blob->Release();
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateVertexShader Failed", MB_OK);
            return hr;
        }

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT num_of_elements = ARRAYSIZE(layout);

        // Create the input layout
        hr = d3d_device_->CreateInputLayout(layout, num_of_elements, vertex_shader_blob->GetBufferPointer(),
            vertex_shader_blob->GetBufferSize(), &vertex_layout_);
        vertex_shader_blob->Release();
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateInputLayout Failed", MB_OK);
            return hr;
        }

        // Set the input layout
        immediate_context_->IASetInputLayout(vertex_layout_);

        // Compile the pixel shader
        ID3DBlob* pixel_shader_blob = nullptr;
#if 0
        hr = CompileShaderFromFile(fx_path.c_str(), "PS", "ps_4_0", &pixel_shader_blob);
        if (FAILED(hr)) {
            ::MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }
#else
        hr = CompileShaderFromAscii(kTestFxShader, strlen(kTestFxShader), "PS", "ps_4_0", &pixel_shader_blob);
        if (FAILED(hr)) {
            ::MessageBox(nullptr,
                L"The kTestFxShader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }
#endif

        // Create the pixel shader
        hr = d3d_device_->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader_);
        pixel_shader_blob->Release();
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreatePixelShader Failed", MB_OK);
            return hr;
        }

        // Create vertex buffer
        SimpleVertex vertices[] =
        {
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        };

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 24;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA init_data = {};
        init_data.pSysMem = vertices;
        hr = d3d_device_->CreateBuffer(&bd, &init_data, &vertex_buffer_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer VertexBuffer Failed", MB_OK);
            return hr;
        }

        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);

        // Create index buffer
        // Create vertex buffer
        WORD indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 36;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        init_data.pSysMem = indices;
        hr = d3d_device_->CreateBuffer(&bd, &init_data, &index_buffer_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer IndexBuffer Failed", MB_OK);
            return hr;
        }

        // Set index buffer
        immediate_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        immediate_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Create the constant buffers
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBNeverChanges);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &never_changes_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBNeverChanges Failed", MB_OK);
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangeOnResize);
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &change_on_resize_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBChangeOnResize Failed", MB_OK);
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &changes_every_frame_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBChangesEveryFrame Failed", MB_OK);
            return hr;
        }

        // Load the Texture
#if 0
        std::wstring tex_path = d3d_res_path + L"seafloor.dds";
        hr = CreateDDSTextureFromFile(d3d_device_, tex_path.c_str(), nullptr, &texture_rv_);
        if (FAILED(hr)) {
            tex_path = L"seafloor.dds";
            hr = CreateDDSTextureFromFile(d3d_device_, tex_path.c_str(), nullptr, &texture_rv_);
            if (FAILED(hr)) {
                std::wstring err_msg = GetLastErrorAsString();
                ::MessageBox(nullptr,
                    err_msg.c_str(), L"CreateDDSTextureFromFile Failed", MB_OK);
                return hr;
            }
        }
#else
        int expected_tex_w = texture_width_;
        int expected_tex_h = texture_height_;
        if (!texture_color_) {
            texture_color_ = new DWORD[expected_tex_w * expected_tex_h];
            SetTextureColor(RGB(255, 0, 0));
        }
        hr = CreateDynamicTextureWithPureColor(d3d_device_, texture_color_, 4 * expected_tex_w * expected_tex_h, expected_tex_w, expected_tex_h, &texture_, &texture_rv_, &texture_rtv_);

        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateTextureWithPureColor Failed", MB_OK);
            return hr;
        }
#endif

        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = d3d_device_->CreateSamplerState(&sampDesc, &sampler_linear_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateSamplerState Failed", MB_OK);
            return hr;
        }
    }
    return hr;
}
void CubeImplementor::UpdateResources()
{
    HWND hwnd = owner_->GetHWND();

    RECT rc;
    ::GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    if (last_width_ == width && last_height_ == height) {
        return;
    }

    // Initialize the world matrices
    world_matrix_ = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    view_matrix_ = XMMatrixLookAtLH(Eye, At, Up);

    CBNeverChanges never_changes;
    never_changes.view = XMMatrixTranspose(view_matrix_);
    if (never_changes_) {
        immediate_context_->UpdateSubresource(never_changes_, 0, nullptr, &never_changes, 0, 0);
    }

    // Initialize the projection matrix
    projection_matrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

    CBChangeOnResize change_on_resize;
    change_on_resize.projection = XMMatrixTranspose(projection_matrix_);
    if (change_on_resize_) {
        immediate_context_->UpdateSubresource(change_on_resize_, 0, nullptr, &change_on_resize, 0, 0);
    }
}
void CubeImplementor::CleanupD3DResources()
{
    if (sampler_linear_) sampler_linear_->Release();
    if (texture_rv_) texture_rv_->Release();
    if (texture_) texture_->Release();
    if (never_changes_) never_changes_->Release();
    if (change_on_resize_) change_on_resize_->Release();
    if (changes_every_frame_) changes_every_frame_->Release();
    if (vertex_buffer_) vertex_buffer_->Release();
    if (index_buffer_) index_buffer_->Release();
    if (vertex_layout_) vertex_layout_->Release();
    if (vertex_shader_) vertex_shader_->Release();
    if (pixel_shader_) pixel_shader_->Release();

    if (texture_color_) {
        delete[] texture_color_;
        texture_color_ = nullptr;
    }
}
void CubeImplementor::OnRender()
{
    if (!immediate_context_)
        return;

    // Update our time
    static float t = 0.0f;
    if (driver_type_ == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        t = (timeCur - timeStart) / 1000.0f;
    }

    // Rotate cube around the origin
    world_matrix_ = XMMatrixRotationY(t);

    // Modify the color
    mesh_color_.x = (sinf(t * 1.0f) + 1.0f) * 0.5f;
    mesh_color_.y = (cosf(t * 3.0f) + 1.0f) * 0.5f;
    mesh_color_.z = (sinf(t * 5.0f) + 1.0f) * 0.5f;

    //
    // Update variables that change once per frame
    //
    CBChangesEveryFrame cb;
    cb.world = XMMatrixTranspose(world_matrix_);
    cb.mesh_color = mesh_color_;
    if (changes_every_frame_) {
        immediate_context_->UpdateSubresource(changes_every_frame_, 0, nullptr, &cb, 0, 0);
    }

    if (texture_) {
        DWORD clear_color = RGB(std::rand() / 255, std::rand() / 255, std::rand() / 255);
        if (texture_rtv_) {
            float color[4];
            // Setup the color to clear the buffer to.
            color[0] = GetRValue(clear_color);
            color[1] = GetGValue(clear_color);
            color[2] = GetBValue(clear_color);
            color[3] = 0;

            // Clear the back buffer.
            immediate_context_->ClearRenderTargetView(texture_rtv_, color);
        }
        else if (texture_color_) {
            // change texure color
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            immediate_context_->Map(texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            SetTextureColor(clear_color, false);
            mapped_resource.RowPitch = 4 * texture_width_;
            mapped_resource.DepthPitch = 0;
            //for (int y = 0; y < texture_height_; y++) {
            memcpy(mapped_resource.pData, texture_color_, 4 * texture_width_ * texture_height_);
            //}
            immediate_context_->Unmap(texture_, 0);
        }
    }

    //
    // Render the cube
    //
    if (vertex_shader_) {
        immediate_context_->VSSetShader(vertex_shader_, nullptr, 0);
    }
    if (never_changes_) {
        immediate_context_->VSSetConstantBuffers(0, 1, &never_changes_);
    }
    if (change_on_resize_) {
        immediate_context_->VSSetConstantBuffers(1, 1, &change_on_resize_);
    }
    if (changes_every_frame_) {
        immediate_context_->VSSetConstantBuffers(2, 1, &changes_every_frame_);
    }
    if (pixel_shader_) {
        immediate_context_->PSSetShader(pixel_shader_, nullptr, 0);
    }
    if (changes_every_frame_) {
        immediate_context_->PSSetConstantBuffers(2, 1, &changes_every_frame_);
    }
    if (texture_rv_) {
        immediate_context_->PSSetShaderResources(0, 1, &texture_rv_);
    }
    if (sampler_linear_) {
        immediate_context_->PSSetSamplers(0, 1, &sampler_linear_);
    }
    if (index_buffer_) {
        immediate_context_->DrawIndexed(36, 0, 0);
    }
}

void CubeImplementor::SetTextureColor(DWORD pure_color, bool random)
{
    if (texture_color_) {
        for (int y = 0; y < texture_height_; y++) {
            for (int x = 0; x < texture_width_; x++) {
                if (random) {
                    texture_color_[x + y * texture_height_] = RGB(std::rand() / 255, std::rand() / 255, std::rand() / 255);
                }
                else {
                    texture_color_[x + y * texture_height_] = pure_color;
                }
            }
        }
    }
}



const char kRectFxShader[] = {
R"(
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbNeverChanges : register(b0)
{
    matrix View;
};

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample(samLinear, input.Tex);
}
)"
};

RectImplementor::RectImplementor(QuiD3D11Window * owner)
    : QuiD3D11Implementor(owner)
{

}
RectImplementor::~RectImplementor()
{

}

HRESULT RectImplementor::InitD3DResources()
{
    HRESULT hr = S_OK;

    if (!vertex_shader_) {
        // Compile the vertex shader
        ID3DBlob* vertex_shader_blob = nullptr;

        std::wstring d3d_res_path;
        hr = CompileShaderFromAscii(kRectFxShader, strlen(kRectFxShader), "VS", "vs_4_0", &vertex_shader_blob);
        if (FAILED(hr)) {
            ::MessageBox(nullptr,
                L"The kRectFxShader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        // Create the vertex shader
        hr = d3d_device_->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader_);
        if (FAILED(hr)) {
            vertex_shader_blob->Release();
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateVertexShader Failed", MB_OK);
            return hr;
        }

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT num_of_elements = ARRAYSIZE(layout);

        // Create the input layout
        hr = d3d_device_->CreateInputLayout(layout, num_of_elements, vertex_shader_blob->GetBufferPointer(),
            vertex_shader_blob->GetBufferSize(), &vertex_layout_);
        vertex_shader_blob->Release();
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateInputLayout Failed", MB_OK);
            return hr;
        }

        // Set the input layout
        immediate_context_->IASetInputLayout(vertex_layout_);

        // Compile the pixel shader
        ID3DBlob* pixel_shader_blob = nullptr;
        hr = CompileShaderFromAscii(kRectFxShader, strlen(kRectFxShader), "PS", "ps_4_0", &pixel_shader_blob);
        if (FAILED(hr)) {
            ::MessageBox(nullptr,
                L"The kRectFxShader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        // Create the pixel shader
        hr = d3d_device_->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader_);
        pixel_shader_blob->Release();
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreatePixelShader Failed", MB_OK);
            return hr;
        }

        // Create vertex buffer
        SimpleVertex vertices[] =
        {
            { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        };

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA init_data = {};
        init_data.pSysMem = vertices;
        hr = d3d_device_->CreateBuffer(&bd, &init_data, &vertex_buffer_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer VertexBuffer Failed", MB_OK);
            return hr;
        }

        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        immediate_context_->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);

        // Create index buffer
        // Create vertex buffer
        WORD indices[] =
        {
            0,1,2,
            2,3,0,
        };

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 6;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        init_data.pSysMem = indices;
        hr = d3d_device_->CreateBuffer(&bd, &init_data, &index_buffer_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer IndexBuffer Failed", MB_OK);
            return hr;
        }

        // Set index buffer
        immediate_context_->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        immediate_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Create the constant buffers
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBNeverChanges);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &never_changes_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBNeverChanges Failed", MB_OK);
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangeOnResize);
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &change_on_resize_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBChangeOnResize Failed", MB_OK);
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        hr = d3d_device_->CreateBuffer(&bd, nullptr, &changes_every_frame_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateBuffer CBChangesEveryFrame Failed", MB_OK);
            return hr;
        }

        int expected_tex_w = texture_width_;
        int expected_tex_h = texture_height_;
        if (!texture_color_) {
            texture_color_ = new DWORD[expected_tex_w * expected_tex_h];
            SetTextureColor(RGB(255, 0, 0));
        }
        hr = CreateDynamicTextureWithPureColor(d3d_device_, texture_color_, 4 * expected_tex_w * expected_tex_h, expected_tex_w, expected_tex_h, &texture_, &texture_rv_, &texture_rtv_);

        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateTextureWithPureColor Failed", MB_OK);
            return hr;
        }

        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = d3d_device_->CreateSamplerState(&sampDesc, &sampler_linear_);
        if (FAILED(hr)) {
            std::wstring err_msg = GetLastErrorAsString();
            ::MessageBox(nullptr,
                err_msg.c_str(), L"CreateSamplerState Failed", MB_OK);
            return hr;
        }
    }
    return hr;
}
void RectImplementor::UpdateResources()
{
    HWND hwnd = owner_->GetHWND();

    RECT rc;
    ::GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    if (last_width_ == width && last_height_ == height) {
        return;
    }

    // Initialize the world matrices
    world_matrix_ = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    view_matrix_ = XMMatrixLookAtLH(Eye, At, Up);

    CBNeverChanges never_changes;
    never_changes.view = XMMatrixTranspose(view_matrix_);
    if (never_changes_) {
        immediate_context_->UpdateSubresource(never_changes_, 0, nullptr, &never_changes, 0, 0);
    }

    // Initialize the projection matrix
    projection_matrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

    CBChangeOnResize change_on_resize;
    change_on_resize.projection = XMMatrixTranspose(projection_matrix_);
    if (change_on_resize_) {
        immediate_context_->UpdateSubresource(change_on_resize_, 0, nullptr, &change_on_resize, 0, 0);
    }
}
void RectImplementor::CleanupD3DResources()
{
    if (sampler_linear_) sampler_linear_->Release();
    if (texture_rv_) texture_rv_->Release();
    if (texture_) texture_->Release();
    if (never_changes_) never_changes_->Release();
    if (change_on_resize_) change_on_resize_->Release();
    if (changes_every_frame_) changes_every_frame_->Release();
    if (vertex_buffer_) vertex_buffer_->Release();
    if (index_buffer_) index_buffer_->Release();
    if (vertex_layout_) vertex_layout_->Release();
    if (vertex_shader_) vertex_shader_->Release();
    if (pixel_shader_) pixel_shader_->Release();

    if (texture_color_) {
        delete[] texture_color_;
        texture_color_ = nullptr;
    }
}
void RectImplementor::OnRender()
{
    if (!immediate_context_)
        return;

    // Update our time
    static float t = 0.0f;
    if (driver_type_ == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        t = (timeCur - timeStart) / 1000.0f;
    }

    // Rotate cube around the origin
    world_matrix_ = XMMatrixRotationY(t);

    // Modify the color
    mesh_color_.x = (sinf(t * 1.0f) + 1.0f) * 0.5f;
    mesh_color_.y = (cosf(t * 3.0f) + 1.0f) * 0.5f;
    mesh_color_.z = (sinf(t * 5.0f) + 1.0f) * 0.5f;

    //
    // Update variables that change once per frame
    //
    CBChangesEveryFrame cb;
    cb.world = XMMatrixTranspose(world_matrix_);
    cb.mesh_color = mesh_color_;
    if (changes_every_frame_) {
        immediate_context_->UpdateSubresource(changes_every_frame_, 0, nullptr, &cb, 0, 0);
    }

    if (texture_) {
        DWORD clear_color = RGB(std::rand() / 255, std::rand() / 255, std::rand() / 255);
        if (texture_rtv_) {
            float color[4];
            // Setup the color to clear the buffer to.
            color[0] = mesh_color_.x; //GetRValue(clear_color) / 255.0f;
            color[1] = mesh_color_.y; //GetGValue(clear_color) / 255.0f;
            color[2] = mesh_color_.z; //GetBValue(clear_color) / 255.0f;
            color[3] = 0;

            // Clear the back buffer.
            immediate_context_->ClearRenderTargetView(texture_rtv_, color);
        }
        else if (texture_color_) {
            // change texure color
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            immediate_context_->Map(texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            SetTextureColor(clear_color, false);
            mapped_resource.RowPitch = 4 * texture_width_;
            mapped_resource.DepthPitch = 0;
            //for (int y = 0; y < texture_height_; y++) {
            memcpy(mapped_resource.pData, texture_color_, 4 * texture_width_ * texture_height_);
            //}
            immediate_context_->Unmap(texture_, 0);
        }
    }

    //
    // Render the cube
    //
    if (vertex_shader_) {
        immediate_context_->VSSetShader(vertex_shader_, nullptr, 0);
    }
    if (never_changes_) {
        immediate_context_->VSSetConstantBuffers(0, 1, &never_changes_);
    }
    if (change_on_resize_) {
        immediate_context_->VSSetConstantBuffers(1, 1, &change_on_resize_);
    }
    if (changes_every_frame_) {
        immediate_context_->VSSetConstantBuffers(2, 1, &changes_every_frame_);
    }
    if (pixel_shader_) {
        immediate_context_->PSSetShader(pixel_shader_, nullptr, 0);
    }
    if (changes_every_frame_) {
        immediate_context_->PSSetConstantBuffers(2, 1, &changes_every_frame_);
    }
    if (texture_rv_) {
        immediate_context_->PSSetShaderResources(0, 1, &texture_rv_);
    }
    if (sampler_linear_) {
        immediate_context_->PSSetSamplers(0, 1, &sampler_linear_);
    }
    if (index_buffer_) {
        immediate_context_->DrawIndexed(6, 0, 0);
    }
}

void RectImplementor::SetTextureColor(DWORD pure_color, bool random)
{
    if (texture_color_) {
        for (int y = 0; y < texture_height_; y++) {
            for (int x = 0; x < texture_width_; x++) {
                if (random) {
                    texture_color_[x + y * texture_height_] = RGB(std::rand() / 255, std::rand() / 255, std::rand() / 255);
                }
                else {
                    texture_color_[x + y * texture_height_] = pure_color;
                }
            }
        }
    }
}
