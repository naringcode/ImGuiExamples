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
            static bool s_TagA = false;
            static bool s_TagB = false;
            static bool s_TagC = false;
            static bool s_TagD = false;

            int width;
            int height;

            SDL_GetWindowSize(window, &width, &height);

            ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
            ImGui::SetNextWindowSize(ImVec2{ (float)width - 20.0f, (float)height - 20.0f });

            if (true == ImGui::Begin("Window"))
            {
                // // 식별자를 공유하는 상태(이름이 같음)
                // if (true == ImGui::Button("Button"))
                // {
                //     s_TagA = !s_TagA;
                // }
                // 
                // // s_TagB는 반응하지 않는다.
                // if (true == ImGui::Button("Button"))
                // {
                //     s_TagB = !s_TagB;
                // }

                // 다음과 같이 ID를 임의로 부여하는 방법도 있다.
                ImGui::PushID(0);
                if (true == ImGui::Button("Button"))
                {
                    s_TagA = !s_TagA;
                }
                ImGui::PopID();
                
                ImGui::PushID(1);
                if (true == ImGui::Button("Button"))
                {
                    s_TagB = !s_TagB;
                }
                ImGui::PopID();

                // 다음과 같이 ##을 쓰면 별도의 ID로 식별하지만 뒤의 문자열은 렌더링하지 않는다.
                if (true == ImGui::Button("Button##01"))
                {
                    s_TagC = !s_TagC;
                }

                if (true == ImGui::Button("Button##02"))
                {
                    s_TagD = !s_TagD;
                }

                // Show
                if (true == s_TagA)
                {
                    ImGui::Text("Im TagA");
                }
                
                if (true == s_TagB)
                {
                    ImGui::Text("Im TagB");
                }

                if (true == s_TagC)
                {
                    ImGui::Text("Im TagC");
                }

                if (true == s_TagD)
                {
                    ImGui::Text("Im TagD");
                }
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