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
            int width;
            int height;

            SDL_GetWindowSize(window, &width, &height);

            static bool fullScreenOnOff = false;

            if (false == fullScreenOnOff)
            {
                ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
                ImGui::SetNextWindowSize(ImVec2{ (float)width - 20.0f, (float)height - 20.0f });

                if (true == ImGui::Begin("Window"))
                {
                    ImGui::Text("Hello ImGui");

                    if (ImGui::Button("Full Screen On"))
                    {
                        fullScreenOnOff = true;
                    }
                }
            }
            else
            {
                ImGui::SetNextWindowPos(ImVec2{ 0.0f, 0.0f });
                ImGui::SetNextWindowSize(ImVec2{ (float)width, (float)height });

                if (true == ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoDecoration))
                {
                    ImGui::Text("Hello ImGui");

                    if (ImGui::Button("Full Screen Off"))
                    {
                        fullScreenOnOff = false;
                    }
                }
            }

            // 최소화 버튼 제거 밑 유저가 임의로 윈도우 움직이지 못 하게 막기.
            // SetNextWindowXXX() 주석치고 테스트
            // if (true == ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
            // {
            //     ImGui::Text("Hello ImGui");
            // }

            // 윈도우 타이틀 옵션 전부 제거
            // if (true == ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoDecoration))
            // {
            //     ImGui::Text("Hello ImGui");
            // }

            // 다양한 윈도우 플래그 테스트하기!

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