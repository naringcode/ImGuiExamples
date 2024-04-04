#include "Render.h"

#include <chrono>
#include <time.h>

#include <tuple>
#include <cmath>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/WallClock/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    const auto kCursorPos = ImGui::GetCursorScreenPos();
    _center = ImVec2(kCursorPos.x + kCircleRadius, kCursorPos.y + kCircleRadius);

    drawCircle(kCircleRadius);

    getTime();
    const auto [hourTheta, minuteTheta, secondTheta] = getTheta();

    drawClockHand(kCircleRadius * kHrsClockHandLength,  hourTheta,   ImColor{ 1.0f, 0.0f, 0.0f, 1.0f });
    drawClockHand(kCircleRadius * kMinsClockHandLength, minuteTheta, ImColor{ 0.0f, 1.0f, 0.0f, 1.0f });
    drawClockHand(kCircleRadius * kSecsClockHandLength, secondTheta, ImColor{ 0.0f, 0.0f, 1.0f, 1.0f });
    
    drawAllHourStrokes();
    drawAllMinuteStrokes();

    drawCircle(kInnerRadius);

    drawDigitalClock();

    ImGui::End();
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}

void WidgetWindow::drawCircle(const float kRadius)
{
    ImGui::GetWindowDrawList()->AddCircle(_center, kRadius, ImGui::GetColorU32(ImGuiCol_Text), 100, 2.0f);
}

void WidgetWindow::drawClockHand(const float kRadius, const float kTheta, const ImColor kColor)
{
    const auto cos = std::cos(kTheta + kOffsetTheta);
    const auto sin = std::sin(kTheta + kOffsetTheta);

    const auto endPoint = ImVec2{ _center.x - kRadius * cos, _center.y - kRadius * sin };

    // ImColor는 ImU32, ImVec4로 변환될 수 있게 연산자 오퍼레이터가 구현되어 있다.
    ImGui::GetWindowDrawList()->AddLine(_center, endPoint, kColor, 3.0f);
}

void WidgetWindow::drawAllHourStrokes()
{
    for (std::uint32_t hr = 0; hr < 12; hr++)
    {
        const auto kTheta = (hr / 12.0f) * (2.0f * kPi); // + kOffsetTheta;

        const auto cos = std::cos(kTheta);
        const auto sin = std::sin(kTheta);

        const auto startPoint = ImVec2{ _center.x + (kCircleRadius * kHrsStrokesLength) * cos,
                                        _center.y + (kCircleRadius * kHrsStrokesLength) * sin };

        const auto endPoint = ImVec2{ _center.x + kCircleRadius * cos, _center.y + kCircleRadius * sin };

        ImGui::GetWindowDrawList()->AddLine(startPoint, endPoint, ImGui::GetColorU32(ImGuiCol_Text), 2.0f);
    }
}

void WidgetWindow::drawAllMinuteStrokes()
{
    for (std::uint32_t min = 0; min < 60; min++)
    {
        const auto kTheta = (min / 60.0f) * (2.0f * kPi); // + kOffsetTheta;
    
        const auto cos = std::cos(kTheta);
        const auto sin = std::sin(kTheta);
    
        const auto startPoint = ImVec2{ _center.x + (kCircleRadius * kMinsStrokesLength) * cos,
                                        _center.y + (kCircleRadius * kMinsStrokesLength) * sin };
    
        const auto endPoint = ImVec2{ _center.x + kCircleRadius * cos, _center.y + kCircleRadius * sin };
    
        ImGui::GetWindowDrawList()->AddLine(startPoint, endPoint, ImGui::GetColorU32(ImGuiCol_Text), 1.0f);
    }
}

void WidgetWindow::drawDigitalClock()
{
    ImGui::Text("%02d:%02d:%02d", _hrs, _mins, _secs);
}

void WidgetWindow::getTime()
{
    // high_resolution_clock은 주로 시간 측정에 사용됨.
    // const auto timestampNow = std::chrono::high_resolution_clock::now();

    // system_clock은 현재 시간을 얻고자 할 때 주로 사용됨.
    const auto timestampNow = std::chrono::system_clock::now();

    // https://modoocode.com/113
    // time_t는 C 프로그래밍에서 사용하는 구조체
    // time_t는 32비트 혹은 64비트 정수형이다(1970년 1월 1일 자정부터 현재까지 흐른 초가 저장됨).
    const auto timeNow = std::chrono::system_clock::to_time_t(timestampNow);
    
    // https://modoocode.com/120
    // localtime()은 time_t를 tm 구조체로 변환한다(내부에서 정적 버퍼를 활용하기 때문에 스레드 안전하지 않음).
    // const auto timeStruct = std::localtime(&timeNow);
    // 
    // _secs = timeStruct->tm_sec;
    // _mins = timeStruct->tm_min;
    // _hrs  = timeStruct->tm_hour;

    // 스레드 안전하게 사용 가능
    tm timeStruct;
    localtime_s(&timeStruct, &timeNow);

    _secs = timeStruct.tm_sec;
    _mins = timeStruct.tm_min;
    _hrs  = timeStruct.tm_hour;

}

std::tuple<float, float, float> WidgetWindow::getTheta()
{
    const auto kSecondsFrac = static_cast<float>(_secs);
    const auto kMinutesFrac = static_cast<float>(_mins) + kSecondsFrac / 60.0f;
    const auto kHoursFrac   = static_cast<float>(_hrs)  + kMinutesFrac / 60.0f;

    const auto kHourTheta   = (kHoursFrac   / 12.0f) * (2.0f * kPi); // + kOffsetTheta;
    const auto kMinuteTheta = (kMinutesFrac / 60.0f) * (2.0f * kPi); // + kOffsetTheta;
    const auto kSecondTheta = (kSecondsFrac / 60.0f) * (2.0f * kPi); // + kOffsetTheta;

    // return std::make_tuple(kHourTheta, kMinuteTheta, kSecondTheta);
    return { kHourTheta, kMinuteTheta, kSecondTheta };
}