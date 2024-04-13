#include "TextEditor.h"

#include <format>

#define RENDER_TEST 1

#if RENDER_TEST == 1

void TextEditor::Render(bool* open)
{
    if (nullptr != open && false == *open)
        return;

    // I don't know why but PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });

    this->beginStyle();

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 8.0f });
    this->pushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_ResizeGrip, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        // Debug Code for Window Padding Bug(?)
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
    if (nullptr != open && false == *open)
        return;

    ImGuiStyle& style = ImGui::GetStyle();

    // I don't know why but PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });

    this->beginStyle();

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 8.0f, 8.0f });
    this->pushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_ResizeGrip, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        ImDrawList* mainDrawList = ImGui::GetWindowDrawList();

        // Debug Code for Window Padding Bug(?)
        // mainDrawList->AddCircleFilled(ImGui::GetCursorScreenPos(), 24.0f, IM_COL32_WHITE);

        /**
         * Menu
         */
        if (_renderMenuCallback && 0 != (_windowFlags & ImGuiWindowFlags_MenuBar))
        {
            _renderMenuCallback(*this);
        }
        
        /**
         * Header
         */
        if (_renderHeaderCallback)
        {
            this->beginStyle();

            //this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });

            ImGui::BeginChild(std::format("{}##Header", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
            {
                _renderHeaderCallback(*this);
            }
            ImGui::EndChild();

            this->endStyle();
        }

        /**
         * Editor Part
         */
        auto editorBorderThicknessSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
        auto editorSize = ImVec2{ 0.0f, 0.0f };

        if (_renderFooterCallback)
        {
            editorSize.y = ImGui::GetContentRegionAvail().y - style.ItemSpacing.y - _footerHeight - style.ChildBorderSize;
        }

        this->beginStyle();

        this->pushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
        this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 4.0f }); // 3.0f or 4.0f
        this->pushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 3.0f }); // 3.0f or 4.0f

        ImGui::BeginChild(std::format("{}##Editor", _windowTitle).c_str(), editorSize, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImDrawList* editorDrawList = ImGui::GetWindowDrawList();

            /**
             * Sizes and Positions
             */
            const ImVec2 kFontSize    = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#");
            const float  kMaxFontSize = std::max(kFontSize.x, kFontSize.y);

            auto breakPointAreaSize = ImVec2{ kMaxFontSize, ImGui::GetContentRegionAvail().y };
            {
                // kBreakPointAreaSize
            }

            /**
             * Breakpoitns Area Test
             */
            ImVec2 breakPointAreaRectMin = ImGui::GetCursorScreenPos() + editorBorderThicknessSize;
            ImVec2 breakPointAreaRectMax = breakPointAreaRectMin;
            {
                breakPointAreaRectMin.x += ImGui::GetScrollX() - style.WindowPadding.x;
                breakPointAreaRectMin.y += ImGui::GetScrollY() - style.WindowPadding.y;

                // breakPointAreaRectMax += breakPointAreaSize;

                breakPointAreaRectMax.x = breakPointAreaRectMin.x + kMaxFontSize;
                breakPointAreaRectMax.y = breakPointAreaRectMin.y + ImGui::GetContentRegionAvail().y;
                 
                // -2.0f means borders
                breakPointAreaRectMax.x += style.WindowPadding.x * 2.0f - 2.0f;
                breakPointAreaRectMax.y += style.WindowPadding.y * 2.0f - 2.0f;
            }

            // Debug Code
            // mainDrawList->AddCircleFilled(breakPointAreaRectMin, 2.0f, IM_COL32_WHITE);
            // mainDrawList->AddCircleFilled(breakPointAreaRectMax, 2.0f, IM_COL32_WHITE);

            mainDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));
            // ImGui::GetForegroundDrawList()->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));

            for (auto& textLine : _textLines)
            {
                // Breakpoints Test
                ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                {
                    breakPointPos.x += kMaxFontSize * 0.5f + ImGui::GetScrollX();
                    breakPointPos.y += kMaxFontSize * 0.5f - 1.0f; // + ImGui::GetScrollY();
                }

                editorDrawList->AddCircleFilled(breakPointPos, kMaxFontSize * 0.45f, IM_COL32(197, 81, 89, 255));

                // Debug Code for Text Line
                {
                    ImVec2 sp = { ImGui::GetCursorScreenPos().x + kMaxFontSize + style.WindowPadding.x * 2.0f, ImGui::GetCursorScreenPos().y };
                    ImVec2 ep = { sp.x, sp.y + kFontSize.y };

                    editorDrawList->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
                }
                
                // Text Line Test
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + kMaxFontSize + style.WindowPadding.x * 2.0f + 8.0f);


                for (auto& charInfo : textLine)
                {
                    ImGui::Text("%c", charInfo.ch);

                    ImGui::SameLine(0.0f, 0.0f);
                }

                ImGui::Text("\n");
            }
        }
        ImGui::EndChild();

        this->endStyle();

        /**
         * Footer
         */
        if (_renderFooterCallback)
        {
            this->beginStyle();

            //this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 2.0f });

            ImGui::BeginChild(std::format("{}##Footer", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
            {
                _renderFooterCallback(*this);

                _footerHeight = ImGui::GetWindowSize().y;
            }
            ImGui::EndChild();

            this->endStyle();
        }

    }
    ImGui::End();

    this->endStyle();
}

#elif RENDER_TEST == 3

void TextEditor::Render(bool* open)
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::Begin("TestWindow");
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

        ImGui::PopStyleVar();
    }
    ImGui::End();
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
    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Border;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    
    ImGuiStyle& style        = ImGui::GetStyle();
    ImDrawList* mainDrawList = ImGui::GetWindowDrawList();

    auto borderThicknessSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
    auto bodySize = ImVec2{ 0.0f, 0.0f };

    if (_renderFooterCallback)
    {
        bodySize.y = ImGui::GetContentRegionAvail().y - style.ItemSpacing.y - _footerHeight - borderThicknessSize.y;
    }

    this->pushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_Border,  ImGui::ColorConvertU32ToFloat4(IM_COL32(62, 62, 62, 255)));
    
    // give widths from zero to one by testing, and we handle positions programmatically.
    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 1.0f, 5.0f });
    // this->pushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2{ 0.0f, 0.0f });
    this->pushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2{ 0.0f, 3.0f }); // 3.0f

    ImGui::BeginChild(std::format("{}##Editor", _windowTitle).c_str(), bodySize, kChildFlags, kWindowFlags);
    {
        ImDrawList* editorDrawList = ImGui::GetWindowDrawList();
        
        /**
         * Sizes and Positions
         */
        const ImVec2 kFontSize    = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#");
        const float  kMaxFontSize = std::max(kFontSize.x, kFontSize.y);

        // (style.WindowPadding.y - borderThicknessSize.y) * 2.0f == style.WindowPadding.y * 2.0f - borderThicknessSize.y * 2.0f;
        auto breakPointAreaSize = ImVec2{ kMaxFontSize * 1.15f, ImGui::GetContentRegionAvail().y + (style.WindowPadding.y - borderThicknessSize.y) * 2.0f };

        ImVec2 breakPointAreaRectMin = ImGui::GetCursorScreenPos() - style.WindowPadding + ImVec2{ style.ChildBorderSize, style.ChildBorderSize } + ImVec2{ ImGui::GetScrollX(), ImGui::GetScrollY() };
        ImVec2 breakPointAreaRectMax = breakPointAreaRectMin + breakPointAreaSize;

        ImVec2 breakPointButtonSize = ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x + 0.0f };

        constexpr float kTextSpacingAtBeginning = 8.0f;

        // Debug Code
        // mainDrawList->AddCircleFilled(breakPointAreaRectMin, 2.0f, IM_COL32_WHITE);
        // mainDrawList->AddCircleFilled(breakPointAreaRectMax, 2.0f, IM_COL32_WHITE);

        mainDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));

        float renderPivotX = style.WindowPadding.x;
        float renderPivotY = style.WindowPadding.y;

        for (int32_t idx = 0; idx < _textLines.size(); idx++)
        {
            // Debug Code
            // mainDrawList->AddRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x }, IM_COL32_WHITE);
            // mainDrawList->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);

            /**
             * Break-Point
             */
            auto breakPointIter = _breakPoints.find(idx);

            ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
            {
                breakPointPos.x += breakPointAreaSize.x * 0.5f;
                breakPointPos.y += breakPointAreaSize.x * 0.5f - 2.0f; // give -2.0f for a visual beauty.
            }

            ImGui::SetCursorPosY(renderPivotY - 2.0f);

            // if (ImGui::Button(std::format("##{}", idx).c_str(), breakPointButtonSize))
            if (ImGui::InvisibleButton(std::format("BreakPointBtn##{}", idx).c_str(), breakPointButtonSize))
            {
                if (_breakPoints.end() == breakPointIter)
                {
                    _breakPoints.insert(idx);
                }
                else
                {
                    breakPointIter = _breakPoints.erase(breakPointIter);
                }
            }

            if (_breakPoints.end() != breakPointIter)
            {
                editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
            }

            if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
            {
                editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
            }

            /**
             * Text Line
             */
            ImGui::SetCursorPosX(breakPointAreaSize.x);
            ImGui::SetCursorPosY(renderPivotY);

            // Text Line Marker
            {
                ImVec2 sp = { ImGui::GetCursorScreenPos().x + 1.0f, ImGui::GetCursorScreenPos().y - 1.0f };
                ImVec2 ep = { sp.x, sp.y + kFontSize.y };
            
                editorDrawList->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
            }

            ImGui::SetCursorPosX(renderPivotX + breakPointAreaSize.x + kTextSpacingAtBeginning);

            // Debug Code
            // editorDrawList->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));

            for (auto& charInfo : _textLines[idx])
            {
                ImGui::Text("%c", charInfo.ch);
        
                ImGui::SameLine();
            }

            ImGui::SetCursorPosY(renderPivotY);

            ImGui::Text("\n");

            // Update
            renderPivotX = style.WindowPadding.x;
            renderPivotY += kFontSize.y + style.ItemSpacing.y;
        }
    }
    ImGui::EndChild();
}

void TextEditor::renderFooter()
{
    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_None;

    if (_renderFooterCallback)
    {
        ImGui::BeginChild(std::format("{}##Footer", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, kChildFlags, kWindowFlags);
        {
            _renderFooterCallback(*this);

            _footerHeight = ImGui::GetWindowSize().y;
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
