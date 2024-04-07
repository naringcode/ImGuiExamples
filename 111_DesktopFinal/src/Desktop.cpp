#include "Desktop.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/Final/src

void Desktop::Draw(const std::string& windowName, bool* open)
{
    ImGui::SetNextWindowPos(this->GetNextPos());
    ImGui::SetNextWindowSize(this->GetNextSize());

    ImGui::Begin(windowName.c_str(), nullptr, kMainWindowFlags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);
    
    this->SettingsMenuBar();

    drawBackground();
    drawDesktop();
    drawTaskbar();

    ImGui::End();
}

void Desktop::drawBackground()
{
    if (nullptr == _bgImageChunk)
        return;

    // ImGui::Begin("Img Test");

    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowSize().x / 2 - _bgImageChunk->width / 2,
                                ImGui::GetWindowSize().y / 2 - _bgImageChunk->height / 2 });

    DisplayImage(*_bgImageChunk);

    ImGui::SetCursorPos(ImVec2{ 0.0f, 0.0f });

    // ImGui::End();
}

void Desktop::drawDesktop()
{
    for (auto& icon : _icons)
    {
        icon.Draw(*this);
    }
}

void Desktop::drawTaskbar()
{
    static constexpr auto kTaskbarFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar;

    auto taskbarSize = ImVec2{ ImGui::GetWindowSize().x, 40.0f };
    // auto taskbarPos = ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - taskbarSize.y };
    auto taskbarPos  = ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - 40.0f }; // taskbarSize.y�� ũ�⸦ ������ �̸� ����.

    ImGui::SetNextWindowSize(taskbarSize);
    ImGui::SetNextWindowPos(taskbarPos);

    static bool openTaskbar = false;

    ImGui::Begin("Taskbar", nullptr, kTaskbarFlags);

    if (ImGui::Button("All Icons", ImVec2{ 100.0F, 30.0F }))
    {
        ImGui::OpenPopup("My Programs");

        openTaskbar = true;
    }

    if (openTaskbar)
    {
        showIconList(&openTaskbar);
    }

    ImGui::SameLine();

    static auto themeOpen = false;

    if (ImGui::Button("Theme", ImVec2{ 100.0F, 30.0F }) || themeOpen)
    {
        themeOpen = true;

        drawColorsSettings(&themeOpen);
    }

    ImGui::SameLine();

    // ��ġ�� ����������
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 100.0f);

    _clock.GetTime();

    static auto clockOpen = false;
    const  auto time = std::format("{:02}:{:02}:{:02}", _clock._hrs, _clock._mins, _clock._hrs);

    if (ImGui::Button(time.data(), ImVec2{ 100.0f, 30.0f }) || clockOpen)
    {
        _clock.UpdateSystemWindowSize(this->GetSystemWindowSize());

        _clock.Draw("ClockWindow");

        clockOpen = true;
    }

    // ȭ���� �������� �� ���콺 ���� ��ư�� ������ ������ ��.
    if (clockOpen && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        clockOpen = false;
    }

    ImGui::End();
}

void Desktop::showIconList(bool* open)
{
    const auto kSelectableHeight = ImGui::GetTextLineHeightWithSpacing();
    const auto kPopupHeight      = kSelectableHeight * _icons.size() + 40.0f;

    // ImGuiCond_Once // �� ��Ÿ�� �� ������ �Ӽ��� �� ���� �ݿ�
    // ImGuiCond_Always // ������ �Ӽ��� �׻� �ݿ�(ImGuiCond_None�� ����)
    // ImGuiCond_FirstUseEver // ini ���Ͽ� ����� ������ ������ ������ �Ӽ��� ���(�̹� �ִ� �����͸� �ش� ���� ���)
    ImGui::SetNextWindowSize(ImVec2{ 120.0f, kPopupHeight }, ImGuiCond_Always);
    // ImGui::SetNextWindowPos(ImVec2{ 0.0f, 680.0f - kPopupHeight }, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - kPopupHeight - 40.0f}, ImGuiCond_Always);

    if (ImGui::BeginPopupModal("My Programs", open, ImGuiWindowFlags_NoResize))
    {
        for (auto& icon : _icons)
        {
            if (ImGui::Selectable(icon.label.data()))
            {
                icon.popupOpen = true;

                icon.base->UpdateSystemWindowSize(GetSystemWindowSize());

                // icon.base->Draw(icon.label, &icon.popupOpen);

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void Desktop::Icon::Draw(WindowBase& parent)
{
    static constexpr auto kButtonSize = ImVec2{ 100.0f, 50.0f };

    const auto labelWindow = std::format("{}##IconWindow", this->label);
    const auto labelPopup  = std::format("{}##IconPopup", this->label);

    // ImGui::SetNextWindowSize();
    // ImGui::SetNextWindowPos();

    ImGui::Begin(labelWindow.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button(this->label.data(), kButtonSize) || popupOpen)
    {
        popupOpen = true;

        base->UpdateSystemWindowSize(parent.GetSystemWindowSize());
        base->Draw(label, &popupOpen);
    }

    ImGui::End();
}

void Render(Desktop& window, int systemWindowWidth, int systemWindowHeight)
{
    window.UpdateSystemWindowSize(ImVec2{ (float)systemWindowWidth, (float)systemWindowHeight });

    window.Draw("Window Name");
}
