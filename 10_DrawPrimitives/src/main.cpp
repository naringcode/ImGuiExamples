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
                                 ImGuiWindowFlags_NoDecoration - ImGuiWindowFlags_NoTitleBar - ImGuiWindowFlags_NoResize
                                 | ImGuiWindowFlags_NoBringToFrontOnFocus))
        {
            // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
            // 7920 라인 부근 : IMGUI_DEMO_MARKER("Examples/Custom Rendering");

            // Primitives를 그리는 것? 직접적인 Function 없음.
            // DrawList를 써야 함.
            // 모든 Window는 고유한 DrawList를 가지고 있다.
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            {
                ImGui::Text("Gradients");

                ImVec2 gradientSize{ ImGui::CalcItemWidth(), ImGui::GetFrameHeight() };

                // First Gradient
                {
                    static ImVec4 startColor{ 0.0f, 0.0f, 0.0f, 1.0f };
                    static ImVec4 endColor{ 1.0f, 1.0f, 1.0f, 1.0f };

                    // Picker 기준으로는 0 ~ 255로 표시된다.
                    ImGui::ColorEdit4("FirstColor##01", (float*)&startColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    ImGui::SameLine();
                    ImGui::ColorEdit4("FirstColor##02", (float*)&endColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                    ImGui::SameLine();

                    // GetCursorScreenPos() : cursor position in absolute coordinates (useful to work with ImDrawList API)
                    ImVec2 leftTop = ImGui::GetCursorScreenPos();
                    ImVec2 rightBottom = ImVec2{ leftTop.x + gradientSize.x, leftTop.y + gradientSize.y };

                    ImU32 startUColor = ImGui::GetColorU32(startColor);
                    ImU32 endUColor = ImGui::GetColorU32(endColor);

                    drawList->AddRectFilledMultiColor(leftTop, rightBottom, startUColor, endUColor, endUColor, startUColor);

                    // 다음 위치로 커서를 위치시키기 위해서 사용(IsItemActive, IsItemHovered를 적용시키기에도 유용하다).
                    ImGui::InvisibleButton("##Gradient01", gradientSize);
                    ImGui::SetItemTooltip("First Gradient");
                }

                // Second Gradient
                {
                    static ImVec4 startColor{ 0.0f, 1.0f, 0.0f, 1.0f };
                    static ImVec4 endColor{ 1.0f, 0.0f, 0.0f, 1.0f };

                    ImGui::ColorEdit4("SecondColor##01", (float*)&startColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                    ImGui::SameLine();
                    ImGui::ColorEdit4("SecondColor##02", (float*)&endColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                    ImGui::SameLine();

                    ImVec2 leftTop = ImGui::GetCursorScreenPos();
                    ImVec2 rightBottom = ImVec2{ leftTop.x + gradientSize.x, leftTop.y + gradientSize.y };

                    ImU32 startUColor = ImGui::GetColorU32(startColor);
                    ImU32 endUColor = ImGui::GetColorU32(endColor);

                    drawList->AddRectFilledMultiColor(leftTop, rightBottom, startUColor, endUColor, endUColor, startUColor);

                    // 다음 위치로 커서를 위치시키기 위해서 사용(IsItemActive, IsItemHovered를 적용시키기에도 유용하다).
                    ImGui::InvisibleButton("##Gradient02", gradientSize);

                    if (ImGui::IsItemHovered())
                    {
                        if (ImGui::BeginTooltip())
                        {
                            ImVec4 perColor{ endColor - startColor };
                            {
                                float barWidth = rightBottom.x - leftTop.x;

                                perColor.x /= barWidth;
                                perColor.y /= barWidth;
                                perColor.z /= barWidth;
                                perColor.w /= barWidth;
                            }

                            ImVec4 buttonColor = startColor;
                            {
                                float diff = ImGui::GetIO().MousePos.x - leftTop.x;

                                buttonColor.x += perColor.x * diff;
                                buttonColor.y += perColor.y * diff;
                                buttonColor.z += perColor.z * diff;
                                buttonColor.w += perColor.w * diff;
                            }

                            ImGui::Text("R(%d) G(%d) B(%d) A(%d)", (int)(buttonColor.x * 255.f), (int)(buttonColor.y * 255.f), (int)(buttonColor.z * 255.f), (int)(buttonColor.w * 255.f));
                            ImGui::SameLine();
                            ImGui::ColorButton("TooltipColorButton", buttonColor);

                            ImGui::EndTooltip();
                        }
                    }
                }

                ImGui::Separator();

                ImGui::Text("Background / Foreground DrawList");
                {
                    static bool s_OpenWindow = false;

                    static ImVec2 s_BGCenter{ 0.0f, 0.0f };
                    static float  s_BGRadius = 200.0f;
                    static ImVec4 s_BGColor{ 255.0f, 0.0f, 0.0f, 255.0f };
                    static int    s_BGSegments  = 0;
                    static float  s_BGThickness = 5.0f;

                    static ImVec2 s_FGCenter{ 0.0f, 0.0f };
                    static float  s_FGRadius = 100.0f;
                    static ImVec4 s_FGColor{ 0.0f, 255.0f, 0.0f, 255.0f };
                    static int    s_FGSegments = 0;
                    static float  s_FGThickness = 5.0f;

                    std::string buttonStr = s_OpenWindow ? "Close##BGFG" : "Open##BGFG";

                    ImGui::SameLine();

                    if (ImGui::Button(buttonStr.c_str()))
                    {
                        s_OpenWindow = !s_OpenWindow;
                    }

                    // s_OpenWindow가 열려 있는지 확인한 다음에 Begin()을 진행해야 한다.
                    if (true == s_OpenWindow)
                    {
                        if (ImGui::Begin("BG/FG DrawList", &s_OpenWindow))
                        {
                            ImGui::Text("Hello");
                        }

                        ImVec2 windowPos  = ImGui::GetWindowPos();
                        ImVec2 windowSize = ImGui::GetWindowSize();

                        ImVec2 windowCenter = windowPos + (windowSize / 2);

                        // Background
                        {
                            ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();
                            
                            bgDrawList->AddCircle(s_BGCenter + windowCenter, s_BGRadius, ImGui::ColorConvertFloat4ToU32(s_BGColor), s_BGSegments, s_BGThickness);
                        }

                        // Foreground
                        {
                            ImDrawList* fgDrawList = ImGui::GetForegroundDrawList();

                            fgDrawList->AddCircle(s_FGCenter + windowCenter, s_FGRadius, ImGui::ColorConvertFloat4ToU32(s_FGColor), s_FGSegments, s_FGThickness);
                        }

                        ImGui::End();
                    }

                }

                ImGui::Text("Canvas");
                {
                    // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L8015
                    // 8015 라인 부근
                    static bool s_OpenWindow = false;

                    std::string buttonStr = s_OpenWindow ? "Close##Canvas" : "Open##Canvas";

                    ImGui::SameLine();

                    if (ImGui::Button(buttonStr.c_str()))
                    {
                        s_OpenWindow = !s_OpenWindow;
                    }

                    // s_OpenWindow가 열려 있는지 확인한 다음에 Begin()을 진행해야 한다.
                    if (true == s_OpenWindow)
                    {
                        if (ImGui::Begin("Canvas", &s_OpenWindow))
                        {
                            ImGui::Text("Hello");
                        }

                        ImGui::End();
                    }
                }
            }
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