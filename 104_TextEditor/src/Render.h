#pragma once

#include <string>
#include <filesystem>

#include <imgui.h>

namespace fs = std::filesystem;

class WidgetWindow
{
public:
    static constexpr auto kBufferSize = std::size_t{ 1024 }; // 1024 characters
    static constexpr auto kPopupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    static constexpr auto kPopupSize  = ImVec2{ 300.0f, 100.0f };
    static constexpr auto kPopupButtonSize = ImVec2{ 120.0f, 0.0f }; // 0을 지정하면 Automatically하게 사이즈 조절

public:
    WidgetWindow() : _currentFilename({})
    {
        std::memset(_textBuffer, 0x00, kBufferSize);
    }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

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

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
