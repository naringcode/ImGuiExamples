// C++
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

// User
#include "TextEditor.h"

// Variables
SDL_Window*   g_Window;
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
    // ImGui::EndFrame(); // NewFrame()�� ��Ī(Render()���� �ڵ����� ȣ���ϱ� ������ �� �ᵵ ��

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

    TextEditor textEditor;
    bool isTextEditorOpened = true;

    textEditor.SetWindowFlags(ImGuiWindowFlags_MenuBar);

    textEditor.SetRenderMenuCallback([](TextEditor& textEditor) {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::MenuItem("First");
                ImGui::MenuItem("Second");
                ImGui::MenuItem("Third");
    
                ImGui::EndMenu();
            }
    
            ImGui::EndMenuBar();
        }
    });
    
    textEditor.SetRenderHeaderCallback([](TextEditor& textEditor) {
        ImGui::Text("Header | Lines : %d", textEditor.GetTotalLines());
    });

    textEditor.SetRenderFooterCallback([](TextEditor& textEditor) {
        ImGui::Text("Footer | Lang : C++");
    });

    // Set Text
    {
        std::ifstream in("src/TextEditor.cpp");

        if (in && in.is_open())
        {
            std::string str{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

            textEditor.SetText(str);
        }
    }
    
    // Main Logic
    while (true == g_IsRunning)
    {
        PollEvents();

        BeginFrame();

        if (ImGui::BeginMainMenuBar())
        {
            if (false == isTextEditorOpened && ImGui::Button("Open Editor"))
            {
                isTextEditorOpened = true;
            }
            else if (isTextEditorOpened && ImGui::Button("Close Editor"))
            {
                isTextEditorOpened = false;
            }

            ImGui::EndMainMenuBar();
        }

        textEditor.SetNextWindowSize(ImVec2{ 800.0f, 600.0f }, ImGuiCond_FirstUseEver);
        textEditor.RenderWindow(&isTextEditorOpened);

        ImGui::SetNextWindowSize(ImVec2{ 800.0f, 600.0f }, ImGuiCond_FirstUseEver);
        ImGui::Begin("Test Window", nullptr);
        ImGui::Text("AAABBBCCC");
        ImGui::Text("AAABBBCCC");
        textEditor.RenderChildWindow(100.0f);
        ImGui::Text("AAABBBCCC");
        ImGui::Text("AAABBBCCC");
        ImGui::End();

        EndFrame();
    }

    Shutdown();

    return 0;
}