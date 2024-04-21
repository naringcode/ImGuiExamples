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
#include <unordered_map>
#include <unordered_set>

#include <string>
#include <string_view>

#include <functional>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

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

        std::vector<float> cumulativeCharWidths; // helpful for finding a column

        float renderingWidth; // text line rendering width(not length, not counts of characters)
    };

    struct LineNumber
    {
        std::string numStr;

        float renderingWidth;
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
        /**
         * column | consider out of bounds(like "if (lineCoord.column > 0) { cumulativeCharWidths[lineCoord.column - 1]; }")
         * 0  1  2  3  4  5
         * ⊿  ⊿  ⊿  ⊿  ⊿  ⊿
         * 忙式式成式式成式式成式式成式式忖 lineNum
         * 弛00弛01弛02弛03弛04弛 ∠ 0
         * 戍式式托式式托式式托式式托式式扣
         * 弛10弛11弛12弛13弛14弛 ∠ 1
         * 戍式式托式式托式式托式式托式式扣
         * 弛20弛21弛22弛23弛24弛 ∠ 2
         * 戌式式扛式式扛式式扛式式扛式式戎
         */
        int32_t lineNum = 0;
        int32_t column  = 0;

        bool operator==(const LineCoordinate& rhs) const
        {
            return lineNum == rhs.lineNum && column == rhs.column;
        }

        bool operator!=(const LineCoordinate& rhs) const
        {
            return lineNum != rhs.lineNum || column != rhs.column;
        }

        bool operator<(const LineCoordinate& rhs) const
        {
            if (lineNum != rhs.lineNum)
                return lineNum < rhs.lineNum;

            return column < rhs.column;
        }

        bool operator<=(const LineCoordinate& rhs) const
        {
            if (lineNum != rhs.lineNum)
                return lineNum < rhs.lineNum;

            return column <= rhs.column;
        }
        
        bool operator>(const LineCoordinate& rhs) const
        {
            if (lineNum != rhs.lineNum)
                return lineNum > rhs.lineNum;

            return column > rhs.column;
        }

        bool operator>=(const LineCoordinate& rhs) const
        {
            if (lineNum != rhs.lineNum)
                return lineNum > rhs.lineNum;

            return column >= rhs.column;
        }
    };

    struct TextSelection
    {
        LineCoordinate start;
        LineCoordinate end;
    };

    enum class TextSelectionMode
    {
        Simple,
        Word, // select as word units
        Line, // select as line units
    };

public:
    TextEditor(ImFont* font = nullptr);
    ~TextEditor() = default;

    TextEditor(const TextEditor& rhs) = delete;
    TextEditor(TextEditor&& rhs) noexcept = delete;

    TextEditor& operator=(const TextEditor& rhs) = delete;
    TextEditor& operator=(TextEditor&& rhs) noexcept = delete;

public:
    void RenderWindow(bool* open = nullptr);
    void RenderChildWindow(ImVec2 editorFrameSize = ImVec2{ 0.0f, 0.0f });

public:
    void ChangeFont(ImFont* nextFont);

private:
    void updateBeforeRender();
    void updateAfterRender();

    void renderMenu();
    void renderHeader(float editorFrameWidth);
    void renderFooter(float editorFrameWidth);
    void renderEditor(ImVec2 editorFrameSize);
    
private:
    void handleMainEditorInputs();

    void handleMainEditorKeyboardInputs();
    void handleMainEditorMouseInputs();

    /**
     * Coordinate
     */
    auto sanitizeCoordinate(const LineCoordinate& lineCoord) const -> LineCoordinate;

    auto getLineCoordinateScreenStartPos() const -> ImVec2;
    auto isValidCoordinate(const LineCoordinate& lineCoord) const -> bool;

    // auto findFirstCharPosInWord()

    /**
     * Mouse
     */
    // check screen pos(especially mouse pos) in the main editor frame except scrollbar frame
    auto isScreenPosInMainEditorFrame(const ImVec2& screenPos) const -> bool;

    /**
     * Converting
     */
    auto convertLineCoordinateToScreenPos(const LineCoordinate& lineCoord) const -> ImVec2;
    auto convertScreenPosToLineCoordinate(const ImVec2& screenPos) const -> LineCoordinate;

    auto convertToFinalTextSelection(const TextSelection& textSelection, TextSelectionMode selectionMode) const -> TextSelection;
    auto convertToCursorCoordinate(const TextSelection& textSelection, TextSelectionMode selectionMode) const -> LineCoordinate;

private:
    /**
     * Text Line
     */
    void calcAllTextLineRenderingSizes();
    void calcTextLineRenderingSize(int32_t lineIdx);

    void updateAdditionalLineNums();

    void calcAllLineNumRenderingSizes(); // call when we change a font type
    void calcLineNumRenderingSize(int32_t lineNum);

    // TODO : how to colorize comments?
    // void colorizeAllTextLines();
    // void colorizeTextLine(int32_t lineIdx);

    auto getMaxTextLineRenderingWidth() const -> float;
    auto getMainContentRegionFullSize() const -> ImVec2;

    auto getMaxLineNumRenderingWidth() const -> float;

    // TODO : UTF8 SUPPORT
    // auto getUtf8CharByteLength() const -> int32_t

private:
    /**
     * Style
     */
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
    /**
     * Public Getters & Setters
     */
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

    auto GetCursorCoordinate() const -> LineCoordinate
    {
        return _finalCursorCoord;
    }

    auto SetCursorCoordinate(const LineCoordinate& cursorCoord)
    {
        _temporaryCursorCoord     = this->sanitizeCoordinate(cursorCoord);
        _temporaryDistanceLineNum = 0;

        _finalCursorCoord = _temporaryCursorCoord;
    }

    auto GetMainContentPaddingX() const -> float
    {
        return _mainContentPadding.x;
    }

    void SetMainContentPaddingX(float mainContentPaddingX)
    {
        _mainContentPadding.x = mainContentPaddingX;
    }

    auto GetMainContentPaddingY() const -> float
    {
        return _mainContentPadding.y;
    }

    void SetMainContentPaddingY(float mainContentPaddingY)
    {
        _mainContentPadding.y = mainContentPaddingY;
    }

    auto GetLineNumLeftSpacing() const -> float
    {
        return _lineNumLeftSpacing;
    }

    void SetLineNumLeftSpacing(float lineNumLeftSpacing)
    {
        _lineNumLeftSpacing = lineNumLeftSpacing;
    }

    auto GetLineNumRightSpacing() const -> float
    {
        return _lineNumRightSpacing;
    }

    void SetLineNumRightSpacing(float lineNumRightSpacing)
    {
        _lineNumRightSpacing = lineNumRightSpacing;
    }

    auto GetTextLineTopSpacing() const -> float
    {
        return _textLineTopSpacing;
    }

    void SetTextLineTopSpacing(float textLineTopSpacing)
    {
        _textLineTopSpacing = textLineTopSpacing;
    }

    auto GetTextLineBottomSpacing() const -> float
    {
        return _textLineBottomSpacing;
    }

    void SetTextLineBottomSpacing(float textLineBottomSpacing)
    {
        _textLineBottomSpacing = textLineBottomSpacing;
    }

    auto GetTextLineLeftSpacing() const -> float
    {
        return _textLineLeftSpacing;
    }

    void SetTextLineLeftSpacing(float textLineLeftSpacing)
    {
        _textLineLeftSpacing = textLineLeftSpacing;
    }

    auto GetTextLineRightSpacing() const -> float
    {
        return _textLineRightSpacing;
    }

    void SetTextLineRightSpacing(float textLineRightSpacing)
    {
        _textLineRightSpacing = textLineRightSpacing;
    }

    auto GetShowBreakPoints() const -> bool
    {
        return _showBreakPoints;
    }

    void SetShowBreakPoints(bool enabled)
    {
        _showBreakPoints = enabled;
    }

    auto GetShowLineNums() const -> bool
    {
        return _showLineNums;
    }

    void SetShowLineNums(bool enabled)
    {
        _showLineNums = enabled;
    }

    auto GetShowLineMarkers() const -> bool
    {
        return _showLineMarkers;
    }

    void SetShowLineMarkers(bool enabled)
    {
        _showLineMarkers = enabled;
    }

    auto GetShowWindowResizeGrip() const -> bool
    {
        return _showWindowResizeGrip;
    }

    void SetShowWindowResizeGrip(bool enabled)
    {
        _showWindowResizeGrip = enabled;
    }

private:
    /**
     * Core
     */
    uint32_t    _windowFlags = ImGuiWindowFlags_None; // | ImGuiWindowFlags_MenuBar;
    std::string _windowTitle = "Text Editor Window";

    std::function<void(TextEditor&)> _renderMenuCallback   = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderHeaderCallback = nullptr; // [](TextEditor&) {};
    std::function<void(TextEditor&)> _renderFooterCallback = nullptr; // [](TextEditor&) {};

    std::vector<std::vector<StyleVarItem>>   _styleVarStackList;
    std::vector<std::vector<StyleColorItem>> _styleColStackList;

    std::vector<TextLine>   _textLines; // _textLines must have one line at least
    std::vector<LineNumber> _lineNums;

    std::unordered_set<int32_t> _breakPoints; // TODO : thread safe

    // the length of text line cannot be defined at a compile time.
    std::string _textBuffer = "";

    /**
     * Update
     */
    bool _deferredUpdate_calcAllTextLineRenderingSizes = false;
    bool _deferredUpdate_calcAllLineNumRenderingSizes  = false; // set true when we change a font type

    std::vector<int32_t> _deferredUpdate_additionalLineNumIndices;

    ImFont* _deferredUpdate_nextFont = nullptr;

    double _currMainTime = 0.0f; // internal value by ImGui::GetTime()

    /**
     * Inputs
     */
    static constexpr int32_t _kTabSize = 4;

    bool _handleEditorKeyboardInputs = true;
    bool _handleEditorMouseInputs    = true;

    bool _mouseEventOnGoing = false;

    // track whether shift, ctrl, or alt keys are pressed when the left mouse button is clicked(not double, not triple)
    bool _leftButtonClickedShiftPressed = false;
    bool _leftButtonClickedCtrlPressed  = false;
    bool _leftButtonClickedAltPressed   = false;

    double _lastLeftButtonDoubleClickedTime = -10.0;

    TextSelectionMode _textSelectionMode = TextSelectionMode::Simple;

    TextSelection _temporaryTextSelection; // metadata before conversion to _finalTextSelection
    TextSelection _finalTextSelection;     // finalized actual text selection used in the TextEditor

    LineCoordinate _temporaryCursorCoord;     // cursor coordinate when left button is clicked once or keyboard event occurs like keyMoveLeft() or keyMoveRight()
    int32_t        _temporaryDistanceLineNum; // line num distance for keyboard event keyEventUp() and keyMoveDown()

    LineCoordinate _firstClickedMousePosCoord; // in order to check double click or triple click event
    LineCoordinate _finalCursorCoord;          // finalized actual cursor coordinate used in the TextEditor sometimes calculated from _temporaryCursorCoord and _temporaryDistanceLineNum
    
    /**
     * Editor Rendering Info
     */
    ImFont* _currFont = nullptr;

    ImVec2 _mainContentPadding = ImVec2{ 0.0f, 4.0f };

    float _lineNumLeftSpacing  = 12.0f;
    float _lineNumRightSpacing = 4.0f;

    float _textLineTopSpacing    = 2.0f;
    float _textLineBottomSpacing = 2.0f;

    float _textLineLeftSpacing  = 8.0f;
    float _textLineRightSpacing = 100.0f;

    float _lineHeight = 1.0f; // automatically calculated internally

    // TODO Soon
    static constexpr double _kCursorBlinkInterval = 400.0;
    double _cursorBlinkingTime = 0.0;

    bool _isCursorBlicking = false; // internal use

    bool _showBreakPoints = true;
    bool _showLineNums    = true;
    bool _showLineMarkers = true;

    bool _showWindowResizeGrip = false;

    float _footerSpacingY = 0.0f; // to calculate the height of contents body(internal use)
};
