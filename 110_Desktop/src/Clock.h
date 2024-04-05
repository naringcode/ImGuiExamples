#pragma once

#include <string>
#include <numbers>

#include <imgui.h>

class ClockWindow
{
private:
    static constexpr auto kPi = std::numbers::pi_v<float>;
    static constexpr auto kCircleRadius = 250.0f;
    static constexpr auto kOffsetTheta = kPi / 2.0f;
    static constexpr auto kInnerRadius = 5.0f;
    static constexpr auto kHrsClockHandLength = 0.95f; // 시침
    static constexpr auto kMinsClockHandLength = 0.85f; // 분침
    static constexpr auto kSecsClockHandLength = 0.75f; // 초침
    static constexpr auto kHrsStrokesLength = 0.9f;
    static constexpr auto kMinsStrokesLength = 0.95f;

public:
    ClockWindow() : _secs(0), _mins(0), _hrs(0), _center{}
    { }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

private:
    void drawCircle(const float kRadius);
    void drawClockHand(const float kRadius, const float kTheta, const ImColor kColor);

    void drawAllHourStrokes();
    void drawAllMinuteStrokes();
    void drawDigitalClock();

public:
    void GetTime();
    std::tuple<float, float, float> GetTheta();

public:
    std::int32_t _secs;
    std::int32_t _mins;
    std::int32_t _hrs;

private:
    ImVec2 _center;
};

void Render(ClockWindow& window, int systemWindowWidth, int systemWindowHeight);
