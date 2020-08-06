#pragma once

#include "qui/d3d11/qui_d3d11_window.h"

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <dcomp.h>

class QuiD3D11Implementor {
public:
    QuiD3D11Implementor(QuiD3D11Window * owner);
    virtual ~QuiD3D11Implementor();

    void * operator new(size_t sz) { return _mm_malloc(sz, 16); }
    void operator delete(void *ptr) { _mm_free(ptr); }

public:
    HRESULT InitDevice();
    virtual HRESULT InitD3DResources() { return S_OK; }
    virtual void UpdateResources() {}
    void CleanupDevice();
    virtual void CleanupD3DResources() {}
    void Render();
    virtual void OnRender() {}

protected:
    D3D_DRIVER_TYPE driver_type_{ D3D_DRIVER_TYPE_NULL };
    D3D_FEATURE_LEVEL feature_level_{ D3D_FEATURE_LEVEL_11_0 };
    ID3D11Device* d3d_device_{ nullptr };
    ID3D11Device1* d3d_device1_{ nullptr };
    ID3D11DeviceContext* immediate_context_{ nullptr };
    ID3D11DeviceContext1* immediate_context1_{ nullptr };
    IDXGISwapChain* dxgi_swapchain_{ nullptr };
    IDXGISwapChain1* dxgi_swapchain1_{ nullptr };
    ID3D11RenderTargetView* render_target_view_{ nullptr };
    ID3D11Texture2D* depth_stencil_{ nullptr };
    ID3D11DepthStencilView* depth_stencil_view_{ nullptr };
    IDXGIFactory1* dxgi_factory_{ nullptr };
    IDXGIFactory2* dxgi_factory2_{ nullptr };
    //IDCompositionDevice* m_pDevice = nullptr;
    //IDCompositionTarget* m_pCompositionTarget = nullptr;
    //IDCompositionVisual* m_pVisual = nullptr;

    QuiD3D11Window * owner_{ nullptr };
    UINT last_width_{ 0 };
    UINT last_height_{ 0 };
};

class CubeImplementor : public QuiD3D11Implementor {
public:
    CubeImplementor(QuiD3D11Window * owner);
    ~CubeImplementor() override;

public:
    HRESULT InitD3DResources() override;
    void UpdateResources() override;
    void CleanupD3DResources() override;
    void OnRender() override;

protected:
    void SetTextureColor(DWORD pure_color, bool random = false);

protected:
    ID3D11VertexShader* vertex_shader_{ nullptr };
    ID3D11PixelShader* pixel_shader_{ nullptr };
    ID3D11InputLayout* vertex_layout_{ nullptr };
    ID3D11Buffer* vertex_buffer_{ nullptr };
    ID3D11Buffer* index_buffer_{ nullptr };
    ID3D11Buffer* never_changes_{ nullptr };
    ID3D11Buffer* change_on_resize_{ nullptr };
    ID3D11Buffer* changes_every_frame_{ nullptr };
    ID3D11Resource* texture_{ nullptr };
    ID3D11ShaderResourceView* texture_rv_{ nullptr };
    ID3D11RenderTargetView* texture_rtv_{ nullptr };
    ID3D11SamplerState* sampler_linear_{ nullptr };
    DirectX::XMMATRIX world_matrix_;
    DirectX::XMMATRIX view_matrix_;
    DirectX::XMMATRIX projection_matrix_;
    DirectX::XMFLOAT4 mesh_color_;

    DWORD* texture_color_{ nullptr };
    int texture_width_{ 128 };
    int texture_height_{ 128 };
};

class RectImplementor : public QuiD3D11Implementor {
public:
    RectImplementor(QuiD3D11Window * owner);
    ~RectImplementor() override;

public:
    HRESULT InitD3DResources() override;
    void UpdateResources() override;
    void CleanupD3DResources() override;
    void OnRender() override;

protected:
    void SetTextureColor(DWORD pure_color, bool random = false);

protected:
    ID3D11VertexShader* vertex_shader_{ nullptr };
    ID3D11PixelShader* pixel_shader_{ nullptr };
    ID3D11InputLayout* vertex_layout_{ nullptr };
    ID3D11Buffer* vertex_buffer_{ nullptr };
    ID3D11Buffer* index_buffer_{ nullptr };
    ID3D11Buffer* never_changes_{ nullptr };
    ID3D11Buffer* change_on_resize_{ nullptr };
    ID3D11Buffer* changes_every_frame_{ nullptr };
    ID3D11Resource* texture_{ nullptr };
    ID3D11ShaderResourceView* texture_rv_{ nullptr };
    ID3D11RenderTargetView* texture_rtv_{ nullptr };
    ID3D11SamplerState* sampler_linear_{ nullptr };
    DirectX::XMMATRIX world_matrix_;
    DirectX::XMMATRIX view_matrix_;
    DirectX::XMMATRIX projection_matrix_;
    DirectX::XMFLOAT4 mesh_color_;

    DWORD* texture_color_{ nullptr };
    int texture_width_{ 128 };
    int texture_height_{ 128 };
};

