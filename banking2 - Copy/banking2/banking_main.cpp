#include <iostream>
#include <string>
#include <string.h>
#include "banking_interface.h"
#include <thread>
#include "imgui_code/imgui.h"
#include "imgui_code/imgui_impl_dx9.h"
#include "imgui_code/imgui_impl_win32.h"
#include "b_header.h"


/*
READ ME:
this code creates a gui that takes information from the keyboard and populates a linked list with it ,
the information refers to a clients name acc nr and password it takes them all from a textbox checks all the nodes of the linked list,
if it finds them it says the account allready exists and treats it as a login skipping straight to the place where you make withdrawls and deposits
if it doesnt find it it uses a diferent function to create a new node with the curent data and add it before the head this whole thing is handled
by the b_header.cpp and b_header.h files this is all the simple part the hard part came from the using Dear ImGui to create a gui 
banking_main.cpp,banking_interface.h and banking_interface.cpp handle that the rest are imgui library files

Enjoy using this software and modifying it to suit your needs
*/

int __stdcall wWinMain(HINSTANCE instance , HINSTANCE previousInstance , PWSTR arguments , int commandShow){

    //create gui
    gui::Create_h_window("test" , "test class");
    gui::Create_device();
    gui::Create_imgui();

    //head node
    Node* head = NULL;

    //rendering loop
    while(gui::exit){
        gui::Begin_render();
        gui::Render(head);
        gui::End_render();

       std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  

    gui::Destroy_imgui();
    gui::Destroy_device();
    gui::Destroy_h_window();

    return EXIT_SUCCESS;
}
