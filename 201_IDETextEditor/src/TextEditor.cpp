#include "TextEditor.h"

void TextEditor::Render(bool* open)
{
    if (nullptr != open && false == *open)
        return;

    ImGui::Begin(_windowTitle.c_str(), open, _windowFlags);
    {
        if (_renderMenuCallback && 0 != (_windowFlags & ImGuiWindowFlags_MenuBar))
        {
            _renderMenuCallback(*this);
        }

        if (_renderHeaderCallback)
        {
            _renderHeaderCallback(*this);
        }

        for (auto& textLine : _textLines)
        {
            for (auto& charInfo : textLine)
            {
                ImGui::Text("%c", charInfo.ch);

                ImGui::SameLine(0.0f, 0.0f);
            }

            ImGui::Text("\n");
        }

        if (_renderFooterCallback)
        {
            _renderFooterCallback(*this);
        }
    }
    ImGui::End();
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
