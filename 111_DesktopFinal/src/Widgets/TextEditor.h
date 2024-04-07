#pragma once

#include <string>
#include <filesystem>

#include <imgui.h>

#include "WindowBase.h"

namespace fs = std::filesystem;

class TextEditorWindow : public WindowBase
{
public:
    static constexpr auto kBufferSize = std::size_t{ 1024 }; // 1024 characters
    static constexpr auto kPopupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    static constexpr auto kPopupSize  = ImVec2{ 300.0f, 100.0f };
    static constexpr auto kPopupButtonSize = ImVec2{ 120.0f, 0.0f }; // 0�� �����ϸ� Automatically�ϰ� ������ ����

public:
    TextEditorWindow() : _currentFilename({})
    {
        std::memset(_textBuffer, 0x00, kBufferSize);
    }

    virtual ~TextEditorWindow()
    { }

public:
    void Draw(const std::string& windowName, bool* open) override final;

private:
    void drawMenu();
    void drawContents();
    void drawInfo();

    void drawSavePopup();
    void drawLoadPopup();

    void saveToFile(const std::string_view& filename);
    void loadFromFile(const std::string_view& filename);

    std::string getFileExtension(const std::string_view& filename);

private:
    char _textBuffer[kBufferSize];

    std::string _currentFilename;
};
