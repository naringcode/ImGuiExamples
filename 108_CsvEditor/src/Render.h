#pragma once

#include <string>
#include <string_view>

#include <vector>

#include <imgui.h>

class WidgetWindow
{
public:
    static constexpr auto kPopupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto kPopupSize  = ImVec2{ 300.0f, 100.0f };
    static constexpr auto kPopupButtonSize = ImVec2{ 120.0f, 0.0f }; // 0을 지정하면 Automatically하게 사이즈 조절

    static constexpr auto kMaxNumRows = 30;
    static constexpr auto kMaxNumCols = 8;

public:
    WidgetWindow() : _data{}, _filenameBuffer("test.csv")
    { }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

private:
    void drawSizeButtons();
    void drawIoButtons();
    void drawTable();

    void drawSavePopup();
    void drawLoadPopup();
    void drawValuePopup(const int kRow, const int kCol);

    void saveToCsvFile(const std::string_view& filename);
    void loadFromCsvFile(const std::string_view& filename);

    template <typename T>
    void PlotCellValue(std::string_view formatter, const T value);

    void SetPopupLayout();

private:
    // CSV는 테이블
    std::int32_t _numCols = 0;
    std::int32_t _numRows = 0;
    std::vector<std::vector<float>> _data;

    char _filenameBuffer[256];
};

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
