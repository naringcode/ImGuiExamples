#pragma once

/**
 * Originally referenced from https://github.com/BalazsJako/ImGuiColorTextEdit
 * Customized by naringcode (naringcode@gmail.com)
 * 
 * version : imgui:x64-windows 1.90.2
 */
#include <cstdint>

#include <string>
#include <string_view>

#include "imgui.h"

class TextEditor
{
public:
    TextEditor() = default;
    ~TextEditor() = default;

public:
    void Render(bool* open = nullptr);

public:
    void SetWindowTitle(const std::string_view& windowTitle /* = "default window name" */)
    {
        _windowTitle = windowTitle;
    }

    void SetWindowFlags(uint32_t windowFlags /* = ImGuiWindowFlags_None*/)
    {
        _windowFlags = windowFlags | ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_MenuBar;
    }

    void SetNextWindowSize(const ImVec2& nextWindowSize, uint32_t condition = ImGuiCond_None)
    {
        ImGui::SetNextWindowSize(nextWindowSize, condition);
    }

    void SetNextWindowPos(const ImVec2& nextWindowPos, uint32_t condition = ImGuiCond_None, const ImVec2& pivot = { 0, 0 })
    {
        ImGui::SetNextWindowPos(nextWindowPos, condition, pivot);
    }

private:
    std::string _windowTitle = "Text Editor Window";
    uint32_t    _windowFlags = ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_MenuBar;
};
