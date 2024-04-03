#pragma once

#include <string>
#include <string_view>

#include <vector>

#include <imgui.h>

class WidgetWindow
{
public:
    // Position, Color, Size
    using PointData = std::tuple<ImVec2, ImColor, float>;

    static constexpr auto kBufferSize = std::size_t{ 1024 }; // 1024 characters

    static constexpr auto kPopupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto kPopupSize = ImVec2{ 300.0f, 100.0f };
    static constexpr auto kPopupButtonSize = ImVec2{ 120.0f, 0.0f }; // 0을 지정하면 Automatically하게 사이즈 조절

public:
    WidgetWindow() : _points{}, _canvasPos{}, _currentDrawColor{ ImColor(255, 255, 255) }, _pointDrawSize{ 2.0f }, _filenameBuffer{ "test.bin" }
    { }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

private:
    void drawMenu();
    void drawCanvas();
    void drawColorButtons();
    void drawSizeSettings();

    void drawSavePopup();
    void drawLoadPopup();

    void saveToImageFile(std::string_view filename);
    void loadFromImageFile(std::string_view filename);

    void clearCanvas();

private:
    std::uint32_t _numRows = 800;
    std::uint32_t _numCols = 600;
    std::uint32_t _numChannels = 3; // RGB

    ImVec2 _canvasSize = ImVec2{ (float)_numRows, (float)_numCols };

    std::vector<PointData> _points;
    ImVec2 _canvasPos;

    ImColor _currentDrawColor;
    float   _pointDrawSize;

    char _filenameBuffer[256];
};

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
