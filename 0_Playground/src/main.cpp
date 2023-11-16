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
        int width;
        int height;

        SDL_GetWindowSize(window, &width, &height);

        ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
        ImGui::SetNextWindowSize(ImVec2{ (float)width - 20.0f, (float)height - 20.0f });

        if (true == ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoDecoration - ImGuiWindowFlags_NoTitleBar))
        {
            static bool  s_CheckDisabled = true;
            static float s_SpacingX = ImGui::GetStyle().ItemSpacing.x;
            static float s_SpacingY = ImGui::GetStyle().ItemSpacing.y;

            static std::string s_InputStr = "Lorem ipsum dolor sit amet";

            if (ImGui::Button("Toggle UIs"))
            {
                s_CheckDisabled = !s_CheckDisabled;
            }

            ImGui::PushItemWidth(120.0f);
            {
                ImGui::SliderFloat("ItemSpacingX", &s_SpacingX, 0.0f, 200.0f);

                ImGui::SameLine();

                ImGui::SliderFloat("ItemSpacingY", &s_SpacingY, 0.0f, 50.0f);
            }
            ImGui::PopItemWidth();

            ImGui::Separator();

            // Disabled
            ImGui::BeginDisabled(s_CheckDisabled);
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ s_SpacingX, s_SpacingY });
                
                ImGui::Text("Hello World!");

                ImGui::SameLine();

                ImGui::Text(s_InputStr.c_str());

                ImGui::SameLine();

                ImGui::Text("Hello World!");

                ImGui::Button("Button##01");
                ImGui::InputText("InputText##01", &s_InputStr);

                for (int i = 1; i <= 12; i++)
                {
                    std::string str = "This Text ";
                    str += (char)(i + 'A' - 1);

                    ImGui::Text(str.c_str());
                    
                    if (0 != i % 3)
                    {
                        ImGui::SameLine();
                    }
                }

                ImGui::PopStyleVar();
            }
            ImGui::EndDisabled();

            // 빛의 3속성 : 색상(Hue), 채도(Saturation), 명도(Value)

            // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
            // 600 라인 부근 : IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Colored)");


        }

        ImGui::End();

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