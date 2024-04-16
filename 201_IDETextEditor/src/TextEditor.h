#pragma once

/**
 * Originally referenced from https://github.com/BalazsJako/ImGuiColorTextEdit
 * Customized by naringcode (naringcode@gmail.com)
 * 
 * version : imgui:x64-windows 1.90.2
 */
#include <cstdint>

#include <array>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <string>
#include <string_view>

#include <functional>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

class TextEditor
{
public:
    struct CharInfo
    {
        char ch;
    };

    struct TextLine
    {
        std::vector<CharInfo> text; // without '\0'

        ImVec2 size; // text line size
    };

    struct LineNumber
    {
        std::string numStr;

        ImVec2 size;
    };

    // using TextLine = std::vector<CharInfo>;

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

    struct LineCoordinate
    {
        int32_t lineNo;
        int32_t column;
    };

    struct LineSelection
    {
        LineCoordinate start;
        LineCoordinate end;

        LineCoordinate cursor;
    };

public:
    TextEditor()
    {
        _textLines.push_back(TextLine{ }); // empty text
    }

    ~TextEditor() = default;

    TextEditor(const TextEditor& rhs) = delete;
    TextEditor(TextEditor&& rhs) noexcept = delete;

    TextEditor& operator=(const TextEditor& rhs) = delete;
    TextEditor& operator=(TextEditor&& rhs) noexcept = delete;


public:
    void RenderWindow(bool* open = nullptr);
    void RenderChildWindow(ImVec2 editorFrameSize = ImVec2{ 0.0f, 0.0f });

private:
    void handleKeyboardInputs();
    void handleMouseInputs();

    void renderMenu();
    void renderHeader(float editorFrameWidth);
    void renderEditor(ImVec2 editorFrameSize);
    void renderFooter(float editorFrameWidth);

private:
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

    void pushStyleVar(ImGuiStyleVar idx, float val)
    {
        ImGui::PushStyleVar(idx, val);

        _styleVarStackList.back().push_back({ idx, ImVec2{ val, -1.0f } });
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
    void updateBeforeRender();
    void updateAfterRender();

    void calcAllTextLineSizes();
    void calcTextLineSize(int32_t lineIdx);

    void updateAdditionalLineNums();

    void calcAllLineNumSizes(); // call when we change a font type
    void calcLineNumSize(int32_t lineNum);

    // TODO : how to colorize comments?
    // void colorizeAllTextLines();
    // void colorizeTextLine(int32_t lineIdx);

    auto getMaxTextLineWidth() const -> float;
    auto getMainContentRegionFullSize() const -> ImVec2;

    auto getMaxLineNumWidth() const -> float;

public:
    void SetWindowFlags(ImGuiWindowFlags windowFlags /* = ImGuiWindowFlags_None*/)
    {
        _windowFlags = windowFlags; // | ImGuiWindowFlags_MenuBar;
    }

    void SetWindowTitle(const std::string_view& windowTitle /* = "default window name" */)
    {
        _windowTitle = windowTitle;
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
    uint32_t    _windowFlags = ImGuiWindowFlags_None; // | ImGuiWindowFlags_MenuBar;
    std::string _windowTitle = "Text Editor Window";

    std::function<void(TextEditor&)> _renderMenuCallback   = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderHeaderCallback = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderFooterCallback = nullptr; // [](TextEditor&) {};

    std::vector<std::vector<StyleVarItem>>   _styleVarStackList;
    std::vector<std::vector<StyleColorItem>> _styleColStackList;

    std::vector<TextLine>   _textLines;
    std::vector<LineNumber> _lineNums;

    std::unordered_set<int32_t> _breakPoints; // TODO : thread safe

    float _footerSpacingY = 0.0f; // to calculate the height of contents body

    /**
     * Update
     */
    bool _deferredUpdate_calcAllTextLineSizes = false;
    bool _deferredUpdate_calcAllLineNumSizes  = false; // set true when we change a font type

    std::vector<int32_t> _deferredUpdate_additionalLineNumIndices;

    /**
     * Rendering Info
     */
    ImVec2 _mainContentPadding = ImVec2{ 0.0f, 4.0f };
    float  _itemSpacingY = 3.0f;

    float _lineNumLeftSpacing  = 12.0f;
    float _lineNumRightSpacing = 4.0f;

    float _textLineLeftSpacing  = 8.0f;
    float _textLineRightSpacing = 100.0f;

    float _lineHeight = 1.0f;

    // the length of text line cannot be defined at a compile time.
    std::string _textBuffer = "";
};
