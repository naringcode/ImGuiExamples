#include "Calendar.h"

#include <fstream>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/Calendar

void CalendarWindow::Draw(const std::string& windowName, bool* open)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(this->GetNextPos());
    ImGui::SetNextWindowSize(this->GetNextSize());

    ImGui::Begin(windowName.c_str(), open, kWindowFlags);

    drawDataCombo();

    ImGui::Separator();

    drawCalendar();

    ImGui::Separator();
    
    drawMeetingList();

    if (_addMeetingWindowOpen)
    {
        drawAddMeetingWindow();
    }

    ImGui::End();
}

void CalendarWindow::drawDataCombo()
{
    ImGui::Text("Select a Date:");

    ImGui::PushItemWidth(50);

    if (ImGui::BeginCombo("##day", std::to_string(_selectedDay).data()))
    {
        for (std::int32_t dayIdx = 1; dayIdx <= 31; dayIdx++)
        {
            const auto currDate = std::chrono::year_month_day(std::chrono::year(_selectedYear), std::chrono::month(_selectedMonth), std::chrono::day(dayIdx));

            // ��¥�� ��ȿ�� �� �޺��� �߰��ϱ�
            if (false == currDate.ok())
                break;

            if (ImGui::Selectable(std::to_string(dayIdx).data(), dayIdx == _selectedDay))
            {
                _selectedDay  = dayIdx;
                _selectedDate = currDate;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushItemWidth(100);

    // if (ImGui::BeginCombo("##month", std::to_string(_selectedMonth).data()))
    if (ImGui::BeginCombo("##month", kMonthNames[_selectedMonth - 1].data()))
    {
        for (std::int32_t monthIdx = 1; monthIdx <= 12; monthIdx++)
        {
            const auto currDate = std::chrono::year_month_day(std::chrono::year(_selectedYear), std::chrono::month(monthIdx), std::chrono::day(_selectedDay));

            if (false == currDate.ok())
                break;

            // if (ImGui::Selectable(std::to_string(monthIdx).data(), monthIdx == _selectedMonth))
            if (ImGui::Selectable(kMonthNames[monthIdx - 1].data(), monthIdx == _selectedMonth))
            {
                _selectedMonth = monthIdx;
                _selectedDate  = currDate;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushItemWidth(60);

    if (ImGui::BeginCombo("##year", std::to_string(_selectedYear).data()))
    {
        for (std::int32_t yearIdx = kMinYear; yearIdx <= kMaxYear; yearIdx++)
        {
            const auto currDate = std::chrono::year_month_day(std::chrono::year(yearIdx), std::chrono::month(_selectedMonth), std::chrono::day(_selectedDay));

            if (false == currDate.ok())
                break;

            if (ImGui::Selectable(std::to_string(yearIdx).data(), yearIdx == _selectedYear))
            {
                _selectedYear = yearIdx;
                _selectedDate = currDate;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();

    if (ImGui::Button("Add Meeting"))
    {
        _addMeetingWindowOpen = true;
    }
}

void CalendarWindow::drawCalendar()
{
    const auto kChildSize = ImVec2{ ImGui::GetContentRegionAvail().x, 400.0f };

    ImGui::BeginChild("##calendar", kChildSize);

    // const float originalFontSize = ImGui::GetFontSize();
    const float originalFontSize = ImGui::GetIO().FontGlobalScale;

    // ��Ʈ ũ�� ��ü
    ImGui::SetWindowFontScale(_calendarFontSize); // Deprecated // �κ������� �����ϸ��� �� ����.
    // ImGui::GetIO().FontGlobalScale = _calendarFontSize; // �κ������� �����ϸ��ϴ� ���� �� ��.

    // ���� ��¥ ���
    const auto currTime   = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const auto todaysDate = std::chrono::year_month_day(currTime);

    const int year = _selectedYear;

    for (std::int32_t month = 1; month <= 12; month++)
    {
        // first three characters
        ImGui::Text("%s", std::format("{:.3}", kMonthNames[month - 1]).data());

        for (std::int32_t day = 1; day <= 31; day++)
        {
            const auto iterationDate = std::chrono::year_month_day(std::chrono::year(year), std::chrono::month(month), std::chrono::day(day));

            if (false == iterationDate.ok())
                continue;

            ImGui::SameLine();

            if (iterationDate == todaysDate)
            {
                ImGui::TextColored(ImVec4{ 0.0f, 1.0f, 0.f, 1.0f }, "%d", day);
            }
            else if (iterationDate == _selectedDate)
            {
                ImGui::TextColored(ImVec4{ 0.0f, 0.0f, 1.f, 1.0f }, "%d", day);
            }
            else if (_meetings.contains(iterationDate))
            {
                ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.f, 1.0f }, "%d", day);
            }
            else
            {
                ImGui::Text("%d", day);
            }

            if (ImGui::IsItemClicked())
            {
                _selectedDate = iterationDate;

                updateSelectedDateVariables();
            }
        }
    }

    // ��Ʈ ũ�� ����
    ImGui::SetWindowFontScale(originalFontSize); // Deprecated
    // ImGui::GetIO().FontGlobalScale = originalFontSize;

    ImGui::EndChild();
}

void CalendarWindow::drawAddMeetingWindow()
{
    static char meetingNameBuffer[128] = "...";

    ImVec2 winCenterPos;
    {
        winCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - winCenterPos.x / 2.0f;
        winCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - winCenterPos.y / 2.0f;
    }

    ImGui::SetNextWindowPos(winCenterPos);
    ImGui::SetNextWindowSize(kMeetingWindowSize);

    ImGui::Begin("##addMeeting", &_addMeetingWindowOpen, kMeetingWindowFlags);

    ImGui::Text("Add meeting to %d.%s.%d", _selectedDay, kMonthNames[_selectedMonth - 1].data(), _selectedYear);

    ImGui::InputText("Meeting name", meetingNameBuffer, sizeof(meetingNameBuffer));

    if (ImGui::Button("Save"))
    {
        _meetings[_selectedDate].push_back(Meeting{ meetingNameBuffer });

        std::memset(meetingNameBuffer, 0x00, sizeof(meetingNameBuffer));

        _addMeetingWindowOpen = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        _addMeetingWindowOpen = false;
    }

    ImGui::End();
}

void CalendarWindow::drawMeetingList()
{
    if (false == _meetings.contains(_selectedDate))
    {
        ImGui::Text("No meetings at all.");

        return;
    }

    ImGui::Text("Meetings on %d.%s.%d", _selectedDay, kMonthNames[_selectedMonth - 1].data(), _selectedYear);

    if (_meetings.empty())
    {
        ImGui::Text("No meetings at all.");

        return;
    }

    for (const auto& meeting : _meetings[_selectedDate])
    {
        // ���̳��� ���װ� ����.
        // - 11
        // - 11
        // �̷��� �Էµ� ���¿��� ������ �ϳ��� �ƴ� 2���� �� ������.
        // �� �κ��� ImGui�� ID�� ó���ϴ� �۾� ������ �׷� ��(������ �۾��� ���� ##(���� �ؽ��±�) ���̱�).
        ImGui::BulletText("%s", meeting.name.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            std::erase(_meetings[_selectedDate], meeting);

            if (0 == _meetings[_selectedDate].size())
            {
                _meetings.erase(_selectedDate);
            }

            // �ݺ��� ��ȿȭ�� ���� ó��
            return;
        }
    }
}

void CalendarWindow::loadMeetingsFromFile(const std::string_view& filename)
{
    // ���� �б�
    auto in = std::ifstream{ filename.data(), std::ios::binary }; // binary�� ����

    if (!in || !in.is_open())
        return;

    size_t meetingsNum = std::size_t{ 0 };

    in.read((char*)(&meetingsNum), sizeof(meetingsNum));

    for (std::size_t i = 0; i < meetingsNum; i++)
    {
        auto date = std::chrono::year_month_day{ };
        in.read((char*)(&date), sizeof(date));

        size_t meetingsNumOnThatDate = std::size_t{ 0 };
        in.read((char*)(&meetingsNumOnThatDate), sizeof(meetingsNumOnThatDate));

        for (std::size_t j = 0; j < meetingsNumOnThatDate; j++)
        {
            auto meeting = Meeting::Deserialize(in);

            _meetings[date].push_back(meeting);
        }
    }

    in.close();
}

void CalendarWindow::saveMeetingsToFile(const std::string_view& filename)
{
    // ���� ����
    auto out = std::ofstream{ filename.data(), std::ios::binary };

    if (!out || !out.is_open())
        return;

    const size_t kMeetingsCount = _meetings.size();

    // ���̳ʸ� �����ʹ� "<<"�� �ƴ� write()�� ����.
    out.write((const char*)(&kMeetingsCount), sizeof(kMeetingsCount));

    for (const auto& [date, meetingVec] : _meetings)
    {
        out.write((const char*)(&date), sizeof(date));

        const size_t meetingsCountOnThatDate = meetingVec.size();
        out.write((const char*)(&meetingsCountOnThatDate), sizeof(meetingsCountOnThatDate));

        for (const auto& meeting : meetingVec)
        {
            meeting.Serialize(out);
        }
    }

    out.close();
}

void CalendarWindow::updateSelectedDateVariables()
{
    // https://learn.microsoft.com/ko-kr/cpp/standard-library/year-class?view=msvc-170
    // https://learn.microsoft.com/ko-kr/cpp/standard-library/month-class?view=msvc-170
    // https://learn.microsoft.com/ko-kr/cpp/standard-library/day-class?view=msvc-170
    _selectedDay   = (int)(_selectedDate.day().operator unsigned int());
    _selectedMonth = (int)(_selectedDate.month().operator unsigned int());
    _selectedYear  = (int)_selectedDate.year();
}

void CalendarWindow::Meeting::Serialize(std::ofstream& out) const
{
    const auto nameLen = name.size();

    out.write((const char*)(&nameLen), sizeof(nameLen));
    out.write(name.data(), (std::streamsize)name.size());
}

CalendarWindow::Meeting CalendarWindow::Meeting::Deserialize(std::ifstream& in)
{
    auto meeting = Meeting{ };
    auto nameLen = std::size_t{ };

    in.read((char*)&nameLen, sizeof(nameLen));
    
    // !! resize() �� �о�� �� !!
    meeting.name.resize(nameLen);
    in.read(meeting.name.data(), (std::streamsize)nameLen);

    return meeting;
}
