#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <imgui.h>
#include <implot.h>

#include "SimpleIni.h"

namespace fs = std::filesystem;

class WindowBase
{
public:
    static constexpr auto kMainWindowFlags = 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

public:
    WindowBase() : _ini{}
    { }

    virtual ~WindowBase()
    { }

public:
    void UpdateSystemWindowSize(ImVec2 systemWindowSize);

    // SettingsMenuBar()를 쓰기 위해선 ImGuiWindowFlags_MenuBar로 Window를 생성해야 한다.
    void SettingsMenuBar();

    void LoadTheme();
    void SaveTheme();

protected:
    void drawColorsSettings(bool* open = nullptr);
    static ImGuiStyle defaultColorStyle();

public:
    ImVec2 GetSystemWindowSize() const;
    ImVec2 GetNextSize() const;
    ImVec2 GetNextPos() const;

public:
    virtual void Draw(const std::string& windowName, bool* open = nullptr) = 0;

protected:
    CSimpleIniA _ini;

private:
    ImVec2 _systemWindowSize;
};
