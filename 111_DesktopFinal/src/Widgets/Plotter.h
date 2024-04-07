#pragma once

#include <string>
#include <array>
#include <set>

#include "WindowBase.h"

class PlotterWindow : public WindowBase
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
    PlotterWindow() : _selectedFunctions({ })
    { }

    virtual ~PlotterWindow()
    { }

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;

public:
    void drawSelection();
    void drawPlot();

private:
    Function functionNameMapping(std::string_view functionName);
    double   evaluateFunction(const Function function, const double x);

public:
    std::set<Function> _selectedFunctions;
};
