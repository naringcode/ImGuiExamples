// C++
#include <iostream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
// 2480 라인 부근 : https://github.com/ocornut/imgui/blob/master/imgui.h
#define IMGUI_DEFINE_MATH_OPERATORS // ImVec 계열에 연산자를 추가하기 위한 매크로 / #include <imgui.h> 보다 선행되어야 함

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

namespace ImGui
{
    bool Link(const std::string& text, const std::string& url)
    {
        bool res = false;

        ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);

        ImGui::PushStyleColor(ImGuiCol_Text, buttonColor);
        {
            ImGui::Text(text.c_str());
        }
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(url.c_str());

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                res = true;
            }
        }
        else
        {
            // ImGui::GetItemRectMin() : Screen Space Top Left(from the bounding rectangle of the last item)
            // ImGui::GetItemRectMax() : Screen Space Bottom Right(from the bounding rectangle of the last item)
            // ImGui::GetItemRectSize() : Size of Last Item
            ImVec2 lineEnd   = ImGui::GetItemRectMax();
            ImVec2 lineStart = ImVec2{ ImGui::GetItemRectMin().x, lineEnd.y };

            ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, ImGui::ColorConvertFloat4ToU32(buttonColor));
        }

        return res;
    }
}

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

        ImGui::SetNextWindowPos(ImVec2{ 50.0f, 50.0f });
        ImGui::SetNextWindowSize(ImVec2{ (float)width - 100.0f, (float)height - 100.0f });

        if (true == ImGui::Begin("Window", nullptr,
                                 ImGuiWindowFlags_NoDecoration - ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Link("Google", "https://www.google.com/");
            ImGui::Link("Youtube", "https://www.youtube.com/");
        }

        ImGui::End();

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