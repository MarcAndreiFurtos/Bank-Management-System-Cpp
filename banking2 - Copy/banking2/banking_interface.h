#pragma once
#include<d3d9.h>
#include"b_header.h"

namespace gui{
    //constant window size
    constexpr int Width =900;
    constexpr int Height=900;
    inline bool exit = true;

    //winapi window vars
    inline HWND window = nullptr;
    inline WNDCLASSEXA windowClass = { };

    //points for window moovement
    inline POINTS position = { };

    //directx state vars
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS presentParameters = { };

    //creation and destruction of window
    void Create_h_window(const char* window_name ,const char* class_name) noexcept;
    void Destroy_h_window() noexcept;

    //creation and destruction of device
    bool Create_device() noexcept;
    void Reset_device() noexcept;
    void Destroy_device() noexcept;

    //create imgui and destroy imgui
    void Create_imgui() noexcept;
    void Destroy_imgui() noexcept;
    
    //rendering logic
    void Begin_render() noexcept;
    void End_render() noexcept;
    void Render(Node* head) noexcept;
    
}