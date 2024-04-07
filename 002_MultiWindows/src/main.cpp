// C++
#include <iostream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

std::vector<std::string> g_WindowVec;

float g_RectColor[3];
float g_RectSize[2];
float g_RectPos[2];

int main(int argc, char* argv[])
{
    // Setup SDL
    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cout << "[Error] SDL_Init() : " << SDL_GetError();

        return -1;
    }

    SDL_Window* window;
    SDL_Renderer* renderer;

    window = SDL_CreateWindow("MultiWindows",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              1280, 720,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    if (nullptr == window)
    {
        SDL_Quit();

        std::cout << "[Error] SDL_CreateWindow() : " << SDL_GetError();

        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (nullptr == renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();

        std::cout << "[Error] SDL_CreateWindow() : " << SDL_GetError();

        return -1;
    }

    g_RectColor[0] = 1.0f;
    g_RectColor[1] = 1.0;
    g_RectColor[2] = 1.0f;

    g_RectSize[0] = 1280.0f / 10.0f;
    g_RectSize[1] = 720.0f / 10.0f;

    g_RectPos[0] = 1280.0f / 2.0f;
    g_RectPos[1] = 720.0f / 2.0f;

    //
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); // 전역 Context 생성

    ImGuiIO& io = ImGui::GetIO();
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    }

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Main Logic
    bool isRunning = true;
    while (true == isRunning)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (SDL_QUIT == event.type)
            {
                isRunning = false;

                break;
            }
            else if (SDL_KEYDOWN == event.type)
            {
                if (SDLK_ESCAPE == event.key.keysym.sym)
                {
                    isRunning = false;

                    break;
                }
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Main Window
        {
            ImGui::Begin("Main");

            ImGui::Text("Number Of Windows : %d", 1 + g_WindowVec.size());

            if (true == ImGui::Button("Push"))
            {
                std::string str = "Hello ";
                str += std::to_string(g_WindowVec.size());

                g_WindowVec.push_back(str);
            }

            ImGui::SameLine();

            if (true == ImGui::Button("Pop") && g_WindowVec.size() > 0)
            {
                g_WindowVec.pop_back();
            }

            ImGui::ColorEdit3("Color", g_RectColor);
            ImGui::DragFloat2("Size", g_RectSize);
            ImGui::DragFloat2("Pos", g_RectPos);

            ImGui::End();
        }

        for (const std::string& str : g_WindowVec)
        {
            ImGui::Begin(str.c_str());

            ImGui::Text("Lorem ipsum");

            ImGui::End();
        }

        // ImGui::EndFrame(); // NewFrame()과 매칭(Render()에서 자동으로 호출하기 때문에 안 써도 됨

        // Rendering
        ImGui::Render();
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 
                g_RectColor[0] * 255.0f, g_RectColor[1] * 255.0f, g_RectColor[2] * 255.0f, 255);
        
        SDL_Rect sdlRect;
        {
            sdlRect.w = g_RectSize[0];
            sdlRect.h = g_RectSize[1];
            sdlRect.x = g_RectPos[0] - g_RectSize[0] / 2.0f;
            sdlRect.y = g_RectPos[1] - g_RectSize[1] / 2.0f;
        };

        SDL_RenderFillRect(renderer, &sdlRect);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}