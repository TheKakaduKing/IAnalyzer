#include "render.hpp"
#include "analyzer1.hpp"
#include "analyzer2.hpp"
#include "analyzer3.hpp"
#include "analyzer4.hpp"
#include <iostream>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include "implot/implot.h"



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}



//Array and function to adjust the window size for the 4 imgui windows
static float WindowPosSize[10]{}; 

void setWindowPosSize(float* arr, int w, int h, GLFWwindow* win);
//Callback function for resizing a window
//will call the setWindowPosSize function to adjust the window size for the 4 imgui windows
void resizefunc(GLFWwindow* win, int w, int h);
void initWindows(float* arr, int w, int h){
  float wf = static_cast<float>(w);
  float hf = static_cast<float>(h);
  arr[0] = static_cast<float>(w)/2;
  arr[1] = static_cast<float>(h)/2;
  arr[2] = 0.0; arr[3] = 0.0;
  arr[4] = wf/2.0; arr[5] = 0.0;
  arr[6] = 0.0; arr[7] = hf/2;
  arr[8] = wf/2; arr[9] = hf/2;
}


int renderGLFWWindow()
{
  

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 460 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // The following 2 lines prevent implot from lagging after resizing the glfw window
    // why? i dont know. deep dive into opengl and glfw window cntext :)
    // still lagging...
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "imguiAnalyzer", nullptr, nullptr);

    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    
    glfwSwapInterval(1); // Enable vsync
                         
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //
    // Setup ImPlot context
    ImPlot::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);

    // Main loop
      
    //Getting notified if the window is resized 
    glfwSetWindowSizeCallback(window, resizefunc);
    int initWidth{0},initHeight{0};
    glfwGetWindowSize(window, &initWidth, &initHeight);
    initWindows(WindowPosSize, initWidth, initHeight);


    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
 
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            static float f = 0.0f;
            static int counter = 0;
            ImVec2 windowSize(WindowPosSize[0],WindowPosSize[1]);
            ImVec2 windowPosition1(WindowPosSize[2],WindowPosSize[3]);
            ImVec2 windowPosition2(WindowPosSize[4],WindowPosSize[5]);
            ImVec2 windowPosition3(WindowPosSize[6],WindowPosSize[7]);
            ImVec2 windowPosition4(WindowPosSize[8],WindowPosSize[9]);

        {
          //actual window
          windowAnalyzer1(windowSize, windowPosition1);
        }
        {
          //actual window
          windowAnalyzer2(windowSize, windowPosition2);
        }
        {
          //actual window
          windowAnalyzer3(windowSize, windowPosition3);
        }
        {
          //actual window
          windowAnalyzer4(windowSize, windowPosition4);
        }


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwMakeContextCurrent(window);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();


    glfwDestroyWindow(window);

    // this is called in ImGui_ImplGlfw_Shutdown()
    // glfwTerminate();

    return 0;
}




void refreshWindowPosSize(float* arr, int w, int h){
  float wf = static_cast<float>(w);
  float hf = static_cast<float>(h);

  arr[0] = static_cast<float>(w)/2;
  arr[1] = static_cast<float>(h)/2;
  arr[2] = 0.0; arr[3] = 0.0;
  arr[4] = wf/2.0; arr[5] = 0.0;
  arr[6] = 0.0; arr[7] = hf/2;
  arr[8] = wf/2; arr[9] = hf/2;
}


void resizefunc(GLFWwindow* win, int w, int h){
  refreshWindowPosSize(WindowPosSize, w, h);
}
