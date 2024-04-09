#include "TextEditor.h"

void TextEditor::Render(bool* open)
{
    if (ImGui::Begin(_windowTitle.c_str(), open, _windowFlags))
    {

    }

    ImGui::End();
}
