#pragma once

#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <include.h>

namespace gui::hook
{
    inline HRESULT(__stdcall* original_present)(IDXGISwapChain*, UINT, UINT) = nullptr;
    inline void(__stdcall* original_execute_command_lists)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*) = nullptr;
    inline HRESULT(__stdcall* original_signal)(ID3D12CommandQueue*, ID3D12Fence*, UINT64) = nullptr;
    inline HRESULT(__stdcall* original_resize_buffers)(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT) = nullptr;
    inline WNDPROC original_wndproc = nullptr;

    inline ID3D12Device* device = nullptr;
    inline ID3D12CommandQueue* command_queue = nullptr;
    inline ID3D12GraphicsCommandList* command_list = nullptr;
    inline ID3D12Fence* fence = nullptr;
    inline ID3D12Fence* game_fence = nullptr;

    inline HANDLE fence_event = nullptr;
    inline HANDLE game_fence_event = nullptr;
    inline UINT64 fence_value = 0;
    inline UINT64 game_fence_value = 0;

    inline ID3D12DescriptorHeap* rtv_heap = nullptr;
    inline ID3D12DescriptorHeap* srv_heap = nullptr;

    inline IDXGISwapChain3* swap_chain = nullptr;

    inline HWND window = nullptr;
    inline bool initialized = false;
    inline DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;

    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator = nullptr;
        UINT64 FenceValue = 0;
        ID3D12Resource* MainRenderTargetResource = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE MainRenderTargetDescriptor = {};
        ID3D12Resource* BackupResource = nullptr;
    };

    inline FrameContext* frame_contexts = nullptr;
    inline int back_buffer_count = 0;

    bool initialize_d3d12(IDXGISwapChain* sc);

    void create_render_targets();
    void cleanup_render_targets();

    void init_imgui();
    void render_ui();

    HRESULT __stdcall present_detour(IDXGISwapChain*, UINT, UINT);
    void __stdcall execute_command_lists_detour(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
    HRESULT __stdcall signal_detour(ID3D12CommandQueue*, ID3D12Fence*, UINT64);
    HRESULT __stdcall resize_buffers_detour(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    void attach();

    void init();
    void render();
}