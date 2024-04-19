#include "TextEditor.h"

#include <iostream>

#include <format>

#define SHOW_TEXT_LINE_PIVOT_POS
// #define SHOW_TEXT_LINE_START_POS
// #define SHOW_BREAKPOINT_FRAME_RECT
#define SHOW_BREAKPOINT_BUTTON_RECT
// #define SHOW_BREAKPOINT_FRAME_MARGIN_RECT
#define SHOW_MAIN_EDITOR_FRAME_RECT_WHEN_HOVERED
#define SHOW_START_POS_OF_LINE_COORDINATE_WHEN_HOVERED

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

constexpr int32_t g_kShortBufferLen = 256;

void TextEditor::RenderWindow(bool* open)
{
    if (nullptr != open && false == *open)
        return;

    this->updateBeforeRender();

    // Old : PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });

    this->beginStyle();

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 8.0f });
    this->pushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));

    if (false == _showWindowResizeGrip)
    {
        this->pushStyleColor(ImGuiCol_ResizeGrip, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
    }

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        /**
         * Menu
         */
        this->renderMenu();

        /**
         * Header
         */
        this->beginStyle();
        {
            // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });

            this->renderHeader(0.0f);
        }
        this->endStyle();

        /**
         * Editor
         */
        this->beginStyle();
        {
            this->renderEditor(ImVec2{ 0.0f, 0.0f });
        }
        this->endStyle();

        ImGui::Spacing(); // without calling Spacing(), the following footer adjoins the editor closely.

        /**
         * Footer
         */
        this->beginStyle();
        {
            // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });

            this->renderFooter(0.0f);
        }
        this->endStyle();
    }
    ImGui::End(); // Main Window

    this->endStyle();

    this->updateAfterRender();
}

void TextEditor::RenderChildWindow(ImVec2 editorFrameSize)
{
    this->updateBeforeRender();

    // Old : PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });
    
    ImGui::BeginGroup();
    {
        /**
         * Header
         */
        this->beginStyle();
        {
            // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });
        
            this->renderHeader(editorFrameSize.x);
        }
        this->endStyle();

        /**
         * Editor
         */
        this->beginStyle();
        {
            this->renderEditor(editorFrameSize);
        }
        this->endStyle();

        ImGui::Spacing(); // without calling Spacing(), the following footer adjoins the editor closely.

        /**
         * Footer
         */
        this->beginStyle();
        {
            // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });
        
            this->renderFooter(editorFrameSize.x);
        }
        this->endStyle();
    }
    ImGui::EndGroup();

    this->updateAfterRender();
}

void TextEditor::ChangeFont(ImFont* nextFont)
{
    _deferredUpdate_calcAllTextLineSizes = true;
    _deferredUpdate_calcAllLineNumSizes = true;

    _deferredUpdate_nextFont = nextFont;
}

void TextEditor::updateBeforeRender()
{
    /**
     * Update
     */
    if (nullptr != _deferredUpdate_nextFont)
    {
        _currFont = _deferredUpdate_nextFont;

        _deferredUpdate_nextFont = nullptr;
    }

    if (nullptr == _currFont)
    {
        _currFont = ImGui::GetFont();


    }

    if (true == _deferredUpdate_calcAllTextLineSizes)
    {
        _deferredUpdate_calcAllTextLineSizes = false;

        this->calcAllTextLineSizes();
    }

    if (true == _deferredUpdate_calcAllLineNumSizes)
    {
        _deferredUpdate_calcAllLineNumSizes = false;

        this->calcAllLineNumSizes();
    }

    this->updateAdditionalLineNums();
}

void TextEditor::updateAfterRender()
{

}

void TextEditor::renderMenu()
{
    if (_renderMenuCallback && 0 != (_windowFlags & ImGuiWindowFlags_MenuBar))
    {
        _renderMenuCallback(*this);
    }
}

void TextEditor::renderHeader(float editorFrameWidth)
{
    constexpr ImGuiChildFlags  kChildFlags  = /* ImGuiChildFlags_FrameStyle | */ ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    std::array<char, g_kShortBufferLen> strBuffer;

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

    if (_renderHeaderCallback)
    {
        auto backIter = std::format_to(strBuffer.begin(), "{}##Header", _windowTitle);
        *backIter = '\0';

        ImGui::BeginChild(strBuffer.data(), ImVec2{ editorFrameWidth, 0.0f }, kChildFlags, kWindowFlags);
        {
            this->popStyleVar(1);

            _renderHeaderCallback(*this);

        }
        ImGui::EndChild();
    }
}

void TextEditor::renderFooter(float editorFrameWidth)
{
    constexpr ImGuiChildFlags  kChildFlags  = /* ImGuiChildFlags_FrameStyle | */ ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    std::array<char, g_kShortBufferLen> strBuffer;

    _footerSpacingY = 0.0f;

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

    if (_renderFooterCallback)
    {
        auto backIter = std::format_to(strBuffer.begin(), "{}##Footer", _windowTitle);
        *backIter = '\0';

        ImGui::BeginChild(strBuffer.data(), ImVec2{ editorFrameWidth, 0.0f }, kChildFlags, kWindowFlags);
        {
            this->popStyleVar(1);

            _renderFooterCallback(*this);

            _footerSpacingY = ImGui::GetWindowSize().y + ImGui::GetStyle().ItemSpacing.y;
        }
        ImGui::EndChild();
    }
}

void TextEditor::renderEditor(ImVec2 editorFrameSize)
{
    /**
     * Styles
     */
    this->pushStyleColor(ImGuiCol_ChildBg,     ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    //this->pushStyleColor(ImGuiCol_ScrollbarBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_Border,      ImGui::ColorConvertU32ToFloat4(IM_COL32(62, 62, 62, 255)));
    
    // Old : give widths from zero to one by testing, and we handle positions programmatically.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 1.0f, 5.0f });
    // this->pushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2{ 0.0f, 3.0f });

    // we handle positions programmatically.
    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    this->pushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2{ 0.0f, 0.0f });

    this->pushStyleVar(ImGuiStyleVar_ScrollbarSize,   14.0f);
    this->pushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::PushFont(_currFont);
    
    /**
     * Core Variables
     */
    constexpr ImGuiChildFlags  kFrameChildFlags  = ImGuiChildFlags_Border;
    constexpr ImGuiWindowFlags kFrameWindowFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoMove;
    
    constexpr ImGuiChildFlags  kSubChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kSubWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;

    constexpr ImGuiChildFlags  kMainChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kMainWindowFlags = ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove;

    ImGuiStyle& style          = ImGui::GetStyle();
    ImDrawList* parentDrawList = ImGui::GetWindowDrawList();

    const auto   kFrameBorderSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };    
    const ImVec2 kFrameSize       = ImVec2{ 0.0f, 0.0f } != editorFrameSize ? editorFrameSize : ImGui::GetContentRegionAvail() - ImVec2{ 0.0f, _footerSpacingY + kFrameBorderSize.y };

    std::array<char, g_kShortBufferLen> strBuffer;

    auto strBackIter = std::format_to(strBuffer.begin(), "{}##EditorFrame", _windowTitle);
    *strBackIter = '\0';

    // TODO OR NOT : ChildWindow To Group(in order to capture io events simply)
    ImGui::BeginChild(strBuffer.data(), kFrameSize, kFrameChildFlags, kFrameWindowFlags);
    {
        /**
         * Variables for General Info
         */
        const float kFontHeight = _currFont->CalcTextSizeA(_currFont->FontSize, FLT_MAX, 0.0f, "#").y;
        
        _lineHeight = kFontHeight + _textLineTopSpacing + _textLineBottomSpacing;

        const ImVec2 kContentSize = kFrameSize - kFrameBorderSize * 2;

        float scrollX = 0.0f;
        float scrollY = 0.0f;

        /**
         * Variables for Break-Point Frame
         */
        auto breakPointFrameSize = ImVec2{ std::round(kFontHeight * 1.15f), ImGui::GetContentRegionAvail().y - (kFrameBorderSize.y * 2.0f) - style.ScrollbarSize };
            
        ImVec2 breakPointFrameRectMin = ImGui::GetWindowPos() + kFrameBorderSize;
        ImVec2 breakPointFrameRectMax = breakPointFrameRectMin + breakPointFrameSize;
            
        // ImVec2 breakPointButtonSize = ImVec2{ breakPointFrameSize.x, breakPointFrameSize.x + 0.0f };
        ImVec2 breakPointButtonSize = ImVec2{ breakPointFrameSize.x, _lineHeight };

        /**
         * Variable for Line-Number Frame
         */
        const int32_t kTotalLineNums = (int32_t)_textLines.size();
        int32_t minLineIdx = 0;
        int32_t maxLineIdx = 0;

        const float kMaxLineNumWidth = this->getMaxLineNumWidth();

        auto lineNumFrameSize = ImVec2{ kMaxLineNumWidth + _lineNumLeftSpacing + _lineNumRightSpacing, breakPointFrameSize.y };

        /**
         * Sub Content Width
         */
        float subContentWidth = (breakPointFrameSize.x) + (kMaxLineNumWidth + _lineNumLeftSpacing + _lineNumRightSpacing);

        if (false == _showBreakPoints)
        {
            subContentWidth -= breakPointFrameSize.x;

            breakPointFrameSize.x = 0.0f;

            // breakPointFrameRectMax.x = breakPointFrameRectMin.x;
            // breakPointButtonSize.x = 0.0f;
        }

        if (false == _showLineNums)
        {
            subContentWidth -= lineNumFrameSize.x;

            lineNumFrameSize.x = 0.0f;
        }

        /**
         * Variables for Contents
         */
        const ImVec2 kSubContentPos  = ImGui::GetWindowPos() + kFrameBorderSize;
        const ImVec2 kSubContentSize = ImVec2{ subContentWidth, kContentSize.y - style.ScrollbarSize };

        const ImVec2 kMainContentPos  = kSubContentPos + ImVec2{ subContentWidth, 0.0f };
        const ImVec2 kMainContentSize = kContentSize - ImVec2{ kSubContentSize.x, 0.0f };

        /**
         * Main Content (Text Line)
         */
        ImGui::SetNextWindowPos(kMainContentPos);

        strBackIter = std::format_to(strBuffer.begin(), "{}##EditorMain", _windowTitle);
        *strBackIter = '\0';
        
        ImGui::BeginChild(strBuffer.data(), kMainContentSize, kMainChildFlags, kMainWindowFlags);
        {
            this->handleMainEditorInputs();

            //
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();
            
            scrollX = ImGui::GetScrollX();
            scrollY = ImGui::GetScrollY();

            // ** when you are using scrolling and if you want to get the content height, you have to type the following code **
            // scrollY + ImGui::GetContentRegionMax().y
            // scrollY + ImGui::GetWindowContentRegionMax().y
            minLineIdx = (int32_t)std::floor((scrollY - _mainContentPadding.y) / _lineHeight);
            maxLineIdx = minLineIdx + (int32_t)std::floor((scrollY + ImGui::GetWindowContentRegionMax().y) / _lineHeight);

            // adding 1 means that we calculate the maximum height including the index itself
            if ((float)((maxLineIdx + 1) * _lineHeight) < (kMainContentSize.y - style.ScrollbarSize) + scrollY - _mainContentPadding.y)
            {
                maxLineIdx++;
            }

            minLineIdx = std::max(0, minLineIdx);
            maxLineIdx = std::min(maxLineIdx, (int32_t)_textLines.size() - 1);

            ImVec2 renderPivot = _mainContentPadding + ImVec2{ 0.0f, _textLineTopSpacing } + ImVec2{ 0.0f, minLineIdx * _lineHeight };
            
            // Loop
            for (int32_t lineIdx = minLineIdx; lineIdx <= maxLineIdx; lineIdx++)
            {
                ImGui::SetCursorPosX(renderPivot.x);
                ImGui::SetCursorPosY(renderPivot.y);

#ifdef SHOW_TEXT_LINE_PIVOT_POS
                ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetCursorScreenPos() - ImVec2{ 0.0f, _textLineTopSpacing }, 1.6f, IM_COL32(255, 0, 255, 255));
#endif

                // Line Markers
                if (true == _showLineMarkers)
                {
                    auto startPos = ImVec2{ ImGui::GetCursorScreenPos().x + scrollX, ImGui::GetCursorScreenPos().y };
                    auto endPos   = ImVec2{ startPos.x, startPos.y + kFontHeight };

                    childDrawList->AddLine(startPos, endPos, IM_COL32(165, 165, 165, 255));
                }

                /**
                 * Text Line
                 */
                ImGui::SetCursorPosX(renderPivot.x + _textLineLeftSpacing);
                ImGui::SetCursorPosY(renderPivot.y);

#ifdef SHOW_TEXT_LINE_START_POS
                ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));
#endif

                _textBuffer.clear();

                for (auto& charInfo : _textLines[lineIdx].text)
                {
                    _textBuffer.push_back(charInfo.ch);
                }

                childDrawList->AddText(ImGui::GetCursorScreenPos(), IM_COL32_WHITE, _textBuffer.c_str());
                
                // Update
                renderPivot.x = _mainContentPadding.x;
                renderPivot.y += _lineHeight;
            }

            /**
             * Cursor
             */
            if (_cursorCoord.lineNum >= minLineIdx && _cursorCoord.lineNum <= maxLineIdx)
            {
                // constexpr float kCursorWidth = 1.0f;

                bool focused = ImGui::IsWindowFocused();

                if (focused)
                {
                    ImVec2 startPos = this->convertLineCoordinateToScreenPos(_cursorCoord);
                    ImVec2 endPos   = startPos + ImVec2{ 0.0f, _lineHeight };

                    childDrawList->AddLine(startPos, endPos, IM_COL32(220, 220, 220, 255));
                }
            }

            // get the full size of the main editor for scrolling
            ImVec2 editorContentRegion = this->getMainContentRegionFullSize();
            {
                // give an additional region to enable scroll down but have to show at lease one text line
                editorContentRegion.y += kContentSize.y - style.ScrollbarSize - _lineHeight - _mainContentPadding.y;
            }

            ImGui::SetCursorPos(editorContentRegion);
        }
        ImGui::EndChild(); // End of EditorMain
        
        /**
         * Sub Content (Breakpoint, Line Num)
         */
        // in ImGui, the zero size means that ImGui renders the child window by resizing automatically
        if (0 != (int32_t)subContentWidth) 
        {
            ImGui::SameLine();

            ImGui::SetNextWindowPos(kSubContentPos);
            ImGui::SetNextWindowScroll(ImVec2{ 0.0f, scrollY }); // ** scrollY ** this is why we have to render the sub window here.

            strBackIter = std::format_to(strBuffer.begin(), "{}##EditorSub", _windowTitle);
            *strBackIter = '\0';

            ImGui::BeginChild(strBuffer.data(), kSubContentSize, kSubChildFlags, kSubWindowFlags);
            {
                ImDrawList* childDrawList = ImGui::GetWindowDrawList();

                ImVec2 renderPivot = ImVec2{ 0.0f, _mainContentPadding.y + _textLineTopSpacing } + ImVec2{ 0.0f, minLineIdx * _lineHeight };

#ifdef SHOW_BREAKPOINT_FRAME_RECT
                ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointFrameRectMin, 2.0f, IM_COL32_WHITE);
                ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointFrameRectMax, 2.0f, IM_COL32_WHITE);
#endif

                if (true == _showBreakPoints)
                {
                    childDrawList->AddRectFilled(breakPointFrameRectMin, breakPointFrameRectMax, IM_COL32(51, 51, 51, 255));
                }
                
                // Loop
                for (int32_t lineIdx = minLineIdx; lineIdx <= maxLineIdx; lineIdx++)
                {
                    /**
                     * Break-Point
                     */
                    if (true == _showBreakPoints)
                    {
                        ImGui::SetCursorPosX(renderPivot.x);
                        ImGui::SetCursorPosY(renderPivot.y - _textLineTopSpacing - 2.0f); // give -2.0f for a visual beauty.

#ifdef SHOW_BREAKPOINT_BUTTON_RECT
                        ImGui::GetForegroundDrawList()->AddRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointButtonSize.x, breakPointButtonSize.y }, IM_COL32_WHITE);
                        ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);
#endif

                        // Break-Point Pos
                        ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                        {
                            breakPointPos.x += breakPointFrameSize.x * 0.5f;
                            breakPointPos.y += breakPointFrameSize.x * 0.5f + _textLineTopSpacing;
                        }

                        //
                        auto breakPointIter = _breakPoints.find(lineIdx);

                        strBackIter = std::format_to(strBuffer.begin(), "BreakPointBtn##{}", lineIdx);
                        *strBackIter = '\0';

                        // Break-Point Button
                        // if (ImGui::Button(std::format("##{}", lineIdx).c_str(), breakPointButtonSize))
                        if (ImGui::InvisibleButton(strBuffer.data(), breakPointButtonSize))
                        {
                            if (_breakPoints.end() == breakPointIter)
                            {
                                _breakPoints.insert(lineIdx);
                            }
                            else
                            {
                                breakPointIter = _breakPoints.erase(breakPointIter);
                            }
                        }

                        if (_breakPoints.end() != breakPointIter)
                        {
                            if (false == ImGui::IsItemHovered())
                            {
                                childDrawList->AddCircleFilled(breakPointPos, breakPointFrameSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
                            }
                            else // if (ImGui::IsItemHovered())
                            {
                                childDrawList->AddCircleFilled(breakPointPos, breakPointFrameSize.x * 0.4f, IM_COL32(245, 141, 149, 255));
                            }
                        }

                        if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
                        {
                            childDrawList->AddCircleFilled(breakPointPos, breakPointFrameSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
                        }
                    }

                    /**
                     * Line Num
                     */
                    if (true == _showLineNums)
                    {
                        const float kCurrLineNumWidth = _lineNums[lineIdx].width;

                        ImGui::SetCursorPosX(renderPivot.x + breakPointFrameSize.x + (kMaxLineNumWidth - kCurrLineNumWidth) + _lineNumLeftSpacing);
                        ImGui::SetCursorPosY(renderPivot.y - 0.0f);

                        ImVec2 lineNumPos = ImGui::GetCursorScreenPos();
                
                        strBackIter = std::format_to(strBuffer.begin(), "{}", lineIdx + 1);
                        *strBackIter = '\0';

                        childDrawList->AddText(lineNumPos, IM_COL32_WHITE, strBuffer.data());
                    }

                    // Update
                    renderPivot.x = 0.0f;
                    renderPivot.y += _lineHeight;
                }

                // set margin of a break-point frame
                ImVec2 marginBreakPointFrameMin = ImVec2{ breakPointFrameRectMin.x, breakPointFrameRectMax.y };
                ImVec2 marginBreakPointFrameMax = ImVec2{ breakPointFrameRectMax.x, marginBreakPointFrameMin.y + style.ScrollbarSize };

#ifdef SHOW_BREAKPOINT_FRAME_MARGIN_RECT
                ImGui::GetForegroundDrawList()->AddRect(marginBreakPointFrameMin, marginBreakPointFrameMax, IM_COL32(255, 0, 255, 255));
#endif

                if (true == _showBreakPoints)
                {
                    parentDrawList->AddRectFilled(marginBreakPointFrameMin, marginBreakPointFrameMax, ImGui::GetColorU32(ImGuiCol_Button));
                }

                // TODO OR NOT
                // // set margin of a line-num frame
                // ImVec2 marginLineNumFrameMin = ImVec2{ marginBreakPointFrameMax.x, breakPointFrameRectMax.y };
                // ImVec2 marginLineNumFrameMax = ImVec2{ marginLineNumFrameMin.x + kMaxLineNumWidth + _lineNumLeftSpacing + _lineNumRightSpacing, marginLineNumFrameMin.y + style.ScrollbarSize };
                // 
                // // Debug Code
                // // ImGui::GetForegroundDrawList()->AddRect(marginLineNumFrameMin, marginLineNumFrameMax, IM_COL32(255, 0, 255, 255));
                // 
                // parentDrawList->AddRectFilled(marginLineNumFrameMin, marginLineNumFrameMax, ImGui::GetColorU32(ImGuiCol_ButtonHovered));

                // dummy for scrolling
                ImGui::Dummy(ImVec2{ 1.0f, 10000.0f });
            }
            ImGui::EndChild(); // End of EditorSub
        }
    }
    ImGui::EndChild(); // End of EditorFrame

    ImGui::PopFont();
}

void TextEditor::handleMainEditorInputs()
{
    _currMainTime = ImGui::GetTime();

    if (_handleEditorKeyboardInputs)
    {
        this->handleMainEditorKeyboardInputs();
    }

    if (_handleEditorMouseInputs)
    {
        this->handleMainEditorMouseInputs();
    }
}

void TextEditor::handleMainEditorKeyboardInputs()
{
    ImGuiIO& io = ImGui::GetIO();

    bool shiftPressed = io.KeyShift;
    bool ctrlPressed  = io.KeyCtrl;
    bool altPressed   = io.KeyAlt;
}

void TextEditor::handleMainEditorMouseInputs()
{
    ImGuiIO& io = ImGui::GetIO();
    
    LineCoordinate lineCoordinate = this->convertMousePosToLineCoordinate(ImGui::GetMousePos());

    bool shiftPressed = io.KeyShift;
    bool ctrlPressed  = io.KeyCtrl;
    bool altPressed   = io.KeyAlt;

    // if (isDragging())

    if (ImGui::IsWindowHovered())
    // if (ImGui::IsWindowFocused())
    {
        bool leftButtonClicked       = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        bool leftButtonDoubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        bool leftButtonTripleClicked = false;

        if (this->isMousePosInMainEditorFrame() /* && !(is in selection) */)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        }

        // triple click possibility
        if (leftButtonClicked && false == leftButtonDoubleClicked && -1.0 != _lastLeftButtonClickedTime)
        {
            if (_currMainTime - _lastLeftButtonClickedTime < io.MouseDoubleClickTime)
            {
                leftButtonTripleClicked = true;
            }
        }

        /**
         * Main Logic
         */
        if (leftButtonTripleClicked)
        {
            _cursorCoord = lineCoordinate;

            // if (false == ctrlPressed) // line selection
            // {
            // 
            // }

            _lastLeftButtonClickedTime = -1.0;

            return;
        }
        else if (leftButtonDoubleClicked)
        {
            _cursorCoord = lineCoordinate;

            // if (false == ctrlPressed) // 
            // {
            // 
            // }

            _lastLeftButtonClickedTime = _currMainTime;

            return;
        }
        else if (leftButtonClicked)
        {
            _cursorCoord = lineCoordinate;

            // if (ctrlPressed) // word selection
            // {
            // 
            // }
            // else // none selection(cursor only)
            // {
            // 
            // }

            _lastLeftButtonClickedTime = _currMainTime;

            return;
        }

        // dragging by the left button
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            _cursorCoord = lineCoordinate;

        }
    }
}

auto TextEditor::adjustCoordinate(const LineCoordinate& lineCoord) const -> LineCoordinate
{
    LineCoordinate ret;

    ret.lineNum = std::max(0, lineCoord.lineNum);
    ret.lineNum = std::min(ret.lineNum, (int32_t)_textLines.size() - 1);

    ret.column  = std::max(0, lineCoord.column);

    if (_textLines[ret.column].text.empty())
    {
        ret.column = 0;
    }
    else
    {
        ret.column = std::min(ret.column, (int32_t)_textLines[ret.lineNum].text.size() /* - 1 */);
    }

    return ret;
}

auto TextEditor::getLineCoordinateScreenStartPos() const -> ImVec2
{
    // ImVec2 screenStartPos = ImGui::GetCursorScreenPos() + _mainContentPadding + ImVec2{ _textLineLeftSpacing, 0.0f };
    ImVec2 screenStartPos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin() + _mainContentPadding + ImVec2{ _textLineLeftSpacing, 0.0f };

    return screenStartPos;
}

auto TextEditor::isValidCoordinate(const LineCoordinate& lineCoord) const -> bool
{
    // Rows
    if (lineCoord.lineNum < 0 || lineCoord.lineNum >= _textLines.size())
        return false;

    // Columns(include the bounds of text)
    if (lineCoord.column < 0 || lineCoord.column > _textLines[lineCoord.lineNum].text.size())
        return false;

    return true;
}

auto TextEditor::isMousePosInMainEditorFrame() const -> bool
{
    // ** when you are using scrolling and if you want to get the content height, you have to type the following code **
    // scrollY + ImGui::GetContentRegionMax().y
    // scrollY + ImGui::GetWindowContentRegionMax().y

    ImVec2 contentRegion = ImGui::GetContentRegionMax() + ImVec2{ ImGui::GetScrollX(), ImGui::GetScrollY() };

    ImVec2 contentRegionRectMin = ImGui::GetWindowPos();
    ImVec2 contentRegionRectMax = contentRegionRectMin + contentRegion;

    ImVec2 mousePos = ImGui::GetMousePos();

#ifdef SHOW_MAIN_EDITOR_FRAME_RECT_WHEN_HOVERED
    ImGui::GetForegroundDrawList()->AddRect(contentRegionRectMin, contentRegionRectMax, IM_COL32(255, 0, 255, 255));
#endif

    if (mousePos.x > contentRegionRectMin.x && mousePos.x < contentRegionRectMax.x &&
        mousePos.y > contentRegionRectMin.y && mousePos.y < contentRegionRectMax.y)
        return true;

    return false;
}

auto TextEditor::convertLineCoordinateToScreenPos(const LineCoordinate& lineCoord) const -> ImVec2
{
    ImVec2 screenPos = this->getLineCoordinateScreenStartPos();
    {
        screenPos.y += lineCoord.lineNum * _lineHeight;
    }

    if (lineCoord.column > 0)
    {
        screenPos.x += _textLines[lineCoord.lineNum].cumulativeCharWidths[lineCoord.column - 1];
    }

    return screenPos;
}

auto TextEditor::convertMousePosToLineCoordinate(const ImVec2& mousePos) const -> LineCoordinate
{
    ImVec2 startPos = this->getLineCoordinateScreenStartPos();
    ImVec2 distance = mousePos - startPos;

    int32_t lineNum = std::max(0, (int32_t)floor(distance.y / _lineHeight));
    int32_t column  = 0;

    lineNum = std::min(lineNum, (int32_t)(_textLines.size() - 1));

    /**
     * start to convert
     */
    const TextLine& textLine = _textLines[lineNum];

    int32_t columnIdx = 0;
    float   lastCumulativeWidth = 0.0f;
    
    // TODO : UTF8 SUPPORT
    while (columnIdx < textLine.text.size())
    {
        float charWidth = lastCumulativeWidth - textLine.cumulativeCharWidths[columnIdx - 1];

        if (distance.x < textLine.cumulativeCharWidths[columnIdx] - charWidth * 0.5f)
            break;

        lastCumulativeWidth = textLine.cumulativeCharWidths[columnIdx];

        columnIdx++;
    }

    // // reached to the last index
    // if (columnIdx == textLine.text.size() && textLine.text.size() > 1)
    // {
    //     columnIdx--;
    // }

#ifdef SHOW_START_POS_OF_LINE_COORDINATE_WHEN_HOVERED
    ImGui::GetForegroundDrawList()->AddCircle(startPos, 5, IM_COL32(255, 0, 255, 255));
#endif

    std::cout << distance.x << ' ' << distance.y << " | " << lineNum << ' ' << columnIdx << '\n';

    return LineCoordinate{ lineNum, columnIdx };
}

void TextEditor::calcAllTextLineSizes()
{
    for (int32_t lineIdx = 0; lineIdx < _textLines.size(); lineIdx++)
    {
        this->calcTextLineSize(lineIdx);
    }
}

void TextEditor::calcTextLineSize(int32_t lineIdx)
{
    _textBuffer.clear();
    _textLines[lineIdx].cumulativeCharWidths.clear();
    //_textLines[lineIdx].charXPivots.clear();

    ImVec2 lastSize = _currFont->CalcTextSizeA(_currFont->FontSize, FLT_MAX, 0.0f, "");

    // TODO : UTF8 SUPPORT
    for (auto& charInfo : _textLines[lineIdx].text)
    {
        _textBuffer.push_back(charInfo.ch);

        lastSize = _currFont->CalcTextSizeA(_currFont->FontSize, FLT_MAX, 0.0f, _textBuffer.c_str());

        _textLines[lineIdx].cumulativeCharWidths.push_back(lastSize.x);
        //_textLines[lineIdx].charXPivots.push_back(lastSize.x);
    }

    _textLines[lineIdx].width = lastSize.x;
}

void TextEditor::updateAdditionalLineNums()
{
    for (int32_t idx = 0; idx < _deferredUpdate_additionalLineNumIndices.size(); idx++)
    {
        int32_t lineNum = _deferredUpdate_additionalLineNumIndices[idx];

        _lineNums.emplace_back();

        _lineNums.back().numStr = std::to_string(lineNum);

        this->calcLineNumSize(lineNum - 1); // idx = num - 1
    }

    _deferredUpdate_additionalLineNumIndices.clear();
}

void TextEditor::calcAllLineNumSizes()
{
    for (int32_t lineIdx = 0; lineIdx < _textLines.size(); lineIdx++)
    {
        this->calcLineNumSize(lineIdx);
    }
}

void TextEditor::calcLineNumSize(int32_t lineIdx)
{
    _lineNums[lineIdx].width = _currFont->CalcTextSizeA(_currFont->FontSize, FLT_MAX, 0.0f, _lineNums[lineIdx].numStr.c_str()).x;
}

auto TextEditor::getMaxTextLineWidth() const -> float
{
    float maxTextLineWidth = 0;

    for (int32_t lineIdx = 0; lineIdx < _textLines.size(); lineIdx++)
    {
        maxTextLineWidth = std::max(maxTextLineWidth, _textLines[lineIdx].width);
    }

    return maxTextLineWidth;
}

auto TextEditor::getMainContentRegionFullSize() const -> ImVec2
{
    ImVec2 contentRegionFullSize;

    float maxTextLineWidth = this->getMaxTextLineWidth();

    contentRegionFullSize.x = maxTextLineWidth + (_mainContentPadding.x * 2.0f) + _textLineLeftSpacing + _textLineRightSpacing;
    contentRegionFullSize.y = _textLines.size() * _lineHeight + (_mainContentPadding.y * 2.0f);

    return contentRegionFullSize;
}

auto TextEditor::getMaxLineNumWidth() const -> float
{
    std::string_view maxLineNumStr = _lineNums.back().numStr;

    float maxLineNumWidth = _currFont->CalcTextSizeA(_currFont->FontSize, FLT_MAX, 0.0f, maxLineNumStr.data()).x;

    return maxLineNumWidth;
}

void TextEditor::SetText(const std::string_view& text)
{
    _textLines.clear();
    _textLines.emplace_back();

    // TODO : UTF8 SUPPORT
    for (auto ch : text)
    {
        // if ('r' == ch)
        // {
        //     // ignore the carriage return character
        //     continue;
        // }
        
        if ('\n' == ch)
        {
            _textLines.emplace_back();
        }
        else if ('\t' == ch) // don't use tab symbols instead use 4 spaces // TODO : mixing spaces and tabs
        {
            // UTF8 WARNING
            int32_t maxCnt = _kTabSize - _textLines.back().text.size() % _kTabSize;

            for (int32_t cnt = 0; cnt < maxCnt; cnt++)
            {
                _textLines.back().text.push_back(CharInfo{ ' ' });
            }
        }
        else // characters
        {
            _textLines.back().text.push_back(CharInfo{ ch });
        }   
    }

    if (_lineNums.size() < _textLines.size())
    {
        for (int32_t idx = (int32_t)_lineNums.size(); idx < _textLines.size(); idx++)
        {
            _deferredUpdate_additionalLineNumIndices.push_back(idx + 1);
        }
    }

    _deferredUpdate_calcAllTextLineSizes = true;
}
