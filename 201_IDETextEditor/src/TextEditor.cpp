#include "TextEditor.h"

#include <format>

#define RENDER_TEST 1

#if RENDER_TEST == 1

void TextEditor::Render(bool* open)
{
    if (nullptr != open && false == *open)
        return;

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

    if (_renderHeaderCallback)
    {
        ImGui::BeginChild(std::format("{}##Header", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, kChildFlags, kWindowFlags);
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
    constexpr ImGuiChildFlags  kBodyChildFlags  = ImGuiChildFlags_Border;
    constexpr ImGuiWindowFlags kBodyWindowFlags = ImGuiWindowFlags_None;
    
    constexpr ImGuiChildFlags  kSubChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kSubWindowFlags = ImGuiWindowFlags_NoScrollbar;

    constexpr ImGuiChildFlags  kMainChildFlags  = ImGuiChildFlags_None;
    constexpr ImGuiWindowFlags kMainWindowFlags = ImGuiWindowFlags_AlwaysHorizontalScrollbar;

    ImGuiStyle& style          = ImGui::GetStyle();
    ImDrawList* parentDrawList = ImGui::GetWindowDrawList();

    auto kBorderThicknessSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
    auto kEditorBodySize      = ImGui::GetContentRegionAvail() - ImVec2{ 0.0f, _footerSpacingY + kBorderThicknessSize.y };

    ImGui::BeginChild(std::format("{}##EditorBody", _windowTitle).c_str(), kEditorBodySize, kBodyChildFlags, kBodyWindowFlags);
    {
        /**
         * Variables for Sizing
         */
        const float kFontHeight = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#").y;
        
        _lineHeight = kFontHeight + _itemSpacingY;

        const ImVec2 kContentsSize = kEditorBodySize - kBorderThicknessSize * 2;

        float scrollX = 0.0f;
        float scrollY = 0.0f;

        float subWindowWidth = 0.0f;

        /**
         * Variables for Break-Point Area
         */
        auto breakPointAreaSize = ImVec2{ std::round(kFontHeight * 1.15f), ImGui::GetContentRegionAvail().y - (kBorderThicknessSize.y * 2.0f) - style.ScrollbarSize };
            
        ImVec2 breakPointAreaRectMin = ImGui::GetWindowPos() + kBorderThicknessSize;
        ImVec2 breakPointAreaRectMax = breakPointAreaRectMin + breakPointAreaSize;
            
        ImVec2 breakPointButtonSize = ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x + 0.0f };

        /**
         * Variable for Line-No Area
         */
        const int32_t kTotalLineNo = (int32_t)_textLines.size();
        int32_t minLineNo = 0;
        int32_t maxLineNo = 0;

        std::string maxLineNoWidthStr = std::format("{}", kTotalLineNo);
        const float kMaxLineNoWidth   = ImGui::GetFont()->CalcTextSizeA(kFontHeight, FLT_MAX, 0.0f, maxLineNoWidthStr.c_str()).x;

        // TODO
        // if breakpoints enabled
        {
            subWindowWidth += breakPointButtonSize.x;
        }

        // TODO
        // if lineNo enabled
        {
            subWindowWidth += kMaxLineNoWidth + _lineNoLeftSpacing + _lineNoRightSpacing;
        }

        /**
         * Variables for Window Positions
         */
        const ImVec2 kSubWindowPos  = ImGui::GetWindowPos() + kBorderThicknessSize;
        const ImVec2 kSubWindowSize = ImVec2{ subWindowWidth, kContentsSize.y - style.ScrollbarSize };

        const ImVec2 kMainWindowPos  = kSubWindowPos + ImVec2{ subWindowWidth, 0.0f };
        const ImVec2 kMainWindowSize = kContentsSize - ImVec2{ kSubWindowSize.x, 0.0f };

        /**
         * Editor Main Window (Text Line)
         */
        ImGui::SetNextWindowPos(kMainWindowPos);
        
        ImGui::BeginChild(std::format("{}##EditorMain", _windowTitle).c_str(), kMainWindowSize, kMainChildFlags, kMainWindowFlags);
        {
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();
            
            ImVec2 renderPivot = _editorPadding;
            
            float maxTextWidth = 0.0f;
            
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
                    auto sp = ImVec2{ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
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

                std::string str;

                for (auto& charInfo : _textLines[lineIdx])
                {
                    str.push_back(charInfo.ch);
                }

                maxTextWidth = std::max(maxTextWidth, ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, str.c_str()).x);

                childDrawList->AddText(ImGui::GetCursorScreenPos(), IM_COL32_WHITE, str.c_str());

                // Update
                renderPivot.x = _editorPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

            // calculate an area for scrolling
            float calcAreaWidth  = maxTextWidth + (_editorPadding.x * 2.0f) + _textLineLeftSpacing + _textLineRightSpacing;
            float calcAreaHeight = _textLines.size() * (kFontHeight + _itemSpacingY) + (_editorPadding.y * 2.0f);

            if (_textLines.size() > 0)
            {
                calcAreaHeight -= _itemSpacingY;
            }

            ImGui::SetCursorPosX(calcAreaWidth);
            ImGui::SetCursorPosY(calcAreaHeight);
        }
        ImGui::EndChild();
        
        ImGui::SameLine();

        /**
         * Editor Sub Window (Breakpoint, Line No)
         */
        ImGui::SetNextWindowPos(kSubWindowPos);
        ImGui::SetNextWindowScroll(ImVec2{ 0.0f, scrollY }); // this is why we have to render the sub window here.

        ImGui::BeginChild(std::format("{}##EditorSub", _windowTitle).c_str(), kSubWindowSize, kSubChildFlags, kSubWindowFlags);
        {
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();

            ImVec2 renderPivot = _editorPadding;

            // Debug Code (Area Size)
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointAreaRectMin, 2.0f, IM_COL32_WHITE);
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointAreaRectMax, 2.0f, IM_COL32_WHITE);

            childDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));
            
            for (int32_t lineIdx = 0; lineIdx < kTotalLineNo; lineIdx++)
            {
                /**
                 * Break-Point
                 */
                ImGui::SetCursorPosX(renderPivot.x);
                ImGui::SetCursorPosY(renderPivot.y - 2.0f); // give -2.0f for a visual beauty.

                // Debug Code (Break-Point Rect)
                // ImGui::GetForegroundDrawList()->AddRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x }, IM_COL32_WHITE);
                // ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);

                // Break-Point Pos
                ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                {
                    breakPointPos.x += breakPointAreaSize.x * 0.5f;
                    breakPointPos.y += breakPointAreaSize.x * 0.5f;
                }

                //
                auto breakPointIter = _breakPoints.find(lineIdx);

                // Break-Point Button
                // if (ImGui::Button(std::format("##{}", lineIdx).c_str(), breakPointButtonSize))
                if (ImGui::InvisibleButton(std::format("BreakPointBtn##{}", lineIdx).c_str(), breakPointButtonSize))
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
                    childDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
                }

                if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
                {
                    childDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
                }

                /**
                 * Line No
                 */
                std::string lineNoStr = std::format("{}", lineIdx + 1);

                const float kCurrLineNoWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, lineNoStr.c_str()).x;

                ImGui::SetCursorPosX(renderPivot.x + breakPointAreaSize.x + (kMaxLineNoWidth - kCurrLineNoWidth) + _lineNoLeftSpacing);
                ImGui::SetCursorPosY(renderPivot.y - 1.0f);

                ImVec2 lineNoPos = ImGui::GetCursorScreenPos();
                
                childDrawList->AddText(lineNoPos, IM_COL32_WHITE, lineNoStr.c_str());

                // Update
                renderPivot.x = _editorPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

            // set margin of a break-point area
            ImVec2 marginBreakPointAreaMin = ImVec2{ breakPointAreaRectMin.x, breakPointAreaRectMax.y };
            ImVec2 marginBreakPointAreaMax = ImVec2{ breakPointAreaRectMax.x, marginBreakPointAreaMin.y + style.ScrollbarSize };

            // Debug Code
            // ImGui::GetForegroundDrawList()->AddRect(marginBreakPointAreaMin, marginBreakPointAreaMax, IM_COL32(255, 0, 255, 255));

            parentDrawList->AddRectFilled(marginBreakPointAreaMin, marginBreakPointAreaMax, ImGui::GetColorU32(ImGuiCol_Button));

            // TODO OR NOT
            // // set margin of a line-no area
            // ImVec2 marginLineNoAreaMin = ImVec2{ breakPointAreaRectMin.x, breakPointAreaRectMax.y };
            // ImVec2 marginLineNoAreaMax = ImVec2{ breakPointAreaRectMax.x, marginBreakPointAreaMin.y + style.ScrollbarSize };
            // 
            // // Debug Code
            // // ImGui::GetForegroundDrawList()->AddRect(marginBreakPointAreaMin, marginBreakPointAreaMax, IM_COL32(255, 0, 255, 255));
            // 
            // parentDrawList->AddRectFilled(marginBreakPointAreaMin, marginBreakPointAreaMax, ImGui::GetColorU32(ImGuiCol_Button));

            // dummy for scrolling
            ImGui::Dummy(ImVec2{ 1.0f, 1000.0f });
        }
        ImGui::EndChild();
    }
    ImGui::EndChild(); 
}

void TextEditor::renderFooter()
{
    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    _footerSpacingY = 0.0f;

    if (_renderFooterCallback)
    {
        ImGui::BeginChild(std::format("{}##Footer", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, kChildFlags, kWindowFlags);
        {
            _renderFooterCallback(*this);

            _footerSpacingY = ImGui::GetWindowSize().y + ImGui::GetStyle().ItemSpacing.y;
        }
        ImGui::EndChild();
    }
}

void TextEditor::SetText(const std::string_view& text)
{
    _textLines.clear();
    _textLines.emplace_back();

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
        else // characters
        {
            _textLines.back().push_back(CharInfo{ ch });
        }
    }
}
