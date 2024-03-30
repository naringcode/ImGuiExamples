#pragma once

#include <string>

class WidgetWindow
{
public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);
};

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
