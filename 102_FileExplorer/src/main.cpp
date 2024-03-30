// C++
#include <iostream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// User
#include "Render.h"

// Variables
SDL_Window* g_Window;
SDL_Renderer* g_Renderer;

bool g_IsRunning = true;

bool Init()
{
    // Setup SDL
    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cout << "[Error] SDL_Init() : " << SDL_GetError();

        return false;
    }

    g_Window = SDL_CreateWindow("MultiWindows",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                1280, 720,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    if (nullptr == g_Window)
    {
        SDL_Quit();

        std::cout << "[Error] SDL_CreateWindow() : " << SDL_GetError();

        return false;
    }

    g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (nullptr == g_Renderer)
    {
        SDL_DestroyWindow(g_Window);
        SDL_Quit();

        std::cout << "[Error] SDL_CreateWindow() : " << SDL_GetError();

        return false;
    }

    //
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    }

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 16, nullptr, io.Fonts->GetGlyphRangesKorean());

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(g_Window, g_Renderer);
    ImGui_ImplSDLRenderer2_Init(g_Renderer);

    return true;
}

void Shutdown()
{
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}

void PollEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (SDL_QUIT == event.type)
        {
            g_IsRunning = false;

            break;
        }
        else if (SDL_KEYDOWN == event.type)
        {
            if (SDLK_ESCAPE == event.key.keysym.sym)
            {
                g_IsRunning = false;

                break;
            }
        }
    }
}

void BeginFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
}

void EndFrame()
{
    // ImGui::EndFrame(); // NewFrame()과 매칭(Render()에서 자동으로 호출하기 때문에 안 써도 됨

    ImGui::Render();

    SDL_SetRenderDrawColor(g_Renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_Renderer);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(g_Renderer);
}

int main(int argc, char* argv[])
{
    if (false == Init())
        return -1;

    WidgetWindow widget;

    // Main Logic
    while (true == g_IsRunning)
    {
        PollEvents();

        BeginFrame();

        // Main Window
        int sysWinWidth;
        int sysWinHeight;

        SDL_GetWindowSize(g_Window, &sysWinWidth, &sysWinHeight);

        Render(widget, sysWinWidth, sysWinHeight);

        EndFrame();
    }

    Shutdown();

    return 0;
}