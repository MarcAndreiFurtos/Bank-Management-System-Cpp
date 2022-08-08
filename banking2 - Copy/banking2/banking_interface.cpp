#include <iostream>
#include "imgui_code/imgui.h"
#include "imgui_code/imgui_impl_dx9.h"
#include "imgui_code/imgui_impl_win32.h"
#include "banking_interface.h"
#include "b_header.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window , UINT message , WPARAM wideParameter , LPARAM longParameter);

long __stdcall Window_process(HWND window , UINT message , WPARAM wideParameter , LPARAM longParameter){

    if(ImGui_ImplWin32_WndProcHandler(window , message , wideParameter , longParameter)){
        return true;
    }

    switch (message){
        //disable ALT app menu
        case WM_SYSCOMMAND:{
            if((wideParameter&0xfff0)==SC_KEYMENU){
                return 0;
            }
            break;
        }
        case WM_DESTROY :{
            PostQuitMessage(0);
            break;
        }
        //set click points
        case WM_LBUTTONDOWN: {
            gui::position = MAKEPOINTS(longParameter); 
        }
        case WM_MOUSEMOVE: {
            if(wideParameter==MK_LBUTTON){
                const auto points = MAKEPOINTS(longParameter);
                auto rect = ::RECT{ };
                GetWindowRect(gui::window , &rect);
                rect.left += points.x - gui::position.x;
                rect.top += points.y - gui::position.y;

                if (gui::position.x >= 0 && gui::position.y <= gui::Width && gui::position.y >= 0 && gui::position.y <= 19){
                    SetWindowPos(gui::window , HWND_TOPMOST , rect.left , rect.top , 0 , 0 , SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
                }
            }
        }

    }

    return DefWindowProc(window , message , wideParameter , longParameter);
}

//creation and destruction of window
void gui::Create_h_window(const char* window_name ,const char* class_name) noexcept{
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = Window_process;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(0);
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = class_name;
    windowClass.hIconSm = 0;

    RegisterClassExA(&windowClass);

    window = CreateWindowA(class_name , window_name , WS_POPUP , 100 , 100 ,Width  , Height , 0 , 0 , windowClass.hInstance , 0);

    ShowWindow(window ,SW_SHOWDEFAULT);

    UpdateWindow(window);
}

void  gui::Destroy_h_window() noexcept{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName , windowClass.hInstance);
}

 //creation and destruction of device
bool  gui::Create_device() noexcept{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if(!d3d){
        return false;
    }

    ZeroMemory(&presentParameters , sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if(d3d->CreateDevice(D3DADAPTER_DEFAULT ,D3DDEVTYPE_HAL , window , D3DCREATE_HARDWARE_VERTEXPROCESSING , &presentParameters , &device)<0){
        return false;
    }
 return true;
}

void  gui::Reset_device() noexcept{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    
    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL){
        IM_ASSERT(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

void  gui::Destroy_device() noexcept{
    if (device){
        device->Release();
        device = nullptr;
    }
    if(d3d){
        d3d->Release();
        d3d = nullptr;
    }
}

//create imgui and destroy imgui
void  gui::Create_imgui() noexcept{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = NULL;
    ImFont* Arial = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);
   

    ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void  gui::Destroy_imgui() noexcept{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//rendering logic
void  gui::Begin_render() noexcept{

    //get windows message translate and dispach it
    MSG message;
    while (PeekMessage(&message , 0 , 0 , 0 , PM_REMOVE)){
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    //start the ImGui frame 
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

}

void  gui::End_render() noexcept{
    //begins scene ends scene and presents scene
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE , FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE , FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE , FALSE);

    device->Clear(0 , 0 , D3DCLEAR_TARGET | D3DCLEAR_TARGET , D3DCOLOR_RGBA(0 , 0 , 0 , 225) , 1.0f , 0);

    if(device->BeginScene() >= 0){
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0 , 0 , 0 , 0);

    //handle loss of D3D9 device
    if(result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET){
        Reset_device();
    }

}
//function that renters the window itself and all its contents it also links the ui to the linked list
void  gui::Render(Node *head) noexcept{
    ImGui::SetNextWindowPos({0 , 0});
    ImGui::SetNextWindowSize({Width , Height});
    ImGui::Begin("welcome to banking online" , &exit , ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoMove);
    
    //creating variables that we will need to create the ui and load the linked list

    static int deposit;
    static int withdraw;
    static bool exists;
    static bool a;
    static bool b;
    static bool c;
    static bool d;
    static int acc_nr=0;
    static char acc_pass[50];
    static char hold_name[50];

    //initial window setup

    ImGui::Text("Please input your account name ,number and password: ");
    ImGui::InputText("User Name", hold_name, 50);
    ImGui::InputInt("Account Number", &acc_nr, 50);
    ImGui::InputText("Password", acc_pass, 50);
   
    //doing almost the same thing twice one time if the account exists within the linked list (we just log in) 
    //and one time if it does not (we create the account)
    
    if (ImGui::Button("Register")) {
        a = true;
    }

    if (a == true) {

        if (Funk::check_existance(head, acc_nr, hold_name, acc_pass) == true) {
            exists = true;
            ImGui::Text("Account allready exists: ");

            //the curent node will always be the head because 

            if (head->holder_name == hold_name) {
                ImGui::Text("Account holder name: %s", hold_name);
            }
            if (head->account_password == acc_pass) {
                ImGui::Text("Password: %s", acc_pass);
            }
            if (head->account_number == acc_nr) {
                ImGui::Text("Account number: %d", acc_nr);
            }

            int acc_sum = head->account_sum;

            ImGui::Text("Ballance: %d $", acc_sum);

            if (ImGui::Button("See options")) {
                b = true;
            }
            if (b == true) {
                ImGui::SetNextWindowPos({ 250 , 125 });
                ImGui::SetNextWindowSize({ 500 , 600 });
                ImGui::Begin("Please select an option", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
                ImGui::Text("How much would you like to withdraw?");
                ImGui::InputInt("Withdrawl sum", &withdraw);

                if (ImGui::Button("withdraw money") == true) {
                    c = true;
                }

                if (c == true) {


                    head->account_sum = head->account_sum - withdraw;
                    int acc_sum = head->account_sum;
                    ImGui::Text("Ballance: %d $", acc_sum);
                    ImGui::Text("Thank You!");
                }

                ImGui::Text("How much would you like to Deposit?");
                ImGui::InputInt("Deposit sum", &deposit);

                if (ImGui::Button("Deposit money") == true) {
                    d = true;
                }

                if (d == true) {


                    head->account_sum = head->account_sum + deposit;
                    int acc_sum2 = head->account_sum;
                    ImGui::Text("Ballance: %d $", acc_sum2);
                    ImGui::Text("Thank You!");
                }

            }
            else if (b == false) {
                ImGui::End();
            }

        }
        else {
            exists = false;
            ImGui::Text("Creating new account....");
            Funk::add_before_head(head, acc_nr, hold_name, acc_pass);
            ImGui::Text("Account created: ");

            if (head->holder_name == hold_name) {
                 ImGui::Text("Account holder name: %s", hold_name);
            }
            if (head->account_password == acc_pass) {
                ImGui::Text("Password: %s", acc_pass);
            }
            if (head->account_number == acc_nr) {
                ImGui::Text("Account number: %d", acc_nr);
            }

            int acc_sum = head->account_sum;

            ImGui::Text("Ballance: %d $", acc_sum);

            if (ImGui::Button("See options")) {
                b = true;
            }
            if (b == true) {
                ImGui::SetNextWindowPos({ 250 , 125 });
                ImGui::SetNextWindowSize({ 500 , 600 });
                ImGui::Begin("Please select an option" , &exit , ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
                ImGui::Text("How much would you like to withdraw?");
                ImGui::InputInt("Withdrawl sum", &withdraw);

                if (ImGui::Button("withdraw money") == true) {
                    c = true;
                }

                if (c == true) {

                    
                    head->account_sum = head->account_sum - withdraw;
                    int acc_sum = head->account_sum;
                    ImGui::Text("Ballance: %d $", acc_sum);
                    ImGui::Text("Thank You!");
                }

                ImGui::Text("How much would you like to Deposit?");
                ImGui::InputInt("Deposit sum", &deposit);

                if (ImGui::Button("Deposit money") == true) {
                    d = true;
                }

                if (d == true) {


                    head->account_sum = head->account_sum + deposit;
                    int acc_sum2 = head->account_sum;
                    ImGui::Text("Ballance: %d $", acc_sum2);
                    ImGui::Text("Thank You!");
                }
            }
            else if (b == false) {
                ImGui::End();
            }
            
        }

            
    }
    
   
    ImGui::End();
}


