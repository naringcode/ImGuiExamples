#include "Render.h"

#include <format>
#include <cmath>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/1_HelloWorld/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    drawSelection();
    drawPlot();

    ImGui::End();
}

void WidgetWindow::drawSelection()
{
    for (const std::string_view& funcName : kFunctionNames)
    {
        const Function currFunction = functionNameMapping(funcName);

        bool selected = _selectedFunctions.contains(currFunction);

        if (ImGui::Checkbox(funcName.data(), &selected))
        {
            if (true == selected)
            {
                _selectedFunctions.insert(currFunction);
            }
            else
            {
                _selectedFunctions.erase(currFunction);
            }
        }
    }
}

void WidgetWindow::drawPlot()
{
    static constexpr int   kNumPoints = 10'000;
    static constexpr double kMinX = -100.0;
    static constexpr double kMaxX = 100.0;
    static const     double stepX = (std::abs(kMaxX) + std::abs(kMinX)) / kNumPoints;

    static auto xArr = std::array<double, kNumPoints>{ };
    static auto yArr = std::array<double, kNumPoints>{ };

    bool noDraw = (0 == _selectedFunctions.size());

    // 하나 선택한 것이 kNone이면 그리지 않음
    noDraw |= (1 == _selectedFunctions.size() && Function::kNone == *_selectedFunctions.begin());

    if (true == noDraw)
    {
        // ImVec2{ -1.0f, -1.0f }을 지정하면 사용하지 않는 영역을 대상으로 그린다.
        ImPlot::BeginPlot("##Plot", ImVec2{ -1.0f, -1.0f }, ImPlotFlags_NoTitle);

        ImPlot::EndPlot();

        return;
    }

    // ImVec2{ -1.0f, -1.0f }을 지정하면 사용하지 않는 영역을 대상으로 그린다.
    ImPlot::BeginPlot("##Plot", ImVec2{ -1.0f, -1.0f }, ImPlotFlags_NoTitle);

    for (const auto& function : _selectedFunctions)
    {
        float x = kMinX;

        for (std::size_t i = 0; i < kNumPoints; i++)
        {
            xArr[i] = x;
            yArr[i] = evaluateFunction(function, x);

            x += stepX;
        }

        const std::string plotLabel = std::format("##function{}", (int)function);

        // 점을 연결해서 보여줌
        // ImPlot::PlotLine(plotLabel.data(), xArr.data(), yArr.data(), kNumPoints);

        // 점으로 보여줌
        ImPlot::PlotScatter(plotLabel.data(), xArr.data(), yArr.data(), kNumPoints);
    }

    ImPlot::EndPlot();
}

WidgetWindow::Function WidgetWindow::functionNameMapping(std::string_view functionName)
{
    if ("sin(x)" == functionName)
        return WidgetWindow::Function::kSin;

    if ("cos(x)" == functionName)
        return WidgetWindow::Function::kCos;

    return WidgetWindow::Function::kNone;
}

double WidgetWindow::evaluateFunction(const Function function, const double x)
{
    switch (function)
    {
        case Function::kSin:
            return std::sin(x);

        case Function::kCos:
            return std::cos(x);

        case Function::kNone:
        default:
        {

            return 0.0f;
        }
    }
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}
