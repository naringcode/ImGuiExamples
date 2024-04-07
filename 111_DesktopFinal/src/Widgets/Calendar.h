#pragma once

#include <string>
#include <string_view>

#include <chrono>
#include <algorithm>

#include <array>
#include <map>

#include <imgui.h>

#include "WindowBase.h"

class CalendarWindow : public WindowBase
{
public:
    static constexpr auto kMeetingWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto kMeetingWindowSize = ImVec2{ 300.0f, 100.0f };
    static constexpr auto kMeetingWindowButtonSize = ImVec2{ 120.0f, 0.0f }; // 0을 지정하면 Automatically하게 사이즈 조절

public:
    static constexpr auto kMonthNames = std::array<std::string_view, 12U> {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December",
    };

    static constexpr auto kMinYear = 2000U;
    static constexpr auto kMaxYear = 2038U;

    struct Meeting
    {
        std::string name;

        void Serialize(std::ofstream& out) const;
        static Meeting Deserialize(std::ifstream& in);

        // Comparing Function for (erase()...)
        constexpr bool operator==(const Meeting& rhs) const
        {
            return name == rhs.name;
        }
    };

public:
    CalendarWindow() : _meetings{ }
    {
        this->loadMeetingsFromFile("meetings.bin");
    }

    virtual ~CalendarWindow()
    {
        this->saveMeetingsToFile("meetings.bin");
    }

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;

private:
    void drawDataCombo();
    void drawCalendar();
    void drawAddMeetingWindow();
    void drawMeetingList();

    void loadMeetingsFromFile(const std::string_view& filename);
    void saveMeetingsToFile(const std::string_view& filename);

    void updateSelectedDateVariables();

private:
    int _selectedDay = 1;
    int _selectedMonth = 1;
    int _selectedYear = 2024;

    std::chrono::year_month_day _selectedDate; // C++20

    bool _addMeetingWindowOpen = false;

    float _calendarFontSize = 2.0f;

    std::map<std::chrono::year_month_day, std::vector<Meeting>> _meetings;
};
