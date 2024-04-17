// #pragma execution_character_set("utf-8")
// #define IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS // https://qiita.com/babiron_i/items/759d80965b497384bc0e

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

#include <imgui_internal.h> // ImGuiContext

// 폰트에 대한 자세한 내용은 아래 링크 참고하기
// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md

// IME 구현체에 대한 내용은 아래 링크 참고하기
// ImGui_ImplSDL2_SetPlatformImeData() 쪽 참고하면 됨(버그 있어서 그런지 잘 작동하지 않음(한글 IME가 안 됨))
// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl2.cpp


/**
 * UTF-8 텍스트 입력 과정
 * 
 * 1. 텍스트 선택
 * 2. 타이핑 진행(SDL_TEXTEDITING)
 *    2-1. 타이핑된 내역이 있으면 UTF-8 카운트만큼 문자열을 Pop
 *    2-2. 타이핑 내역을 저장
 * 3. 글자가 완성되면 타이핑 저장 내역 초기화(SDL_TEXTINPUT)
 * 
 * # 글자가 타이핑되고 있는 도중에 Backspace를 눌러도 SDLK_BACKSPACE 이벤트는 호출되지 않음.
 * # 해당 이벤트는 타이핑되고 있는 문자가 없을 때 호출된다.
 */

// UTF-8 한 글자가 몇 바이트로 이루어져 있는지 확인
int ByteCountOfLastCharFromUTF8String(const std::string& utf8Str)
{
    /**
     * ## UTF-8 문자 시작 패턴 ##
     * 
     * ASCII 문자 (U+0000 ~ U+007F): 0xxxxxxx
     * 2바이트 문자 (U+0080 ~ U+07FF): 110xxxxx 10xxxxxx
     * 3바이트 문자 (U+0800 ~ U+FFFF): 1110xxxx 10xxxxxx 10xxxxxx
     * 4바이트 문자 (U+10000 ~ U+10FFFF): 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */
    if (0 == utf8Str.length())
        return 0;

    // 하나는 확정(적어도 아스키코드 하나가 있다고 가정)
    int res = 1;

    for (int idx = utf8Str.size() - 1; idx >= 0; idx--)
    {
        // 0xC0 : 1100 0000
        // 0x80 : 1000 1000
        // 상위 2비트를 뽑았을 때 0x80이 아니라면 다른 인코딩 문자(위 문자 테이블 비교해서 보도록 하기)
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
    // 한글 IME는 안 되는 것 같음(DirectX나 OpenGL에서는 되는데 SDL에서만 안 됨)
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

    // IME를 지원하지 않으면 직접 만들어 쓴다.
    std::string* currIMEStr    = nullptr;
    bool         isTextEditing = false;
    int          prevEditingCnt = 0;

    /**
     * 폰트 HERE
     */
    // AddFontFromFileTTF()로 추가한 첫 번째 폰트가 기본 폰트로 설정됨.
    ImFont* userFontA = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 24, nullptr, io.Fonts->GetGlyphRangesKorean());
    ImFont* userFontB = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Consola.ttf", 16, nullptr, io.Fonts->GetGlyphRangesKorean());
    ImFont* userFontC = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/NanumGothic.ttf", 14, nullptr, io.Fonts->GetGlyphRangesKorean());
    
    ImFont* defaultFont = io.Fonts->AddFontDefault();

    // combine fonts (https://github.com/ocornut/imgui/blob/master/docs/FONTS.md | Combine multiple fonts into one)
    ImFontConfig fontConfig;
    strcpy_s(fontConfig.Name, "Combined(Consolas + NanumGothic), 12pt");

    ImFont* combinedFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Consola.ttf", 12, &fontConfig);
    fontConfig.MergeMode = true; // MergeMode를 true로 하는 순간 AddFontXXX() 계열을 호출할 때마다 fontConfig에 결합됨.
    
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
                    // IME가 적용되고 있는 중이면 안 들어온다(타이핑되고 있는 것이 없어야 들어오는 것 확인함).
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
                // 텍스트 한 칸 입력이 완료되면 들어오는 이벤트
                // ex) ㅎ -> 하 -> 한 -> SDL_TEXTINPUT
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
                // 텍스트를 입력하는 도중에 들어오는 이벤트
                // ex) 한 : ㅎ(SDL_TEXTEDITING) -> 하(SDL_TEXTEDITING) -> 한(SDL_TEXTEDITING)
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
                        // 중간 문자열은 나중에 따로 고민해봐야 함(일단 맨 뒤에 있는 문자만 처리하는 방식)
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

            // 특정 폰트로 출력
            ImGui::PushFont(userFontB);
            {
                ImGui::Text("Hello World!");
                ImGui::Button("This Button");
            }
            ImGui::PopFont();

            ImGui::Spacing();

            // 특정 폰트로 출력
            ImGui::PushFont(defaultFont);
            {
                ImGui::Text("Hello World!");
                ImGui::Button("This Button");
            }
            ImGui::PopFont();

            ImGui::Spacing();

            ImGui::Text("NanumGothic.ttf Only");
            ImGui::Text(u8"한글은 표현할 수 있나요?!");

            // static char s_BuffA[255] = u8"한글";
            // ImGui::InputText("UTF-8##1", &s_BuffA, _countof(s_BuffA));

            // static char s_BuffB[255] = u8"한글";
            // ImGui::InputText("UTF-8##2", &s_BuffB, _countof(s_BuffB));

            // 문자열 설정 무효화
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                currIMEStr = nullptr;
                isTextEditing = false;
                prevEditingCnt = 0;
            }

            static std::string s_StrA = u8"한글";
            ImGui::Text("%s", s_StrA.c_str());
            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    // std::cout << "AAA\n";

                    currIMEStr = &s_StrA;
                }
            }

            // 커서
            if (currIMEStr == &s_StrA)
            {
                ImVec2 lineEnd   = ImGui::GetItemRectMax();
                ImVec2 lineStart = ImVec2{ lineEnd.x, ImGui::GetItemRectMin().y };

                lineEnd.x += 1.0f;

                ImGui::GetWindowDrawList()->AddRectFilled(lineStart, lineEnd, IM_COL32(255, 255, 255, 255));
            }

            static std::string s_StrB = u8"한글";
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
            // Callback으로 동작하게 하는 건 나중에 연구하도록 하자.
            // 일단은 ReadOnly로 처리해서 IME를 핸들링하도록 한다.
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
             * 콜백으로 CursorPos를 받고 IME 작업을 SDL에서 진행하는 것도 하나의 방법일 것 같다.
             */

            // ImGui::GetForegroundDrawList()->AddRectFilled(minPos, maxPos, IM_COL32(255, 0, 255, 255));
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