#pragma once

/**
 * Originally referenced from https://github.com/BalazsJako/ImGuiColorTextEdit
 * Customized by naringcode (naringcode@gmail.com)
 * 
 * version : imgui:x64-windows 1.90.2
 */
#include <cstdint>

#include <vector>
#include <unordered_map>

#include <string>
#include <string_view>

#include <functional>

#include "imgui.h"

class TextEditor
{
public:
    struct CharInfo
    {
        char ch;
    };

    using TextLine = std::vector<CharInfo>;

    struct StyleVarItem
    {
        ImGuiStyleVar idx;
        ImVec2        val;
    };

    struct StyleColorItem
    {
        ImGuiCol idx;
        ImVec4   col;
    };

public:
    TextEditor() = default;
    ~TextEditor() = default;

public:
    void Render(bool* open = nullptr);

protected:
    void beginStyle()
    {
        _styleVarStackList.emplace_back();
        _styleColStackList.emplace_back();
    }

    void endStyle()
    {
        this->popStyleVar(this->getCurrentStyleVarStackSize());
        this->popStyleColor(this->getCurrentStyleColorStackSize());

        _styleVarStackList.pop_back();
        _styleColStackList.pop_back();
    }

    void pushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
    {
        ImGui::PushStyleVar(idx, val);

        _styleVarStackList.back().push_back({ idx, val });
    }

    void popStyleVar(int count)
    {
        ImGui::PopStyleVar(count);

        while (count-- > 0)
        {
            _styleVarStackList.back().pop_back();
        }
    }

    void pushStyleColor(ImGuiCol idx, const ImVec4& col)
    {
        ImGui::PushStyleColor(idx, col);

        _styleColStackList.back().push_back({ idx, col });
    }

    void popStyleColor(int count)
    {
        ImGui::PopStyleColor(count);

        while (count-- > 0)
        {
            _styleColStackList.back().pop_back();
        }
    }

    auto getCurrentStyleVarStackSize() const -> int
    {
        return (int)_styleVarStackList.back().size();
    }

    auto getCurrentStyleColorStackSize() const -> int
    {
        return (int)_styleColStackList.back().size();
    }

public:
    void SetWindowTitle(const std::string_view& windowTitle /* = "default window name" */)
    {
        _windowTitle = windowTitle;
    }

    void SetWindowFlags(ImGuiWindowFlags windowFlags /* = ImGuiWindowFlags_None*/)
    {
        _windowFlags = windowFlags | ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_MenuBar;
    }

    void SetNextWindowSize(const ImVec2& nextWindowSize, ImGuiCond condition = ImGuiCond_None)
    {
        ImGui::SetNextWindowSize(nextWindowSize, condition);
    }

    void SetNextWindowPos(const ImVec2& nextWindowPos, ImGuiCond condition = ImGuiCond_None, const ImVec2& pivot = { 0, 0 })
    {
        ImGui::SetNextWindowPos(nextWindowPos, condition, pivot);
    }

    void SetRenderMenuCallback(std::function<void(TextEditor&)> callback)
    {
        _renderMenuCallback = callback;
    }

    void SetRenderHeaderCallback(std::function<void(TextEditor&)> callback)
    {
        _renderHeaderCallback = callback;
    }

    void SetRenderFooterCallback(std::function<void(TextEditor&)> callback)
    {
        _renderFooterCallback = callback;
    }

    void SetText(const std::string_view& text);

    auto GetTotalLines() const -> int
    {
        return (int)_textLines.size();
    }

private:
    std::string _windowTitle = "Text Editor Window";
    uint32_t    _windowFlags = ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_MenuBar;

    std::function<void(TextEditor&)> _renderMenuCallback = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderHeaderCallback = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderFooterCallback = nullptr; // [](TextEditor&) {};

    std::vector<std::vector<StyleVarItem>>   _styleVarStackList;
    std::vector<std::vector<StyleColorItem>> _styleColStackList;

    std::vector<TextLine> _textLines;
};
