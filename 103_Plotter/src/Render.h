#pragma once

#include <string>
#include <array>
#include <set>

class WidgetWindow
{
public:
    constexpr static auto kFunctionNames = std::array<std::string_view, 3>{ "unk", "sin(x)", "cos(x)" };

    enum class Function
    {
        kNone,
        kSin,
        kCos,
    };

public:
    WidgetWindow() : _selectedFunctions({ })
    { }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

public:
    void drawSelection();
    void drawPlot();

private:
    Function functionNameMapping(std::string_view functionName);
    double   evaluateFunction(const Function function, const double x);

public:
    std::set<Function> _selectedFunctions;
};

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
