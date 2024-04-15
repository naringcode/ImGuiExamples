#include "TextEditor.h"

#include <format>

#define RENDER_TEST 1

#if RENDER_TEST == 1

constexpr int32_t g_kShortBufferLen = 256;

void TextEditor::Render(bool* open)
{
    if (nullptr != open && false == *open)
        return;

    this->updateBeforeRender();

    // Old : PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });

    this->beginStyle();

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 8.0f });
    this->pushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_ResizeGrip, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        // Old : Debug Code for Window Padding Test
        // ImGui::GetWindowDrawList()->AddCircleFilled(ImGui::GetCursorScreenPos(), 24.0f, IM_COL32_WHITE);

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

            this->renderHeader();
        }
        this->endStyle();

        /**
         * Editor
         */
        this->beginStyle();
        {
            this->renderEditor();
        }
        this->endStyle();

        ImGui::Spacing(); // without calling Spacing(), the following footer adjoins the editor closely.

        /**
         * Footer
         */
        this->beginStyle();
        {
            // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });

            this->renderFooter();
        }
        this->endStyle();
    }
    ImGui::End(); // Main Window

    this->endStyle();

    this->updateAfterRender();
}

#elif RENDER_TEST == 2

void TextEditor::Render(bool* open)
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::Begin("TestWindow");

    auto aa = ImGui::GetWindowPos();

    ImGui::BeginChild("TestWindow##1", ImVec2{ 400.0f, 200.0f });
    {
        ImGui::BeginChild("TestWindow##11", ImVec2{ 300.0f, 100.0f }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            for (int i = 0; i < 20; i++)
            {
                ImGui::Text("111");
            }
        }
        ImGui::EndChild();

        ImGui::BeginChild("TestWindow##12", ImVec2{ 400.0f, 100.0f });
        {
            ImGui::Text("222");
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    auto bb = ImGui::GetWindowPos();

    ImGui::BeginChild("TestWindow##2", ImVec2{ 200.0f, 200.0f });
    {
        ImGui::GetForegroundDrawList()->AddLine(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ 300.0f, 0.0f }, IM_COL32_WHITE);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 0.0f });

        auto tempPosY = ImGui::GetCursorPosY();

        ImGui::Text("TextText"); ImGui::SameLine();

        ImGui::Button("Test", ImVec2{ 100.0f, 100.0f });

        ImGui::SetCursorPosY(tempPosY); // A

        ImGui::SameLine();

        ImGui::SetCursorPosY(tempPosY); // B

        ImGui::Text("TextText");

        ImGui::Text("TextText");

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::End();

    auto cc = ImGui::GetWindowPos();
}

#endif

void TextEditor::renderMenu()
{
    if (_renderMenuCallback && 0 != (_windowFlags & ImGuiWindowFlags_MenuBar))
    {
        _renderMenuCallback(*this);
    }
}

void TextEditor::renderHeader()
{
    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    std::array<char, g_kShortBufferLen> strBuffer;

    if (_renderHeaderCallback)
    {
        auto backIter = std::format_to(strBuffer.begin(), "{}##Header", _windowTitle);
        *backIter = '\0';

        ImGui::BeginChild(strBuffer.data(), ImVec2{0.0f, 0.0f}, kChildFlags, kWindowFlags);
        {
            _renderHeaderCallback(*this);
        }
        ImGui::EndChild();
    }
}

void TextEditor::renderEditor()
{
    /**
     * Styles
     */
    this->pushStyleColor(ImGuiCol_ChildBg,     ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_ScrollbarBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_Border,      ImGui::ColorConvertU32ToFloat4(IM_COL32(62, 62, 62, 255)));
    
    // Old : give widths from zero to one by testing, and we handle positions programmatically.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 1.0f, 5.0f });
    // this->pushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2{ 0.0f, 3.0f });

    // we handle positions programmatically.
    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    this->pushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2{ 0.0f, 0.0f });

    this->pushStyleVar(ImGuiStyleVar_ScrollbarSize,   14.0f);
    this->pushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    /**
     * Core Variables
     */
    constexpr ImGuiChildFlags  kFrameChildFlags  = ImGuiChildFlags_Border;
    constexpr ImGuiWindowFlags kFrameWindowFlags = ImGuiWindowFlags_None;
    
    constexpr ImGuiChildFlags  kSubChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kSubWindowFlags = ImGuiWindowFlags_NoScrollbar;

    constexpr ImGuiChildFlags  kMainChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kMainWindowFlags = ImGuiWindowFlags_AlwaysHorizontalScrollbar;

    ImGuiStyle& style          = ImGui::GetStyle();
    ImDrawList* parentDrawList = ImGui::GetWindowDrawList();

    auto kFrameBorderSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
    auto kFrameSize       = ImGui::GetContentRegionAvail() - ImVec2{ 0.0f, _footerSpacingY + kFrameBorderSize.y };

    std::array<char, g_kShortBufferLen> strBuffer;

    auto strBackIter = std::format_to(strBuffer.begin(), "{}##EditorFrame", _windowTitle);
    *strBackIter = '\0';

    ImGui::BeginChild(strBuffer.data(), kFrameSize, kFrameChildFlags, kFrameWindowFlags);
    {
        /**
         * Variables for Sizing
         */
        const float kFontHeight = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#").y;
        
        _lineHeight = kFontHeight + _itemSpacingY;

        const ImVec2 kContentSize = kFrameSize - kFrameBorderSize * 2;

        float scrollX = 0.0f;
        float scrollY = 0.0f;

        float subContentWidth = 0.0f;

        /**
         * Variables for Break-Point Frame
         */
        auto breakPointFrameSize = ImVec2{ std::round(kFontHeight * 1.15f), ImGui::GetContentRegionAvail().y - (kFrameBorderSize.y * 2.0f) - style.ScrollbarSize };
            
        ImVec2 breakPointFrameRectMin = ImGui::GetWindowPos() + kFrameBorderSize;
        ImVec2 breakPointFrameRectMax = breakPointFrameRectMin + breakPointFrameSize;
            
        ImVec2 breakPointButtonSize = ImVec2{ breakPointFrameSize.x, breakPointFrameSize.x + 0.0f };

        /**
         * Variable for Line-No Frame
         */
        const int32_t kTotalLineNo = (int32_t)_textLines.size();
        int32_t minLineNo = 0;
        int32_t maxLineNo = 0;

        strBackIter = std::format_to(strBuffer.begin(), "{}", kTotalLineNo);
        *strBackIter = '\0';

        const float kMaxLineNoWidth = this->getMaxLineNoWidth();

        // TODO
        // if breakpoints enabled
        {
            subContentWidth += breakPointButtonSize.x;
        }

        // TODO
        // if lineNo enabled
        {
            subContentWidth += kMaxLineNoWidth + _lineNoLeftSpacing + _lineNoRightSpacing;
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
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();
            
            ImVec2 renderPivot = _mainContentPadding;
            
            scrollX = ImGui::GetScrollX();
            scrollY = ImGui::GetScrollY();

            for (int32_t lineIdx = 0; lineIdx < kTotalLineNo; lineIdx++)
            {
                ImGui::SetCursorPosX(renderPivot.x);
                ImGui::SetCursorPosY(renderPivot.y);

                // Debug Code (Text Line Start Pos)
                // ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));

                // Line Mark
                {
                    auto sp = ImVec2{ ImGui::GetWindowPos().x, ImGui::GetCursorScreenPos().y };
                    auto ep = ImVec2{ sp.x, sp.y + kFontHeight };

                    childDrawList->AddLine(sp, ep, IM_COL32(165, 165, 165, 255));
                }

                /**
                 * Text Line
                 */
                ImGui::SetCursorPosX(renderPivot.x + _textLineLeftSpacing);
                ImGui::SetCursorPosY(renderPivot.y);

                // Debug Code (Text Line Pos)
                // ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));

                _textBuffer.clear();

                for (auto& charInfo : _textLines[lineIdx].text)
                {
                    _textBuffer.push_back(charInfo.ch);
                }

                childDrawList->AddText(ImGui::GetCursorScreenPos(), IM_COL32_WHITE, _textBuffer.c_str());

                // Update
                renderPivot.x = _mainContentPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

            // get the full size of the main editor for scrolling
            ImVec2 editorContentRegion = this->getMainContentRegionFullSize();
            {
                editorContentRegion.y += kContentSize.y - style.ScrollbarSize - _lineHeight - _mainContentPadding.y;
            }

            ImGui::SetCursorPos(editorContentRegion);
        }
        ImGui::EndChild();
        
        ImGui::SameLine();

        /**
         * Sub Content (Breakpoint, Line No)
         */
        ImGui::SetNextWindowPos(kSubContentPos);
        ImGui::SetNextWindowScroll(ImVec2{ 0.0f, scrollY }); // ** scrollY ** this is why we have to render the sub window here.

        strBackIter = std::format_to(strBuffer.begin(), "{}##EditorSub", _windowTitle);
        *strBackIter = '\0';

        ImGui::BeginChild(strBuffer.data(), kSubContentSize, kSubChildFlags, kSubWindowFlags);
        {
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();

            ImVec2 renderPivot = _mainContentPadding;

            // Debug Code (Frame Size)
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointFrameRectMin, 2.0f, IM_COL32_WHITE);
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointFrameRectMax, 2.0f, IM_COL32_WHITE);

            childDrawList->AddRectFilled(breakPointFrameRectMin, breakPointFrameRectMax, IM_COL32(51, 51, 51, 255));
            
            for (int32_t lineIdx = 0; lineIdx < kTotalLineNo; lineIdx++)
            {
                /**
                 * Break-Point
                 */
                ImGui::SetCursorPosX(renderPivot.x);
                ImGui::SetCursorPosY(renderPivot.y - 2.0f); // give -2.0f for a visual beauty.

                // Debug Code (Break-Point Rect)
                // ImGui::GetForegroundDrawList()->AddRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointFrameSize.x, breakPointFrameSize.x }, IM_COL32_WHITE);
                // ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);

                // Break-Point Pos
                ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                {
                    breakPointPos.x += breakPointFrameSize.x * 0.5f;
                    breakPointPos.y += breakPointFrameSize.x * 0.5f;
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
                    childDrawList->AddCircleFilled(breakPointPos, breakPointFrameSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
                }

                if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
                {
                    childDrawList->AddCircleFilled(breakPointPos, breakPointFrameSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
                }

                /**
                 * Line No
                 */
                const float kCurrLineNoWidth = _lineNoSizes[lineIdx].x;

                ImGui::SetCursorPosX(renderPivot.x + breakPointFrameSize.x + (kMaxLineNoWidth - kCurrLineNoWidth) + _lineNoLeftSpacing);
                ImGui::SetCursorPosY(renderPivot.y - 1.0f);

                ImVec2 lineNoPos = ImGui::GetCursorScreenPos();
                
                strBackIter = std::format_to(strBuffer.begin(), "{}", lineIdx + 1);
                *strBackIter = '\0';

                childDrawList->AddText(lineNoPos, IM_COL32_WHITE, strBuffer.data());

                // Update
                renderPivot.x = _mainContentPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

            // set margin of a break-point frame
            ImVec2 marginBreakPointFrameMin = ImVec2{ breakPointFrameRectMin.x, breakPointFrameRectMax.y };
            ImVec2 marginBreakPointFrameMax = ImVec2{ breakPointFrameRectMax.x, marginBreakPointFrameMin.y + style.ScrollbarSize };

            // Debug Code
            // ImGui::GetForegroundDrawList()->AddRect(marginBreakPointFrameMin, marginBreakPointFrameMax, IM_COL32(255, 0, 255, 255));

            parentDrawList->AddRectFilled(marginBreakPointFrameMin, marginBreakPointFrameMax, ImGui::GetColorU32(ImGuiCol_Button));

            // TODO OR NOT
            // // set margin of a line-no frame
            // ImVec2 marginLineNoFrameMin = ImVec2{ breakPointFrameRectMin.x, breakPointFrameRectMax.y };
            // ImVec2 marginLineNoFrameMax = ImVec2{ breakPointFrameRectMax.x, marginBreakPointFrameMin.y + style.ScrollbarSize };
            // 
            // // Debug Code
            // // ImGui::GetForegroundDrawList()->AddRect(marginBreakPointFrameMin, marginBreakPointFrameMax, IM_COL32(255, 0, 255, 255));
            // 
            // parentDrawList->AddRectFilled(marginBreakPointFrameMin, marginBreakPointFrameMax, ImGui::GetColorU32(ImGuiCol_Button));

            // dummy for scrolling
            ImGui::Dummy(ImVec2{ 1.0f, 10000.0f });
        }
        ImGui::EndChild();
    }
    ImGui::EndChild(); 
}

void TextEditor::renderFooter()
{
    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    std::array<char, g_kShortBufferLen> strBuffer;

    _footerSpacingY = 0.0f;

    if (_renderFooterCallback)
    {
        auto backIter = std::format_to(strBuffer.begin(), "{}##Footer", _windowTitle);
        *backIter = '\0';

        ImGui::BeginChild(strBuffer.data(), ImVec2{ 0.0f, 0.0f }, kChildFlags, kWindowFlags);
        {
            _renderFooterCallback(*this);

            _footerSpacingY = ImGui::GetWindowSize().y + ImGui::GetStyle().ItemSpacing.y;
        }
        ImGui::EndChild();
    }
}

void TextEditor::updateBeforeRender()
{
    if (true == _deferredUpdate_calcAllTextLineSizes)
    {
        _deferredUpdate_calcAllTextLineSizes = false;

        this->calcAllTextLineSizes();
    }

    if (true == _deferredUpdate_calcAllLineNoSizes)
    {
        _deferredUpdate_calcAllLineNoSizes = false;

        this->calcAllLineNoSizes();
    }
}

void TextEditor::updateAfterRender()
{

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

    for (auto& charInfo : _textLines[lineIdx].text)
    {
        _textBuffer.push_back(charInfo.ch);
    }

    _textLines[lineIdx].lineSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, _textBuffer.c_str());
}

void TextEditor::calcAllLineNoSizes()
{
    for (int32_t lineIdx = 0; lineIdx < _textLines.size(); lineIdx++)
    {
        this->calcLineNoSize(lineIdx);
    }
}

void TextEditor::calcLineNoSize(int32_t lineIdx)
{
    std::array<char, g_kShortBufferLen> strBuffer;

    auto strBackIter = std::format_to(strBuffer.begin(), "{}", lineIdx + 1);
    *strBackIter = '\0';

    _lineNoSizes[lineIdx] = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, strBuffer.data());
}

float TextEditor::getMaxTextLineWidth() const
{
    float maxTextLineWidth = 0;

    for (int32_t lineIdx = 0; lineIdx < _textLines.size(); lineIdx++)
    {
        maxTextLineWidth = std::max(maxTextLineWidth, _textLines[lineIdx].lineSize.x);
    }

    return maxTextLineWidth;
}

ImVec2 TextEditor::getMainContentRegionFullSize() const
{
    ImVec2 contentRegionFullSize;

    float maxTextLineWidth = this->getMaxTextLineWidth();

    contentRegionFullSize.x = maxTextLineWidth + (_mainContentPadding.x * 2.0f) + _textLineLeftSpacing + _textLineRightSpacing;
    contentRegionFullSize.y = _textLines.size() * _lineHeight + (_mainContentPadding.y * 2.0f);

    if (_textLines.size() > 0)
    {
        contentRegionFullSize.y -= _itemSpacingY;
    }

    return contentRegionFullSize;
}

float TextEditor::getMaxLineNoWidth() const
{
    std::array<char, g_kShortBufferLen> strBuffer;

    auto strBackIter = std::format_to(strBuffer.begin(), "{}", _textLines.size());
    *strBackIter = '\0';

    float maxLineNoWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, strBuffer.data()).x;

    return maxLineNoWidth;
}

void TextEditor::SetText(const std::string_view& text)
{
    _textLines.clear();
    _textLines.emplace_back();

    _lineNoSizes.clear();
    _lineNoSizes.emplace_back();

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
            _lineNoSizes.emplace_back();
        }
        else // characters
        {
            _textLines.back().text.push_back(CharInfo{ ch });
        }   
    }

    _deferredUpdate_calcAllTextLineSizes = true;
    _deferredUpdate_calcAllLineNoSizes = true;
}
