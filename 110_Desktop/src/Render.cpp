#include "Render.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/1_HelloWorld/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f - 40.0f }); // 40.0f : height of taskbar

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    drawDesktop();
    drawTaskbar(systemWindowWidth, systemWindowHeight);

    ImGui::End();
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}

void WidgetWindow::drawDesktop()
{
    for (auto& icon : _icons)
    {
        icon.Draw();
    }
}

void WidgetWindow::drawTaskbar(int systemWindowWidth, int systemWindowHeight)
{
    static constexpr auto kTaskbarFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar;

    auto taskbarSize = ImVec2{ ImGui::GetWindowSize().x, 40.0f };
    // auto taskbarPos = ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - taskbarSize.y };
    auto taskbarPos  = ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y }; // taskbarSize.y의 크기를 사전에 미리 빼둠.

    ImGui::SetNextWindowSize(taskbarSize);
    ImGui::SetNextWindowPos(taskbarPos);

    static bool openTaskbar = false;

    ImGui::Begin("Taskbar", nullptr, kTaskbarFlags);

    if (ImGui::Button("All Icons"))
    {
        ImGui::OpenPopup("My Programs");

        openTaskbar = true;
    }

    if (openTaskbar)
    {
        showIconList();
    }

    ImGui::SameLine();

    // 위치를 오른쪽으로
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x);

    _clock.GetTime();

    static auto clockOpen = false;
    const  auto time = std::format("{}:{}:{}", _clock._hrs, _clock._mins, _clock._hrs);

    if (ImGui::Button(time.data(), ImVec2{ 100.0f, 30.0f }) || clockOpen)
    {
        _clock.Draw("ClockWindow", systemWindowWidth, systemWindowHeight);

        clockOpen = true;
    }

    // 화면이 열려있을 때 마우스 왼쪽 버튼을 누르면 닫히게 함.
    if (clockOpen && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        clockOpen = false;
    }

    ImGui::End();
}

void WidgetWindow::showIconList(bool* open)
{
    const auto kSelectableHeight = ImGui::GetTextLineHeightWithSpacing();
    const auto kPopupHeight      = kSelectableHeight * kNumIcons + 40.0f;

    // ImGuiCond_Once // 매 런타임 시 지정한 속성을 한 번만 반영
    // ImGuiCond_Always // 지정한 속성을 항상 반영(ImGuiCond_None과 같음)
    // ImGuiCond_FirstUseEver // ini 파일에 저장된 내역이 없으면 지정한 속성을 기록(이미 있는 데이터면 해당 값을 사용)
    ImGui::SetNextWindowSize(ImVec2{ 100.0f, kPopupHeight }, ImGuiCond_Always);
    // ImGui::SetNextWindowPos(ImVec2{ 0.0f, 680.0f - kPopupHeight }, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - kPopupHeight - 40.0f}, ImGuiCond_Always);

    if (ImGui::BeginPopupModal("My Programs", open, ImGuiWindowFlags_NoResize))
    {
        for (auto& icon : _icons)
        {
            if (ImGui::Selectable(icon.label.data()))
            {
                icon.popupOpen = true;

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::Icon::Draw()
{
    static constexpr auto kButtonSize = ImVec2{ 100.0f, 50.0f };

    const auto labelWindow = std::format("IconWindow##{}", this->label);
    const auto labelPopup  = std::format("IconPopup##{}", this->label);

    // ImGui::SetNextWindowSize();
    // ImGui::SetNextWindowPos();

    ImGui::Begin(labelWindow.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button(this->label.data(), kButtonSize))
    {
        clickCount++;
    }

    // Popup을 여는 코드
    if (clickCount > 0 || popupOpen)
    {
        ImGui::OpenPopup(labelPopup.data());

        clickCount = 0;
        popupOpen  = true;
    }

    if (ImGui::BeginPopupModal(labelPopup.data(), &popupOpen))
    {
        // ImGui::Text("Hi");
        ImGui::Text("%s", this->label.c_str());

        if (ImGui::Button("Close"))
        {
            popupOpen = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}
