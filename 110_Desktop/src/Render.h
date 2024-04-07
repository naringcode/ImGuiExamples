#pragma once

#include <string>
#include <format>

#include <vector>

#include "Clock.h"

class WidgetWindow
{
public:
    static constexpr auto kNumIcons = std::uint32_t{ 10 };
    
    struct Icon
    {
        std::string   label;
        ImVec2        position;
        bool          popupOpen;
        std::uint32_t clickCount;

        Icon(std::string_view label) 
            : label(label), position(ImVec2{ }), popupOpen(false), clickCount(0)
        { }

        void Draw();
    };

public:
    WidgetWindow() : _icons{}, _clock{}
    {
        _icons.reserve(kNumIcons);

        for (std::uint32_t i = 0; i < kNumIcons; i++)
        {
            _icons.push_back(Icon{ std::format("Icon{}", i) });
        }
    }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

private:
    void drawDesktop();
    void drawTaskbar();

    void showIconList(bool* open = nullptr);

private:
    std::vector<Icon> _icons;

    ClockWindow _clock;
};

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
