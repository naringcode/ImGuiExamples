// C++
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

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

    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConfig;
    {
        fontConfig.OversampleH = 2;
        fontConfig.OversampleV = 1;
    }

    ImFont* defaultFont  = io.Fonts->AddFontDefault(&fontConfig);
    ImFont* consolasFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Consola.ttf", 16, &fontConfig);

    ImFont* currentFont = defaultFont;

    TextEditor textEditor;
    bool isTextEditorOpened = true;

    textEditor.SetWindowFlags(ImGuiWindowFlags_MenuBar);

    textEditor.SetRenderMenuCallback([&](TextEditor& textEditor) {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (textEditor.GetShowBreakPoints())
                {
                    if (ImGui::MenuItem("Off Breakpoints"))
                    {
                        textEditor.SetShowBreakPoints(false);
                    }
                }
                else
                {
                    if (ImGui::MenuItem("On Breakpoints"))
                    {
                        textEditor.SetShowBreakPoints(true);
                    }
                }

                if (textEditor.GetShowLineNums())
                {
                    if (ImGui::MenuItem("Off Line Nums"))
                    {
                        textEditor.SetShowLineNums(false);
                    }
                }
                else
                {
                    if (ImGui::MenuItem("On Line Nums"))
                    {
                        textEditor.SetShowLineNums(true);
                    }
                }

                if (textEditor.GetShowLineMarkers())
                {
                    if (ImGui::MenuItem("Off Line Markers"))
                    {
                        textEditor.SetShowLineMarkers(false);
                    }
                }
                else
                {
                    if (ImGui::MenuItem("On Line Markers"))
                    {
                        textEditor.SetShowLineMarkers(true);
                    }
                }

                if (textEditor.GetShowWindowResizeGrip())
                {
                    if (ImGui::MenuItem("Off Window Resize Grip"))
                    {
                        textEditor.SetShowWindowResizeGrip(false);
                    }
                }
                else
                {
                    if (ImGui::MenuItem("On Window Resize Grip"))
                    {
                        textEditor.SetShowWindowResizeGrip(true);
                    }
                }
    
                ImGui::EndMenu();
            }
    
            if (ImGui::BeginMenu("Fonts"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    currentFont = defaultFont;

                    textEditor.ChangeFont(currentFont);
                }

                if (ImGui::MenuItem("Consolas"))
                {
                    currentFont = consolasFont;

                    textEditor.ChangeFont(currentFont);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    });
    
    textEditor.SetRenderHeaderCallback([](TextEditor& textEditor) {
        static bool s_ShowUIButtons = true;

        if (true == s_ShowUIButtons && ImGui::Button("Header | Hide UI Buttons"))
        {
            s_ShowUIButtons = false;
        }
        else if (false == s_ShowUIButtons && ImGui::Button("Header | Show UI Buttons"))
        {
            s_ShowUIButtons = true;
        }

        if (true == s_ShowUIButtons)
        {
            int32_t mainContentPaddingX = (int32_t)textEditor.GetMainContentPaddingX();
            ImGui::SliderInt("##MainContentPaddingX", &mainContentPaddingX, 0, 24, "MainContentPaddingX : %d");
            textEditor.SetMainContentPaddingX((float)mainContentPaddingX);

            ImGui::SameLine();

            int32_t mainContentPaddingY = (int32_t)textEditor.GetMainContentPaddingY();
            ImGui::SliderInt("##MainContentPaddingY", &mainContentPaddingY, 0, 24, "MainContentPaddingY : %d");
            textEditor.SetMainContentPaddingY((float)mainContentPaddingY);

            int32_t lineNumLeftSpacing = (int32_t)textEditor.GetLineNumLeftSpacing();
            ImGui::SliderInt("##LineNumLeftSpacing", &lineNumLeftSpacing, 0, 24, "LineNumLeftSpacing : %d");
            textEditor.SetLineNumLeftSpacing((float)lineNumLeftSpacing);

            ImGui::SameLine();

            int32_t lineNumRightSpacing = (int32_t)textEditor.GetLineNumRightSpacing();
            ImGui::SliderInt("##LineNumRightSpacing", &lineNumRightSpacing, 0, 24, "LineNumRightSpacing : %d");
            textEditor.SetLineNumRightSpacing((float)lineNumRightSpacing);

            int32_t textLineTopSpacing = (int32_t)textEditor.GetTextLineTopSpacing();
            ImGui::SliderInt("##TextLineTopSpacing", &textLineTopSpacing, 0, 24, "TextLineTopSpacing : %d");
            textEditor.SetTextLineTopSpacing((float)textLineTopSpacing);

            ImGui::SameLine();

            int32_t textLineBottomSpacing = (int32_t)textEditor.GetTextLineBottomSpacing();
            ImGui::SliderInt("##TextLineBottomSpacing", &textLineBottomSpacing, 0, 24, "TextLineBottomSpacing : %d");
            textEditor.SetTextLineBottomSpacing((float)textLineBottomSpacing);

            int32_t textLineLeftSpacing = (int32_t)textEditor.GetTextLineLeftSpacing();
            ImGui::SliderInt("##TextLineLeftSpacing", &textLineLeftSpacing, 0, 24, "TextLineLeftSpacing : %d");
            textEditor.SetTextLineLeftSpacing((float)textLineLeftSpacing);

            ImGui::SameLine();

            int32_t textLineRightSpacing = (int32_t)textEditor.GetTextLineRightSpacing();
            ImGui::SliderInt("##TextLineRightSpacing", &textLineRightSpacing, 0, 200, "TextLineRightSpacing : %d");
            textEditor.SetTextLineRightSpacing((float)textLineRightSpacing);
        }
    });

    textEditor.SetRenderFooterCallback([](TextEditor& textEditor) {
        ImGui::Text("Footer | Lang : C++ | Lines : %d", textEditor.GetTotalLines());
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
        //textEditor.RenderChildWindow(ImVec2{ 0.0f, 200.0f });
        
        ImGui::Text("AAABBBCCC");
        
        textEditor.SetWindowTitle("Child Window##1");
        textEditor.RenderChildWindow(ImVec2{ 300.0f, 200.0f });
        ImGui::SameLine();
        textEditor.SetWindowTitle("Child Window##2");
        textEditor.RenderChildWindow(ImVec2{ 0.0f, 200.0f });
        
        ImGui::Text("AAABBBCCC");
        ImGui::Text("AAABBBCCC");
        ImGui::End();

        EndFrame();
    }

    Shutdown();

    return 0;
}