#include "TextEditor.h"

#include <format>

void TextEditor::Render(bool* open)
{
    if (nullptr != open && false == *open)
        return;

    ImGuiStyle& style = ImGui::GetStyle();

    // I don't know why but PushStyleVar() have to be placed here before calling ImGui::Begin() which creates a new window.
    // this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 4.0f, 4.0f });

    this->beginStyle();

    this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 6.0f, 6.0f });
    this->pushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
    this->pushStyleColor(ImGuiCol_ResizeGrip, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        ImDrawList* mainDrawList = ImGui::GetWindowDrawList();

        // /**
        //  * Menu
        //  */
        // if (_renderMenuCallback && 0 != (_windowFlags & ImGuiWindowFlags_MenuBar))
        // {
        //     _renderMenuCallback(*this);
        // }
        
        /**
         * Header
         */
        this->beginStyle();

        // ImGui::BeginChild(std::format("{}##Header", _windowTitle).c_str());
        {
            if (_renderHeaderCallback)
            {
                _renderHeaderCallback(*this);
            }
        }
        // ImGui::EndChild();

        this->endStyle();

        /**
         * Editor Part
         */
        this->beginStyle();

        this->pushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(IM_COL32(30, 30, 30, 255)));
        this->pushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.0f, 4.0f }); // 3.0f or 4.0f
        this->pushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 3.0f }); // 3.0f or 4.0f

        ImGui::BeginChild(std::format("{}##Editor", _windowTitle).c_str(), ImVec2{ 0.0f, 0.0f }, ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImDrawList* editorDrawList = ImGui::GetWindowDrawList();

            const ImVec2 fontSize    = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "#");
            const float  maxFontSize = std::max(fontSize.x, fontSize.y);
        
            /**
             * Breakpoitns Area Test
             */
            ImVec2 breakPointAreaRectMin = ImGui::GetCursorScreenPos();
            ImVec2 breakPointAreaRectMax;
            {
                // +1.0f means borders
                breakPointAreaRectMin.x += ImGui::GetScrollX() - style.WindowPadding.x + 1.0f;
                breakPointAreaRectMin.y += ImGui::GetScrollY() - style.WindowPadding.y + 1.0f;

                breakPointAreaRectMax.x = breakPointAreaRectMin.x + maxFontSize;
                breakPointAreaRectMax.y = breakPointAreaRectMin.y + ImGui::GetContentRegionAvail().y;

                // -2.0f means borders
                breakPointAreaRectMax.x += style.WindowPadding.x * 2.0f - 2.0f;
                breakPointAreaRectMax.y += style.WindowPadding.y * 2.0f - 2.0f;
            }

            mainDrawList->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));
            // ImGui::GetForegroundDrawList()->AddRectFilled(breakPointAreaRectMin, breakPointAreaRectMax, IM_COL32(51, 51, 51, 255));

            for (auto& textLine : _textLines)
            {
                // Breakpoints Test
                ImVec2 breakPointPos = ImGui::GetCursorScreenPos();
                {
                    breakPointPos.x += maxFontSize * 0.5f + ImGui::GetScrollX();
                    breakPointPos.y += maxFontSize * 0.5f - 1.0f; // + ImGui::GetScrollY();
                }

                editorDrawList->AddCircleFilled(breakPointPos, maxFontSize * 0.45f, IM_COL32(197, 81, 89, 255));

                // Debug Code for Text Line
                {
                    ImVec2 sp = { ImGui::GetCursorScreenPos().x + maxFontSize + style.WindowPadding.x * 2.0f, ImGui::GetCursorScreenPos().y };
                    ImVec2 ep = { sp.x, sp.y + fontSize.y };

                    editorDrawList->AddLine(sp, ep, IM_COL32(255, 0, 0, 255));
                }
                
                // Text Line Test
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + maxFontSize + style.WindowPadding.x * 2.0f + 8.0f);


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

        // /**
        //  * Footer
        //  */
        // if (_renderFooterCallback)
        // {
        //     _renderFooterCallback(*this);
        // }

    }
    ImGui::End();

    this->endStyle();
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
