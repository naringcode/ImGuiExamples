// #pragma execution_character_set("utf-8")
// #define IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS // https://qiita.com/babiron_i/items/759d80965b497384bc0e

// C++
#include <iostream>
#include <vector>
#include <string>

// SDL
#include <SDL2/SDL.h>

// imgui
// 2480 ���� �α� : https://github.com/ocornut/imgui/blob/master/imgui.h
#define IMGUI_DEFINE_MATH_OPERATORS // ImVec �迭�� �����ڸ� �߰��ϱ� ���� ��ũ�� / #include <imgui.h> ���� ����Ǿ�� ��

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

#include <imgui_internal.h> // ImGuiContext

// ��Ʈ�� ���� �ڼ��� ������ �Ʒ� ��ũ �����ϱ�
// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md

// IME ����ü�� ���� ������ �Ʒ� ��ũ �����ϱ�
// ImGui_ImplSDL2_SetPlatformImeData() �� �����ϸ� ��(���� �־ �׷��� �� �۵����� ����(�ѱ� IME�� �� ��))
// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl2.cpp


/**
 * UTF-8 �ؽ�Ʈ �Է� ����
 * 
 * 1. �ؽ�Ʈ ����
 * 2. Ÿ���� ����(SDL_TEXTEDITING)
 *    2-1. Ÿ���ε� ������ ������ UTF-8 ī��Ʈ��ŭ ���ڿ��� Pop
 *    2-2. Ÿ���� ������ ����
 * 3. ���ڰ� �ϼ��Ǹ� Ÿ���� ���� ���� �ʱ�ȭ(SDL_TEXTINPUT)
 * 
 * # ���ڰ� Ÿ���εǰ� �ִ� ���߿� Backspace�� ������ SDLK_BACKSPACE �̺�Ʈ�� ȣ����� ����.
 * # �ش� �̺�Ʈ�� Ÿ���εǰ� �ִ� ���ڰ� ���� �� ȣ��ȴ�.
 */

// UTF-8 �� ���ڰ� �� ����Ʈ�� �̷���� �ִ��� Ȯ��
int ByteCountOfLastCharFromUTF8String(const std::string& utf8Str)
{
    /**
     * ## UTF-8 ���� ���� ���� ##
     * 
     * ASCII ���� (U+0000 ~ U+007F): 0xxxxxxx
     * 2����Ʈ ���� (U+0080 ~ U+07FF): 110xxxxx 10xxxxxx
     * 3����Ʈ ���� (U+0800 ~ U+FFFF): 1110xxxx 10xxxxxx 10xxxxxx
     * 4����Ʈ ���� (U+10000 ~ U+10FFFF): 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */
    if (0 == utf8Str.length())
        return 0;

    // �ϳ��� Ȯ��(��� �ƽ�Ű�ڵ� �ϳ��� �ִٰ� ����)
    int res = 1;

    for (int idx = utf8Str.size() - 1; idx >= 0; idx--)
    {
        // 0xC0 : 1100 0000
        // 0x80 : 1000 1000
        // ���� 2��Ʈ�� �̾��� �� 0x80�� �ƴ϶�� �ٸ� ���ڵ� ����(�� ���� ���̺� ���ؼ� ������ �ϱ�)
        if ((0xC0 & utf8Str[idx]) != 0x80)
            break;

        res++;
    }

    return res;
}

int main(int argc, char* argv[])
{
    // Setup SDL
    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cout << "[Error] SDL_Init() : " << SDL_GetError();

        return -1;
    }

    // https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl2.cpp#L12
    // �ѱ� IME�� �� �Ǵ� �� ����(DirectX�� OpenGL������ �Ǵµ� SDL������ �� ��)
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

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
    ImGuiContext* imGuiContext = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    }

    ImGui::GetMainViewport()->PlatformHandleRaw = (void*)SDL_GetWindowID(window);

    // TEMP
    // ImVec2 minPos;
    // ImVec2 maxPos;

    // IME�� �������� ������ ���� ����� ����.
    std::string* currIMEStr    = nullptr;
    bool         isTextEditing = false;
    int          prevEditingCnt = 0;

    /**
     * ��Ʈ HERE
     */
    // AddFontFromFileTTF()�� �߰��� ù ��° ��Ʈ�� �⺻ ��Ʈ�� ������.
    ImFont* userFontA = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 24, nullptr, io.Fonts->GetGlyphRangesKorean());
    ImFont* userFontB = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Consola.ttf", 16, nullptr, io.Fonts->GetGlyphRangesKorean());
    ImFont* userFontC = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/NanumGothic.ttf", 14, nullptr, io.Fonts->GetGlyphRangesKorean());
    
    ImFont* defaultFont = io.Fonts->AddFontDefault();

    // combine fonts (https://github.com/ocornut/imgui/blob/master/docs/FONTS.md | Combine multiple fonts into one)
    ImFontConfig fontConfig;
    strcpy_s(fontConfig.Name, "Combined(Consolas + NanumGothic), 12pt");

    ImFont* combinedFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Consola.ttf", 12, &fontConfig);
    fontConfig.MergeMode = true; // MergeMode�� true�� �ϴ� ���� AddFontXXX() �迭�� ȣ���� ������ fontConfig�� ���յ�.
    
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/NanumGothic.ttf", 12, &fontConfig, io.Fonts->GetGlyphRangesKorean()); // merge into the combinedFont
    io.Fonts->Build();

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
                else if (SDLK_BACKSPACE == event.key.keysym.sym)
                {
                    // IME�� ����ǰ� �ִ� ���̸� �� ���´�(Ÿ���εǰ� �ִ� ���� ����� ������ �� Ȯ����).
                    // std::cout << "Backspace!\n";

                    if (nullptr != currIMEStr && currIMEStr->length() > 0)
                    {
                        int cnt = ByteCountOfLastCharFromUTF8String(*currIMEStr);
                        
                        while (cnt--)
                        {
                            currIMEStr->pop_back();
                        }
                    }

                }
            }
            else if (SDL_TEXTINPUT == event.type)
            {
                // �ؽ�Ʈ �� ĭ �Է��� �Ϸ�Ǹ� ������ �̺�Ʈ
                // ex) �� -> �� -> �� -> SDL_TEXTINPUT
                std::cout << "[SDL_TEXTINPUT] ";
                
                for (int i = 0; i < strlen(event.edit.text); i++)
                {
                    printf("0x%02X ", (unsigned char)event.edit.text[i]);
                }
                
                std::cout << '\n';

                if (nullptr != currIMEStr)
                {
                    isTextEditing  = false;
                    prevEditingCnt = 0;

                    for (int i = 0; i < strlen(event.edit.text); i++)
                    {
                        currIMEStr->push_back(event.edit.text[i]);
                    }
                }
            }
            else if (SDL_TEXTEDITING == event.type)
            {
                // �ؽ�Ʈ�� �Է��ϴ� ���߿� ������ �̺�Ʈ
                // ex) �� : ��(SDL_TEXTEDITING) -> ��(SDL_TEXTEDITING) -> ��(SDL_TEXTEDITING)
                std::cout << "[SDL_TEXTEDITING] " << event.edit.start << ' ' << event.edit.length << ' ' << strlen(event.edit.text) << " : ";
                
                for (int i = 0; i < strlen(event.edit.text); i++)
                {
                    printf("0x%02X ", (unsigned char)event.edit.text[i]);
                }
                
                std::cout << '\n';

                // std::cout << imGuiContext->PlatformImeData.InputPos.x << ", " << imGuiContext->PlatformImeData.InputPos.y << '\n';

                // minPos.x = imGuiContext->PlatformImeData.InputPos.x;
                // minPos.y = imGuiContext->PlatformImeData.InputPos.y;
                // 
                // maxPos.x = minPos.x + 12.0f;
                // maxPos.y = minPos.y + imGuiContext->PlatformImeData.InputLineHeight;

                if (nullptr != currIMEStr)
                {
                    if (true == isTextEditing)
                    {
                        std::cout << prevEditingCnt << '\n';
                        // �߰� ���ڿ��� ���߿� ���� ����غ��� ��(�ϴ� �� �ڿ� �ִ� ���ڸ� ó���ϴ� ���)
                        for (int i = 0; i < prevEditingCnt; i++)
                        {
                            currIMEStr->pop_back();
                        }
                    }

                    isTextEditing  = true;
                    prevEditingCnt = strlen(event.edit.text);

                    for (int i = 0; i < strlen(event.edit.text); i++)
                    {
                        currIMEStr->push_back(event.edit.text[i]);
                    }
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
            ImGui::ShowFontSelector("Fonts");

            ImGui::Text("Hello World!");
            ImGui::Button("This Button");

            ImGui::Spacing();

            // Ư�� ��Ʈ�� ���
            ImGui::PushFont(userFontB);
            {
                ImGui::Text("Hello World!");
                ImGui::Button("This Button");
            }
            ImGui::PopFont();

            ImGui::Spacing();

            // Ư�� ��Ʈ�� ���
            ImGui::PushFont(defaultFont);
            {
                ImGui::Text("Hello World!");
                ImGui::Button("This Button");
            }
            ImGui::PopFont();

            ImGui::Spacing();

            ImGui::Text("NanumGothic.ttf Only");
            ImGui::Text(u8"�ѱ��� ǥ���� �� �ֳ���?!");

            // static char s_BuffA[255] = u8"�ѱ�";
            // ImGui::InputText("UTF-8##1", &s_BuffA, _countof(s_BuffA));

            // static char s_BuffB[255] = u8"�ѱ�";
            // ImGui::InputText("UTF-8##2", &s_BuffB, _countof(s_BuffB));

            // ���ڿ� ���� ��ȿȭ
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                currIMEStr = nullptr;
                isTextEditing = false;
                prevEditingCnt = 0;
            }

            static std::string s_StrA = u8"�ѱ�";
            ImGui::Text("%s", s_StrA.c_str());
            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    // std::cout << "AAA\n";

                    currIMEStr = &s_StrA;
                }
            }

            // Ŀ��
            if (currIMEStr == &s_StrA)
            {
                ImVec2 lineEnd   = ImGui::GetItemRectMax();
                ImVec2 lineStart = ImVec2{ lineEnd.x, ImGui::GetItemRectMin().y };

                lineEnd.x += 1.0f;

                ImGui::GetWindowDrawList()->AddRectFilled(lineStart, lineEnd, IM_COL32(255, 255, 255, 255));
            }

            static std::string s_StrB = u8"�ѱ�";
            ImGui::InputText("UTF-8##2", &s_StrB);
            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    // std::cout << "BBB\n";

                    currIMEStr = &s_StrB;
                }
            }

            // https://github.com/ocornut/imgui/issues/186
            // Callback���� �����ϰ� �ϴ� �� ���߿� �����ϵ��� ����.
            // �ϴ��� ReadOnly�� ó���ؼ� IME�� �ڵ鸵�ϵ��� �Ѵ�.
            // ImGui::InputText("UTF-8##2", s_BuffB, _countof(s_BuffB), ImGuiInputTextFlags_CallbackEdit,
            //     [](ImGuiInputTextCallbackData* data) -> int {
            // 
            //         std::cout << data->CursorPos << '\n';
            // 
            //         return 0;
            //     });
            // 
            // if (ImGui::IsItemHovered())
            // {
            //     if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            //     {
            //         // std::cout << "BBB\n";
            //     }
            // }

            /**
             * �ݹ����� CursorPos�� �ް� IME �۾��� SDL���� �����ϴ� �͵� �ϳ��� ����� �� ����.
             */

            // ImGui::GetForegroundDrawList()->AddRectFilled(minPos, maxPos, IM_COL32(255, 0, 255, 255));
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