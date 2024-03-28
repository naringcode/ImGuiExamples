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
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// TEMP
std::string g_Str;
std::vector<std::string> g_StrVec;

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

    //
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

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
            static bool s_ScrollToBottom = false;

            int width;
            int height;

            SDL_GetWindowSize(window, &width, &height);

            ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
            ImGui::SetNextWindowSize(ImVec2{ (float)width - 20.0f, (float)height - 20.0f });

            if (true == ImGui::Begin("Window"))
            {
                ImGui::Text("Enter Something : ");
                ImGui::InputText("##TextField", &g_Str);

                if (true == ImGui::Button("Push") && g_Str.size() > 0)
                {
                    g_StrVec.push_back(g_Str);

                    s_ScrollToBottom = true; // 텍스트를 입력하면 스크롤 최대로
                }

                ImGui::SameLine();

                if (true == ImGui::Button("Pop") && g_StrVec.size() > 0)
                {
                    g_StrVec.pop_back();
                }

                if (g_Str.size() > 0)
                {
                    ImGui::Text(g_Str.c_str());
                    ImGui::Text("Text : %s", g_Str.c_str());
                }

                ImGui::NewLine();
                ImGui::Text("Stack Size : %d", g_StrVec.size());
                
                ImGui::BeginChild("Child", { 200, 100 }, true);
                {
                    for (auto str : g_StrVec)
                    {
                        ImGui::BulletText(str.c_str());
                    }

                    // 스크롤 최대로(1회)
                    if (true == s_ScrollToBottom)
                    {
                        ImGui::SetScrollHereY(1.0f);

                        s_ScrollToBottom = false;
                    }
                }
                ImGui::EndChild();
            }

            ImGui::End();
        }

        // ImGui::EndFrame(); // NewFrame()과 매칭(Render()에서 자동으로 호출하기 때문에 안 써도 됨

        // Rendering
        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

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