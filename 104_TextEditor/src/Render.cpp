#include "Render.h"

#include <fstream>
#include <format>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// InputTextMultiline() -> InputTextEx() -> GetID() -> GetInputTextState() : imgui_internal.h에 있는 내용을 직접 쓰기 위함
#include <imgui_internal.h>

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/TextEditor/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    drawMenu();
    drawContents();
    drawInfo();

    ImGui::End();
}

void WidgetWindow::drawMenu()
{
    const bool kCtrlPressed = ImGui::GetIO().KeyCtrl;
    const bool kSPressed    = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S));
    const bool kLPressed    = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_L));
    // const bool kCPressed    = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C));

    if (ImGui::Button("Save") || (kCtrlPressed && kSPressed))
    {
        ImGui::OpenPopup("Save File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load") || (kCtrlPressed && kLPressed))
    {
        ImGui::OpenPopup("Load File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        std::memset(_textBuffer, 0x00, kBufferSize);
    }

    drawSavePopup();
    drawLoadPopup();
}

void WidgetWindow::drawSavePopup()
{
    static char saveFilenameBuffer[256] = "text.txt";
    const  bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    ImVec2 popupCenterPos;
    {
        popupCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - kPopupSize.x / 2.0f;
        popupCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - kPopupSize.y / 2.0f;
    }

    ImGui::SetNextWindowPos(popupCenterPos);
    ImGui::SetNextWindowSize(kPopupSize);

    if (ImGui::BeginPopupModal("Save File", nullptr, kPopupFlags))
    {
        ImGui::InputText("Filename", saveFilenameBuffer, sizeof(saveFilenameBuffer));

        if (ImGui::Button("Save", kPopupButtonSize))
        {
            saveToFile(saveFilenameBuffer);

            _currentFilename = saveFilenameBuffer;

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", kPopupButtonSize) || kEscPressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::drawLoadPopup()
{
    static char loadFilenameBuffer[256] = "text.txt";
    const  bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    ImVec2 popupCenterPos;
    {
        popupCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - kPopupSize.x / 2.0f;
        popupCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - kPopupSize.y / 2.0f;
    }

    ImGui::SetNextWindowPos(popupCenterPos);
    ImGui::SetNextWindowSize(kPopupSize);

    if (ImGui::BeginPopupModal("Load File"))
    {
        ImGui::InputText("Filename", loadFilenameBuffer, sizeof(loadFilenameBuffer));

        if (ImGui::Button("Load", kPopupButtonSize))
        {
            loadFromFile(loadFilenameBuffer);

            _currentFilename = loadFilenameBuffer;

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", kPopupButtonSize) || kEscPressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::drawContents()
{
    static constexpr auto kInputTextSize  = ImVec2{ 1200.f, 625.f };
    static constexpr auto kLineNumberSize = ImVec2{ 30.0f, kInputTextSize.y };
    static constexpr auto kInputTextFlags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll;

    static float scrollY = 0.0f;

    // ImGuiID inputFieldID = ImGui::GetCurrentWindow()->GetID("##InputField");
    // ImGuiInputTextState* inputTextState = ImGui::GetInputTextState(inputFieldID);

    // Window 내 Child Window를 만드는 개념
    ImGui::BeginChild("LineNumbers", kLineNumberSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    {
        // 기본 텍스트 크기 : 13
        // auto t = ImGui::CalcTextSize("Hello");

        // Dummy()를 쓰면 원하는 크기만큼 스페이싱 가능(ItemSpacing 스타일에는 영향을 받음)
        // ImGui::Dummy(ImVec2{ 0.0f, 0.0f });

        // Ctrl + A로 텍스트를 삭제하면 하나만 카운팅되는 버그가 있음.
        const auto kLineCount = std::count(_textBuffer, _textBuffer + kBufferSize, '\n') + 1;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 8.0f, ImGui::GetStyle().FramePadding.y });
        {
            ImGui::Spacing();
        }
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 8.0f, 0.0f });
        {
            for (auto i = 1; i <= kLineCount; i++)
            {
                ImGui::Text("%d", i);
            }
        }
        ImGui::PopStyleVar();

        // 스크롤링을 위한 비어있는 영역 생성(Spacing()은 안 되는 듯?)
        ImGui::Dummy(ImVec2{ 1.0f, 10.0f });

        // ImGui::SetScrollY(inputTextState->ScrollX);

        // 이전에 지정한 스크롤 위치로 이동(1프레임 늦음)
        ImGui::SetScrollY(scrollY);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::InputTextMultiline("##InputField", _textBuffer, kBufferSize, kInputTextSize, kInputTextFlags);

    // auto temp = ImGui::FindWindowByName(std::format("{}/{}", "##InputField", ));

    // ImGuiWindow* window = ImGui::GetCurrentWindow();
    // scrollY = window->Scroll.y;

    // https://github.com/ocornut/imgui/issues/1224#issuecomment-1306595172
    // g.CurrentWindow <- 이 코드를 사용하는 것이 핵심임(InputTextMultiline(), BeginChild() 둘 다 사용함).
    ImGui::BeginChild("##InputField");
    {
        scrollY = ImGui::GetScrollY();
    }
    ImGui::EndChild();
}

void WidgetWindow::drawInfo()
{
    if (0 == _currentFilename.size())
    {
        ImGui::Text("No File Opened!");

        return;
    }

    const auto fileExtension = getFileExtension(_currentFilename);

    ImGui::Text("Opened file %s | File extension %s", _currentFilename.data(), fileExtension.data());
}

void WidgetWindow::saveToFile(const std::string_view& filename)
{
    // 파일 열기
    auto out = std::ofstream{ filename.data() };

    if (out.is_open()) // 파일이 열렸는지 확인
    {
        out << _textBuffer;
        out.close();
    }
}

void WidgetWindow::loadFromFile(const std::string_view& filename)
{
    // 파일 읽기
    auto in = std::ifstream{ filename.data() };

    if (in.is_open())
    {
        auto buffer = std::stringstream{ };

        buffer << in.rdbuf(); // read buffer

        std::memcpy(_textBuffer, buffer.str().data(), kBufferSize);

        in.close();
    }
}

std::string WidgetWindow::getFileExtension(const std::string_view& filename)
{
    const auto filePath = fs::path{ filename };

    return filePath.extension().string();
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}
