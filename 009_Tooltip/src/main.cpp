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
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

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

        /**
         * ���⼱ Tooltip�� Window ���ο� �ۼ������� �ܵ����� �ۼ��ص� �߱� �Ѵ�.
         * ���� �ʿ��ϴٸ� ���� Window�� �ۼ����� �ʰ� Tooltip�� ǥ���ص� �ȴ�.
         */
        if (true == ImGui::Begin("Window", nullptr, ImGuiWindowFlags_NoDecoration - ImGuiWindowFlags_NoTitleBar))
        {
            // -----
            // 780 ���� �α� : IMGUI_DEMO_MARKER("Widgets/Tooltips");
            // 
            // Typical use cases:
            // - Short-form (text only):      SetItemTooltip("Hello");
            // - Short-form (any contents):   if (BeginItemTooltip()) { Text("Hello"); EndTooltip(); }
            //
            // - Full-form (text only):       if (IsItemHovered(...)) { SetTooltip("Hello"); }
            // - Full-form (any contents):    if (IsItemHovered(...) && BeginTooltip()) { Text("Hello"); EndTooltip(); }
            //
            // HelpMarker(
            //     "Tooltip are typically created by using a IsItemHovered() + SetTooltip() sequence.\n\n"
            //     "We provide a helper SetItemTooltip() function to perform the two with standards flags.");
            // -----

            ImGui::Text("I am Text");

            ImGui::SameLine(0.0f, 10.0f); // 10�ȼ� �����̽�

            // TextDisabled()�� ���� ȸ�� �ؽ�Ʈ�� �׸� �� �ִ�.
            // ���� PushStyleColor()�� �����ϴ� ����� �ᵵ ��.
            // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
            {
                ImGui::TextDisabled("(info)");

                // SetTooltip ���
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("(info) Tooltip");
                }
            }
            // ImGui::PopStyleColor();

            ImGui::NewLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.2f, 0.2f, 1.0f });
            {
                ImGui::Button("I am Button");
                
                // BeginTooltip() / EndTooltip() ���
                if (ImGui::IsItemHovered())
                {
                    if (ImGui::BeginTooltip())
                    {
                        ImGui::Text("Button Text Tooltip");
                        ImGui::Button("Another Button");
                        ImGui::Text("Sin(time) : %f", sinf((float)ImGui::GetTime()));

                        ImGui::EndTooltip();
                    }
                }
            }
            ImGui::PopStyleColor();

            ImGui::NewLine();

            // SetItemTooltip() ���
            {
                ImGui::Text("Text Text Text");
                ImGui::SetItemTooltip("Text Tooltip");

                ImGui::Button("Button Button");
                ImGui::SetItemTooltip("Button Tooltip");
            }
        }

        ImGui::End();

        // ImGui::EndFrame(); // NewFrame()�� ��Ī(Render()���� �ڵ����� ȣ���ϱ� ������ �� �ᵵ ��

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