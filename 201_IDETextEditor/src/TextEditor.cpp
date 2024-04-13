#include "TextEditor.h"

#include <format>

#define RENDER_TEST 1
#define CHOOSE_EDITOR 2

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

#if CHOOSE_EDITOR == 1

void TextEditor::renderEditor()
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
        ImDrawList* editorDrawList = ImGui::GetWindowDrawList();

        /**
         * Variables for Sizing
         */
        const float kFontHeight = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#").y;
        
        _lineHeight = kFontHeight + _itemSpacingY;

        float scrollX = 0.0f;
        float scrollY = 0.0f;

        const ImVec2 kContentsSize = kEditorBodySize - kBorderThicknessSize * 2;

        float subWindowWidth = 0.0f;

        /**
         * Variables for Break-Point Area
         */
        auto breakPointAreaSize = ImVec2{ std::round(kFontHeight * 1.15f), ImGui::GetContentRegionAvail().y - (kBorderThicknessSize.y * 2.0f) };
            
        ImVec2 breakPointAreaRectMin = ImGui::GetWindowPos() + kBorderThicknessSize;
        ImVec2 breakPointAreaRectMax = breakPointAreaRectMin + breakPointAreaSize;
            
        ImVec2 breakPointButtonSize = ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x + 0.0f };

        /**
         * Variable for Line-No Area
         */
        const int32_t kTotalLineNo = (int32_t)_textLines.size();
        int32_t minLineNo = 0;
        int32_t maxLineNo = 0;

        std::string lineNoWidthStr = std::format("{}", kTotalLineNo);
        const float kLineNoWidth   = ImGui::GetFont()->CalcTextSizeA(kFontHeight, FLT_MAX, 0.0f, lineNoWidthStr.c_str()).x;

        // TODO
        // if breakpoints enabled
        {
            subWindowWidth += breakPointButtonSize.x;
        }

        // TODO
        // if lineNo enabled
        {
            // subWindowWidth += kLineNoWidth;
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
            /**
             * 
             */
            auto renderPivot = ImVec2{ _editorStartPadding.x, _editorStartPadding.y };

            scrollX = ImGui::GetScrollX();
            scrollY = ImGui::GetScrollY();

            for (int32_t idx = 0; idx < kTotalLineNo; idx++)
            {
                // Debug Code
                // editorDrawList->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));

                // minLineNo = ;
                // maxLineNo = ;

                //for (auto& charInfo : _textLines[idx])
                //{
                //    ImGui::Text("%c", charInfo.ch);
                //
                //    ImGui::SameLine();
                //}
                //
                //ImGui::SetCursorPosY(renderPivot.y);
                //
                //ImGui::Text("\n");

                // Update
                renderPivot.x = _editorStartPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

        }
        ImGui::EndChild();
        
        ImGui::SameLine();

        /**
         * Editor Sub Window (Breakpoint, Line No)
         */
        ImGui::SetNextWindowPos(kSubWindowPos);

        ImGui::BeginChild(std::format("{}##EditorSub", _windowTitle).c_str(), kSubWindowSize, kSubChildFlags, kSubWindowFlags);
        {
            ImDrawList* childDrawList = ImGui::GetWindowDrawList();

            auto renderPivot = ImVec2{ _editorStartPadding.x, _editorStartPadding.y };

            ImGui::SetScrollY(scrollY);

            // Debug Code
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointAreaRectMin, 2.0f, IM_COL32_WHITE);
            // ImGui::GetForegroundDrawList()->AddCircleFilled(breakPointAreaRectMax, 2.0f, IM_COL32_WHITE);

            childDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));
            
            for (int32_t idx = 0; idx < kTotalLineNo; idx++)
            {
                // Debug Code
                // ImGui::GetForegroundDrawList()->AddRect(ImGui::GetWindowPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x }, IM_COL32_WHITE);
                // ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);

                // Debug Code for Marking
                {
                    ImVec2 sp = { ImGui::GetWindowPos().x + breakPointAreaSize.x + renderPivot.x, ImGui::GetWindowPos().y + renderPivot.y };
                    ImVec2 ep = { sp.x, sp.y + kFontHeight };

                    ImGui::GetForegroundDrawList()->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
                }

                /**
                 * Break-Point
                 */
                auto breakPointIter = _breakPoints.find(idx);
                
                // ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                ImVec2 breakPointPos = ImGui::GetWindowPos() + renderPivot;
                {
                    breakPointPos.x += breakPointAreaSize.x * 0.5f;
                    breakPointPos.y += breakPointAreaSize.x * 0.5f - 2.0f; // give -2.0f for a visual beauty.
                }
                
                ImGui::SetCursorPosX(renderPivot.x);
                ImGui::SetCursorPosY(renderPivot.y - 2.0f);
                
                if (ImGui::Button(std::format("##{}", idx).c_str(), breakPointButtonSize))
                // if (ImGui::InvisibleButton(std::format("BreakPointBtn##{}", idx).c_str(), breakPointButtonSize))
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

                // Update
                renderPivot.x = _editorStartPadding.x;
                renderPivot.y += kFontHeight + _itemSpacingY;
            }

            // dummy for scrolling
            ImGui::Dummy(ImVec2{ 1.0f, 1000.0f });

            // TODO
            style.ScrollbarSize;
        }
        ImGui::EndChild();
    }
    ImGui::EndChild(); 

    // ImGui::BeginChild(std::format("{}##Editor", _windowTitle).c_str(), editorBodySize, kChildFlags, kWindowFlags);
    // {
    //     ImDrawList* editorDrawList = ImGui::GetWindowDrawList();
    //     
    //     /**
    //      * Variables for Position and Size
    //      */
    //     const ImVec2 kFontSize    = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#");
    //     const float  kMaxFontSize = std::max(kFontSize.x, kFontSize.y);
    // 
    //     const float kScrollX = ImGui::GetScrollX();
    //     const float kScrollY = ImGui::GetScrollY();
    // 
    //     _lineHeight = kFontSize.y + style.ItemSpacing.y;
    // 
    //     const int32_t kTotalLineNo = (int32_t)_textLines.size();
    //     const int32_t kMinLineNo   = (int32_t)floor(kScrollY / _lineHeight);
    //     const int32_t kMaxLineNo   = (int32_t)std::max(kTotalLineNo, kMinLineNo + (int32_t)floor((kScrollY + editorBodySize.y)));
    // 
    //     std::string lineNumWidthStr = std::format("{}", kTotalLineNo);
    //     const float kLineNumWidth   = ImGui::GetFont()->CalcTextSizeA(kFontSize.y, FLT_MAX, 0.0f, lineNumWidthStr.c_str()).x;
    // 
    //     constexpr float kLineNumSpacingAtBeginning  = 4.0f;
    //     constexpr float kTextLineSpacingAtBeginning = 8.0f;
    // 
    //     /**
    //      * Break-Point Area
    //      */
    //     auto breakPointAreaSize = ImVec2{ kMaxFontSize * 1.15f, ImGui::GetContentRegionAvail().y - (borderThicknessSize.y * 2.0f) };
    // 
    //     ImVec2 breakPointAreaRectMin = ImGui::GetWindowPos() + ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
    //     ImVec2 breakPointAreaRectMax = breakPointAreaRectMin + breakPointAreaSize;
    // 
    //     ImVec2 breakPointButtonSize = ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x + 0.0f };
    // 
    //     ImGui::BeginGroup();
    //     {
    //         auto renderPivot = ImVec2{ _editorStartPadding.x, _editorStartPadding.y + _lineHeight * kMinLineNo };
    // 
    //         mainDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));
    // 
    //         // Debug Code
    //         // mainDrawList->AddCircleFilled(breakPointAreaRectMin, 2.0f, IM_COL32_WHITE);
    //         // mainDrawList->AddCircleFilled(breakPointAreaRectMax, 2.0f, IM_COL32_WHITE);
    // 
    //         for (int32_t idx = kMinLineNo; idx < kTotalLineNo; idx++)
    //         {
    //             // Debug Code
    //             // mainDrawList->AddRect(ImGui::GetWindowPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x }, IM_COL32_WHITE);
    //             // mainDrawList->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);
    // 
    //             /**
    //              * Break-Point
    //              */
    //             auto breakPointIter = _breakPoints.find(idx);
    // 
    //             // ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
    //             ImVec2 breakPointPos = ImGui::GetWindowPos() + renderPivot;
    //             {
    //                 breakPointPos.x += breakPointAreaSize.x * 0.5f;
    //                 breakPointPos.y += breakPointAreaSize.x * 0.5f - 2.0f; // give -2.0f for a visual beauty.
    //             }
    // 
    //             ImGui::SetCursorPosX(renderPivot.x + kScrollX);
    //             ImGui::SetCursorPosY(renderPivot.y - 2.0f);
    // 
    //             if (ImGui::Button(std::format("##{}", idx).c_str(), breakPointButtonSize))
    //             // if (ImGui::InvisibleButton(std::format("BreakPointBtn##{}", idx).c_str(), breakPointButtonSize))
    //             {
    //                 if (_breakPoints.end() == breakPointIter)
    //                 {
    //                     _breakPoints.insert(idx);
    //                 }
    //                 else
    //                 {
    //                     breakPointIter = _breakPoints.erase(breakPointIter);
    //                 }
    //             }
    // 
    //             if (_breakPoints.end() != breakPointIter)
    //             {
    //                 editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
    //             }
    // 
    //             if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
    //             {
    //                 editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
    //             }
    // 
    //             /**
    //              * Line Num
    //              */
    //             std::string lineNoStr = std::format("{}", idx + 1);
    //             const float kCurrLineNumWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, lineNoStr.c_str()).x;
    // 
    //             ImGui::SetCursorPosX(renderPivot.x + breakPointAreaSize.x + kLineNumSpacingAtBeginning + (kLineNumWidth - kCurrLineNumWidth) + kScrollX);
    //             ImGui::SetCursorPosY(renderPivot.y - 1.0f);
    // 
    //             ImGui::Text("%s", lineNoStr.c_str());
    // 
    //             // Update
    //             renderPivot.x = _editorStartPadding.x;
    //             renderPivot.y += kFontSize.y + _itemSpacingY;
    //         }
    //     }
    //     ImGui::EndGroup();
    // 
    //     ImGui::BeginGroup();
    //     {
    //         auto renderPivot = ImVec2{ _editorStartPadding.x, _editorStartPadding.y + _lineHeight * kMinLineNo };
    // 
    //     // for (int32_t idx = 0; idx < _textLines.size(); idx++)
    //     for (int32_t idx = kMinLineNo; idx < kTotalLineNo; idx++)
    //     {
    //         // Debug Code
    //         //mainDrawList->AddRect(ImGui::GetWindowPos(), ImGui::GetCursorScreenPos() + ImVec2{ breakPointAreaSize.x, breakPointAreaSize.x }, IM_COL32_WHITE);
    //         //mainDrawList->AddCircleFilled(ImGui::GetCursorScreenPos(), 2, IM_COL32_WHITE);
    // 
    //         /**
    //          * Break-Point
    //          */
    //         // auto breakPointIter = _breakPoints.find(idx);
    //         // 
    //         // // ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
    //         // ImVec2 breakPointPos = ImGui::GetWindowPos() + renderPivot;
    //         // {
    //         //     breakPointPos.x += breakPointAreaSize.x * 0.5f;
    //         //     breakPointPos.y += breakPointAreaSize.x * 0.5f - 2.0f; // give -2.0f for a visual beauty.
    //         // }
    //         // 
    //         // ImGui::SetCursorPosY(renderPivot.y - 2.0f);
    //         // 
    //         // if (ImGui::Button(std::format("##{}", idx).c_str(), breakPointButtonSize))
    //         // // if (ImGui::InvisibleButton(std::format("BreakPointBtn##{}", idx).c_str(), breakPointButtonSize))
    //         // {
    //         //     if (_breakPoints.end() == breakPointIter)
    //         //     {
    //         //         _breakPoints.insert(idx);
    //         //     }
    //         //     else
    //         //     {
    //         //         breakPointIter = _breakPoints.erase(breakPointIter);
    //         //     }
    //         // }
    //         // 
    //         // if (_breakPoints.end() != breakPointIter)
    //         // {
    //         //     editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(197, 81, 89, 255));
    //         // }
    //         // 
    //         // if (ImGui::IsItemHovered() && _breakPoints.end() == breakPointIter)
    //         // {
    //         //     editorDrawList->AddCircleFilled(breakPointPos, breakPointAreaSize.x * 0.4f, IM_COL32(183, 183, 183, 255));
    //         // }
    // 
    //         /**
    //          * Line Num
    //          */
    //         // std::string lineNoStr = std::format("{}", idx + 1);
    //         // const float kCurrLineNumWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, lineNoStr.c_str()).x;
    //         // 
    //         // ImGui::SetCursorPosX(renderPivot.x + breakPointAreaSize.x + kLineNumSpacingAtBeginning + (kLineNumWidth - kCurrLineNumWidth));
    //         // ImGui::SetCursorPosY(renderPivot.y - 1.0f);
    //         // 
    //         // ImGui::Text("%s", lineNoStr.c_str());
    // 
    //         /**
    //          * Text Line
    //          */
    //         ImGui::SetCursorPosX(renderPivot.x + breakPointAreaSize.x + kLineNumSpacingAtBeginning + kLineNumWidth);
    //         ImGui::SetCursorPosY(renderPivot.y);
    // 
    //         // Text Line Marker
    //         {
    //             ImVec2 sp = { ImGui::GetCursorScreenPos().x + 1.0f, ImGui::GetCursorScreenPos().y - 1.0f };
    //             ImVec2 ep = { sp.x, sp.y + kFontSize.y };
    //         
    //             mainDrawList->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
    //             // editorDrawList->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
    //         }
    // 
    //         ImGui::SetCursorPosX(renderPivot.x + breakPointAreaSize.x + kLineNumSpacingAtBeginning + kLineNumWidth + kTextLineSpacingAtBeginning);
    // 
    //         // Debug Code
    //         // editorDrawList->AddCircle(ImGui::GetCursorScreenPos(), 1.6f, IM_COL32(255, 0, 255, 255));
    // 
    //         for (auto& charInfo : _textLines[idx])
    //         {
    //             ImGui::Text("%c", charInfo.ch);
    //     
    //             ImGui::SameLine();
    //         }
    // 
    //         ImGui::SetCursorPosY(renderPivot.y);
    // 
    //         ImGui::Text("\n");
    // 
    //         // Update
    //         renderPivot.x = _editorStartPadding.x;
    //         renderPivot.y += kFontSize.y + _itemSpacingY;
    //     }
    //     }
    //     ImGui::EndGroup();
    // }
    // ImGui::EndChild();
}

#elif CHOOSE_EDITOR == 2

void TextEditor::renderEditor()
{
    _editorStartPadding = ImVec2{ 1.0f, 5.0f };

    constexpr ImGuiChildFlags  kChildFlags  = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Border;
    constexpr ImGuiWindowFlags kWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    
    ImGuiStyle& style        = ImGui::GetStyle();
    ImDrawList* mainDrawList = ImGui::GetWindowDrawList();

    auto borderThicknessSize = ImVec2{ style.ChildBorderSize, style.ChildBorderSize };
    auto bodySize = ImVec2{ 0.0f, 0.0f };

    if (_renderFooterCallback)
    {
        bodySize.y = ImGui::GetContentRegionAvail().y - _footerSpacingY - borderThicknessSize.y;
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

#endif

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
