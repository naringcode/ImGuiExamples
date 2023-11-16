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

            // ImGui::SetNextWindowPos(ImVec2{ (float)width * 0.2f, (float)height * 0.2f });
            ImGui::SetNextWindowSize(ImVec2{ (float)width * 0.6f, (float)height * 0.6f });

            // Main Menu
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    ImGui::MenuItem("File Menu Hello", nullptr, nullptr, false);

                    if (ImGui::BeginMenu("New"))
                    {
                        // Shortcut 시스템은 직접 만들어야 함.
                        // https://github.com/ocornut/imgui/issues/1684
                        ImGui::MenuItem("Project", "Ctrl+Shift+N", nullptr);
                        ImGui::MenuItem("File", "Ctrl+N", nullptr);

                        ImGui::EndMenu();
                    }

                    ImGui::MenuItem("Open");

                    ImGui::Separator();

                    ImGui::MenuItem("Close");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit"))
                {
                    ImGui::MenuItem("Undo", "Ctrl+Z", nullptr);
                    ImGui::MenuItem("Redo", "Ctrl+Y", nullptr);

                    ImGui::SeparatorText("SeparatorText()");

                    ImGui::MenuItem("Cut", "Ctrl+X", nullptr);
                    ImGui::MenuItem("Copy", "Ctrl+C", nullptr);
                    ImGui::MenuItem("Paste", "Ctrl+V", nullptr);

                    ImGui::Separator();

                    ImGui::MenuItem("Select All", "Ctrl+A", nullptr);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    ImGui::ShowStyleSelector("Themes");
                    ImGui::ShowFontSelector("Fonts");

                    // From imgui_demo.cpp
                    if (ImGui::BeginMenu("Colors"))
                    {
                        float sz = ImGui::GetTextLineHeight();

                        for (int i = 0; i < ImGuiCol_COUNT; i++)
                        {
                            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
                            ImVec2 p = ImGui::GetCursorScreenPos();
                            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                            ImGui::Dummy(ImVec2(sz, sz));
                            ImGui::SameLine();
                            ImGui::MenuItem(name);
                        }

                        ImGui::EndMenu();
                    }

                    static bool s_Checked = true;

                    // 메뉴 토글링
                    if (true == s_Checked)
                    {
                        ImGui::MenuItem("Checked", nullptr, &s_Checked);
                    }
                    else
                    {
                        ImGui::MenuItem("Unchecked", nullptr, &s_Checked);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            static bool s_SubWinEnabled = false;

            // ImGuiWindowFlags_MenuBar 옵션 사용
            uint64_t windowOptions = 0;
            {
                windowOptions |= ImGuiWindowFlags_MenuBar;
                windowOptions |= (ImGuiWindowFlags_NoDecoration - ImGuiWindowFlags_NoTitleBar);
                windowOptions |= ImGuiWindowFlags_NoBringToFrontOnFocus; // https://github.com/ocornut/imgui/issues/3693
            }

            if (ImGui::Begin("Window", nullptr, windowOptions))
            {
                static std::string s_Str = "None";

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("Sub Menu 1"))
                    {
                        if (ImGui::MenuItem("Sub First##01"))  s_Str = "Sub Menu 1 / Sub First";
                        if (ImGui::MenuItem("Sub Second##01")) s_Str = "Sub Menu 1 / Sub Second";
                        if (ImGui::MenuItem("Sub Third##01"))  s_Str = "Sub Menu 1 / Sub Third";

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Sub Menu 2"))
                    {
                        if (ImGui::MenuItem("Sub First##02"))  s_Str = "Sub Menu 2 / Sub First";
                        if (ImGui::MenuItem("Sub Second##02")) s_Str = "Sub Menu 2 / Sub Second";
                        if (ImGui::MenuItem("Sub Third##02"))  s_Str = "Sub Menu 2 / Sub Third";

                        ImGui::EndMenu();
                    }

                    if (false == s_SubWinEnabled)
                    {
                        // 이렇게도 받을 수 있음
                        s_SubWinEnabled = ImGui::Button("New Window");
                    }

                    ImGui::EndMenuBar();
                }

                ImGui::Text(s_Str.c_str());
            }

            ImGui::End();

            // 서브 윈도우 띄우기
            if (true == s_SubWinEnabled)
            {
                // ImGui::SetNextWindowFocus();
                ImGui::SetNextWindowSize(ImVec2{ 200.0f, 150.0f });
            
                if (ImGui::Begin("Sub Window", &s_SubWinEnabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
                {
                    ImGui::Text("Hello World!");
                }
            
                ImGui::End();
            }
        }

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