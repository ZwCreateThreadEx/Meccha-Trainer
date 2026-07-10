#include "hook.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

static LRESULT __stdcall wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (Globals::Settings::g_menu)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
            return TRUE;
    }

    return CallWindowProc(gui::hook::original_wndproc, hwnd, msg, wparam, lparam);
}

bool gui::hook::initialize_d3d12(IDXGISwapChain* sc)
{
    IDXGISwapChain3* sc3 = nullptr;
    if (FAILED(sc->QueryInterface(IID_PPV_ARGS(&sc3))))
        return false;

    swap_chain = sc3;

    if (FAILED(swap_chain->GetDevice(IID_PPV_ARGS(&device))))
        return false;

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swap_chain->GetDesc(&desc)))
        return false;

    window = desc.OutputWindow;
    back_buffer_count = static_cast<int>(desc.BufferCount);
    back_buffer_format = desc.BufferDesc.Format;

    original_wndproc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc))
        );

    D3D12_DESCRIPTOR_HEAP_DESC rtv_desc{};
    rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_desc.NumDescriptors = back_buffer_count;
    rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtv_desc.NodeMask = 1;
    if (FAILED(device->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&rtv_heap))))
        return false;

    D3D12_DESCRIPTOR_HEAP_DESC srv_desc{};
    srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_desc.NumDescriptors = 3;
    srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srv_desc.NodeMask = 1;
    if (FAILED(device->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&srv_heap))))
        return false;

    frame_contexts = new FrameContext[back_buffer_count];

    UINT rtv_increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();

    for (int i = 0; i < back_buffer_count; i++)
    {
        if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&frame_contexts[i].CommandAllocator))))
            return false;

        frame_contexts[i].FenceValue = 0;
        frame_contexts[i].MainRenderTargetResource = nullptr;
        frame_contexts[i].MainRenderTargetDescriptor = rtv_handle;
        frame_contexts[i].BackupResource = nullptr;

        rtv_handle.ptr += rtv_increment;
    }

    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        frame_contexts[0].CommandAllocator, nullptr,
        IID_PPV_ARGS(&command_list))))
        return false;
    command_list->Close();

    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
        return false;

    fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fence_event)
        return false;

    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&game_fence))))
        return false;

    game_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!game_fence_event)
        return false;

    return true;
}

void gui::hook::create_render_targets()
{
    if (!swap_chain || !device || !frame_contexts)
        return;

    for (int i = 0; i < back_buffer_count; i++)
    {
        if (frame_contexts[i].BackupResource)
        {
            frame_contexts[i].BackupResource->Release();
            frame_contexts[i].BackupResource = nullptr;
        }
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    swap_chain->GetDesc(&desc);

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC backupDesc = {};
    backupDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    backupDesc.Width = desc.BufferDesc.Width;
    backupDesc.Height = desc.BufferDesc.Height;
    backupDesc.DepthOrArraySize = 1;
    backupDesc.MipLevels = 1;
    backupDesc.Format = back_buffer_format;
    backupDesc.SampleDesc.Count = 1;
    backupDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    for (int i = 0; i < back_buffer_count; i++)
    {
        ID3D12Resource* backBuffer = nullptr;
        if (SUCCEEDED(swap_chain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
        {
            device->CreateRenderTargetView(backBuffer, nullptr,
                frame_contexts[i].MainRenderTargetDescriptor);
            frame_contexts[i].MainRenderTargetResource = backBuffer;

            device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &backupDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&frame_contexts[i].BackupResource));
        }
    }
}

void gui::hook::cleanup_render_targets()
{
    if (!frame_contexts)
        return;
    for (int i = 0; i < back_buffer_count; i++)
    {
        if (frame_contexts[i].MainRenderTargetResource)
        {
            frame_contexts[i].MainRenderTargetResource->Release();
            frame_contexts[i].MainRenderTargetResource = nullptr;
        }
        if (frame_contexts[i].BackupResource)
        {
            frame_contexts[i].BackupResource->Release();
            frame_contexts[i].BackupResource = nullptr;
        }
    }
}

void gui::hook::init_imgui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    init();
    io.Fonts->Build();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX12_Init(device, back_buffer_count,
        back_buffer_format, srv_heap,
        srv_heap->GetCPUDescriptorHandleForHeapStart(),
        srv_heap->GetGPUDescriptorHandleForHeapStart());
    ImGui_ImplDX12_CreateDeviceObjects();
}

void gui::hook::render_ui()
{
    ImGui::GetIO().MouseDrawCursor = Globals::Settings::g_menu;

    if (Globals::Settings::g_menu)
    {
        ::ShowCursor(TRUE);
        ::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
    }
    else
    {
        ::ShowCursor(FALSE);
    }

    render();
}

static int ImGuiKeyToVK(int imguiKey)
{
    if (imguiKey >= ImGuiKey_MouseLeft && imguiKey <= ImGuiKey_MouseX2)
        return VK_LBUTTON + (imguiKey - ImGuiKey_MouseLeft);
    if (imguiKey >= ImGuiKey_A && imguiKey <= ImGuiKey_Z)
        return 'A' + (imguiKey - ImGuiKey_A);
    if (imguiKey >= ImGuiKey_0 && imguiKey <= ImGuiKey_9)
        return '0' + (imguiKey - ImGuiKey_0);
    if (imguiKey >= ImGuiKey_F1 && imguiKey <= ImGuiKey_F24)
        return VK_F1 + (imguiKey - ImGuiKey_F1);
    if (imguiKey >= ImGuiKey_Keypad0 && imguiKey <= ImGuiKey_Keypad9)
        return VK_NUMPAD0 + (imguiKey - ImGuiKey_Keypad0);
    switch (imguiKey) {
    case ImGuiKey_LeftShift: return VK_LSHIFT;
    case ImGuiKey_RightShift: return VK_RSHIFT;
    case ImGuiKey_LeftCtrl: return VK_LCONTROL;
    case ImGuiKey_RightCtrl: return VK_RCONTROL;
    case ImGuiKey_LeftAlt: return VK_LMENU;
    case ImGuiKey_RightAlt: return VK_RMENU;
    case ImGuiKey_Space: return VK_SPACE;
    case ImGuiKey_Enter: return VK_RETURN;
    case ImGuiKey_Escape: return VK_ESCAPE;
    case ImGuiKey_Tab: return VK_TAB;
    case ImGuiKey_Backspace: return VK_BACK;
    case ImGuiKey_Insert: return VK_INSERT;
    case ImGuiKey_Delete: return VK_DELETE;
    case ImGuiKey_Home: return VK_HOME;
    case ImGuiKey_End: return VK_END;
    case ImGuiKey_PageUp: return VK_PRIOR;
    case ImGuiKey_PageDown: return VK_NEXT;
    case ImGuiKey_LeftArrow: return VK_LEFT;
    case ImGuiKey_RightArrow: return VK_RIGHT;
    case ImGuiKey_UpArrow: return VK_UP;
    case ImGuiKey_DownArrow: return VK_DOWN;
    case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
    case ImGuiKey_KeypadDivide: return VK_DIVIDE;
    case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
    case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
    case ImGuiKey_KeypadAdd: return VK_ADD;
    case ImGuiKey_KeypadEnter: return VK_RETURN;
    case ImGuiKey_GraveAccent: return VK_OEM_3;
    case ImGuiKey_Minus: return VK_OEM_MINUS;
    case ImGuiKey_Equal: return VK_OEM_PLUS;
    case ImGuiKey_LeftBracket: return VK_OEM_4;
    case ImGuiKey_RightBracket: return VK_OEM_6;
    case ImGuiKey_Backslash: return VK_OEM_5;
    case ImGuiKey_Semicolon: return VK_OEM_1;
    case ImGuiKey_Apostrophe: return VK_OEM_7;
    case ImGuiKey_Comma: return VK_OEM_COMMA;
    case ImGuiKey_Period: return VK_OEM_PERIOD;
    case ImGuiKey_Slash: return VK_OEM_2;
    default: return 0;
    }
}

HRESULT __stdcall gui::hook::present_detour(IDXGISwapChain* sc, UINT sync_interval, UINT flags)
{
    if (!initialized)
    {
        if (!initialize_d3d12(sc))
            return original_present(sc, sync_interval, flags);

        create_render_targets();
        init_imgui();
        initialized = true;
    }

    if (!command_queue || !fence || !game_fence)
        return original_present(sc, sync_interval, flags);

    const UINT64 waitGameValue = game_fence_value;
    if (game_fence->GetCompletedValue() < waitGameValue)
    {
        game_fence->SetEventOnCompletion(waitGameValue, game_fence_event);
        WaitForSingleObject(game_fence_event, INFINITE);
    }

    UINT backBufferIdx = swap_chain->GetCurrentBackBufferIndex();
    FrameContext& frameCtx = frame_contexts[backBufferIdx];

    UINT64 completed = fence->GetCompletedValue();
    if (frameCtx.FenceValue != 0 && completed < frameCtx.FenceValue)
    {
        fence->SetEventOnCompletion(frameCtx.FenceValue, fence_event);
        WaitForSingleObject(fence_event, INFINITE);
    }

    frameCtx.CommandAllocator->Reset();
    command_list->Reset(frameCtx.CommandAllocator, nullptr);

    ID3D12Resource* backBuffer = frameCtx.MainRenderTargetResource;
    ID3D12Resource* backup = frameCtx.BackupResource;

    D3D12_RESOURCE_BARRIER barrierCopy = {};
    barrierCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierCopy.Transition.pResource = backBuffer;
    barrierCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrierCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

    D3D12_RESOURCE_BARRIER barrierBackup = {};
    barrierBackup.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierBackup.Transition.pResource = backup;
    barrierBackup.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierBackup.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrierBackup.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    D3D12_RESOURCE_BARRIER barriers[] = { barrierCopy, barrierBackup };
    command_list->ResourceBarrier(2, barriers);

    command_list->CopyResource(backup, backBuffer);

    barrierCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrierCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    command_list->ResourceBarrier(1, &barrierCopy);

    command_list->CopyResource(backBuffer, backup);

    barrierCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrierCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierBackup.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrierBackup.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_BARRIER finalBarriers[] = { barrierCopy, barrierBackup };
    command_list->ResourceBarrier(2, finalBarriers);

    command_list->SetDescriptorHeaps(1, &srv_heap);
    command_list->OMSetRenderTargets(1, &frameCtx.MainRenderTargetDescriptor, FALSE, nullptr);

    int menuVK = ImGuiKeyToVK(static_cast<int>(Globals::Settings::menuKeybind));

    static bool wasDown = false;

    bool isDown = (GetAsyncKeyState(menuVK) & 0x8000) != 0;

    if (isDown && !wasDown)
    {
        Globals::Settings::g_menu = !Globals::Settings::g_menu;
    }

    wasDown = isDown;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    render_ui();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);

    barrierCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list->ResourceBarrier(1, &barrierCopy);

    command_list->Close();

    ID3D12CommandList* const cmds[] = { command_list };
    command_queue->ExecuteCommandLists(1, cmds);

    frameCtx.FenceValue = ++fence_value;
    command_queue->Signal(fence, frameCtx.FenceValue);

    return original_present(sc, sync_interval, flags);
}

void __stdcall gui::hook::execute_command_lists_detour(ID3D12CommandQueue* queue, UINT num, ID3D12CommandList* const* lists)
{
    if (!command_queue)
        command_queue = queue;

    original_execute_command_lists(queue, num, lists);

    if (queue == command_queue && game_fence)
    {
        ++game_fence_value;
        queue->Signal(game_fence, game_fence_value);
    }
}

HRESULT __stdcall gui::hook::signal_detour(ID3D12CommandQueue* queue, ID3D12Fence* f, UINT64 val)
{
    if (queue == command_queue && f == fence)
        fence_value = val;
    return original_signal(queue, f, val);
}

HRESULT __stdcall gui::hook::resize_buffers_detour(IDXGISwapChain3* sc, UINT count, UINT w, UINT h, DXGI_FORMAT fmt, UINT flags)
{
    if (device) ImGui_ImplDX12_InvalidateDeviceObjects();
    cleanup_render_targets();
    HRESULT hr = original_resize_buffers(sc, count, w, h, fmt, flags);
    if (SUCCEEDED(hr))
    {
        back_buffer_count = count;
        back_buffer_format = fmt;
        create_render_targets();
        if (device) ImGui_ImplDX12_CreateDeviceObjects();
    }
    return hr;
}

void gui::hook::attach()
{
    while (true)
    {
        if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
        {
            kiero::bind(54, (void**)&original_execute_command_lists, execute_command_lists_detour);
            kiero::bind(58, (void**)&original_signal, signal_detour);
            kiero::bind(140, (void**)&original_present, present_detour);
            kiero::bind(145, (void**)&original_resize_buffers, resize_buffers_detour);
            break;
        }
    }
}