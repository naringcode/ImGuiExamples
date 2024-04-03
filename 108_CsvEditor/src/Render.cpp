#include "Render.h"

#include <format>
#include <fstream>
#include <sstream>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/CsvTool/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    drawSizeButtons();

    ImGui::Separator();

    drawIoButtons();

    ImGui::Separator();

    drawTable();

    ImGui::End();
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}

void WidgetWindow::drawSizeButtons()
{
    bool userAddedRows   = false;
    bool userDroppedRows = false;
    bool userAddedCols   = false;
    bool userDroppedCols = false;

    int32_t sliderValueRows = _numRows;
    int32_t sliderValueCols = _numCols;

    ImGui::Text("Num Rows: ");
    ImGui::SameLine();

    if (ImGui::SliderInt("##numRows", &sliderValueRows, 0, kMaxNumRows))
    {
        userAddedRows   = sliderValueRows > _numRows ? true : false;
        userDroppedRows = !userAddedRows;

        _numRows = sliderValueRows;
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Row") && _numRows < kMaxNumRows)
    {
        _numRows++;
        userAddedRows = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Drop Row") && _numRows > 0)
    {
        _numRows--;
        userDroppedRows = true;
    }

    //
    ImGui::Text("Num Cols: ");
    ImGui::SameLine();

    if (ImGui::SliderInt("##numCols", &sliderValueCols, 0, kMaxNumCols))
    {
        userAddedCols   = sliderValueCols > _numCols ? true : false;
        userDroppedCols = !userAddedCols;

        _numCols = sliderValueCols;
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Col") && _numCols < kMaxNumCols)
    {
        _numCols++;
        userAddedCols = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Drop Col") && _numCols > 0)
    {
        _numCols--;
        userDroppedCols = true;
    }

    //
    const auto numRowsI32 = (std::int32_t)(_data.size());

    if (true == userAddedRows)
    {
        for (auto rowIdx = numRowsI32; rowIdx < _numRows; rowIdx++)
        {
            _data.push_back(std::vector<float>(_numCols, 0.0f));
        }
    }
    else if (true == userAddedCols)
    {
        for (auto rowIdx = 0; rowIdx < _numRows; rowIdx++)
        {
            const auto numColsI32 = (std::int32_t)(_data[rowIdx].size());

            for (auto colIdx = numColsI32; colIdx < _numCols; colIdx++)
            {
                _data[rowIdx].push_back(0.0f);
            }
        }
    }
    else if (true == userDroppedRows)
    {
        for (auto rowIdx = numRowsI32; rowIdx > _numRows; rowIdx--)
        {
            _data.pop_back();
        }
    }
    else if (true == userDroppedCols)
    {
        for (auto rowIdx = 0; rowIdx < _numRows; rowIdx++)
        {
            const auto numColsI32 = (std::int32_t)(_data[rowIdx].size());

            for (auto colIdx = numColsI32; colIdx > _numCols; colIdx--)
            {
                _data[rowIdx].pop_back();
            }
        }
    }
}

void WidgetWindow::drawIoButtons()
{
    if (ImGui::Button("Save"))
    {
        ImGui::OpenPopup("Save File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        ImGui::OpenPopup("Load File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        _numRows = 0;
        _numCols = 0;

        _data.clear();
    }

    drawSavePopup();
    drawLoadPopup();
}

void WidgetWindow::drawTable()
{
    // Table : https://github.com/ocornut/imgui/issues/3740
    // 자세한 사용법은 Demo에서 확인하도록 한다.

    // Borders Flags
    constexpr static auto kTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuter;

    static auto rowClicked = 0;
    static auto colClicked = 0;

    if (0 == _numCols)
        return;

    ImGui::BeginTable("Table", _numCols, kTableFlags);

    for (std::int32_t colIdx = 0; colIdx < _numCols; colIdx++)
    {
        const auto kColName = std::format("{}", 'A' + colIdx);

        // Optionally call TableSetupColumn() to submit column name/flags/defaults.
        // Use TableSetupColumn() to specify label, resizing policy, default width/weight, id, various other flags etc.
        // 각 Column의 속성을 정의하는 것 같다.
        ImGui::TableSetupColumn(kColName.data(), ImGuiTableColumnFlags_WidthFixed, 1280.0f / (float)_numCols);
    }

    // Optionally call TableHeadersRow() to submit a header row. Names are pulled from TableSetupColumn() data.
    // TableSetupColumn()에 넣은 label을 헤더로 가시화하는 함수.
    // ImGui::TableHeadersRow();

    // 헤더 추가하기(첫 번째 Row)
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

    for (std::int32_t colIdx = 0; colIdx < _numCols; colIdx++)
    {
        // ImGuiTableRowFlags_Headers 특성이 각 컬럼에 반영
        // TableNextColumn()를 호출하기 위한 함수
        PlotCellValue("%c", 'A' + colIdx);
    }

    // 몸체 그리기
    bool chkHovered = false;

    for (std::size_t rowIdx = 0; rowIdx < _numRows; rowIdx++)
    {
        for (std::size_t colIdx = 0; colIdx < _numCols; colIdx++)
        {
            // TableNextColumn()를 호출하기 위한 함수
            PlotCellValue("%f", _data[rowIdx][colIdx]);

            if (ImGui::IsItemClicked())
            {
                ImGui::OpenPopup("Change Value");

                rowClicked = rowIdx;
                colClicked = colIdx;
            }
            else if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Cell: (%d, %d)", rowIdx, colIdx);

                chkHovered = true;
            }
        }

        // PlotCellValue()에서 TableNextColumn()을 호출함.
        ImGui::TableNextRow();
    }

    // https://github.com/ocornut/imgui/issues/155#issuecomment-84369692
    if (true == chkHovered)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    else
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    drawValuePopup(rowClicked, colClicked);

    ImGui::EndTable();
}

void WidgetWindow::drawSavePopup()
{
    const bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    SetPopupLayout();

    if (ImGui::BeginPopupModal("Save File", nullptr, kPopupFlags))
    {
        ImGui::InputText("Filename", _filenameBuffer, sizeof(_filenameBuffer));

        if (ImGui::Button("Save", kPopupButtonSize))
        {
            saveToCsvFile(_filenameBuffer);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", kPopupButtonSize) || kEscPressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::drawLoadPopup()
{
    const bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    SetPopupLayout();

    if (ImGui::BeginPopupModal("Load File"))
    {
        ImGui::InputText("Filename", _filenameBuffer, sizeof(_filenameBuffer));

        if (ImGui::Button("Load", kPopupButtonSize))
        {
            loadFromCsvFile(_filenameBuffer);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", kPopupButtonSize) || kEscPressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::drawValuePopup(const int kRow, const int kCol)
{
    static char buffer[64]{ '\0' };

    const bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    SetPopupLayout();

    if (ImGui::BeginPopupModal("Change Value", nullptr, kPopupFlags))
    {
        const auto label = std::format("##{}_{}", kRow, kCol);

        ImGui::InputText(label.data(), buffer, sizeof(buffer));

        if (ImGui::Button("Save", kPopupButtonSize))
        {
            _data[kRow][kCol] = std::stof(buffer);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", kPopupButtonSize) || kEscPressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::saveToCsvFile(const std::string_view& filename)
{
    auto out = std::ofstream{ filename.data() };

    if (!out || false == out.is_open())
        return;

    for (std::int32_t rowIdx = 0; rowIdx < _numRows; rowIdx++)
    {
        for (std::int32_t colIdx = 0; colIdx < _numCols; colIdx++)
        {
            out << _data[rowIdx][colIdx];
            out << ',';
        }

        out << '\n';
    }

    out.close();
}

void WidgetWindow::loadFromCsvFile(const std::string_view& filename)
{
    auto in = std::ifstream{ filename.data() };

    if (!in || !in.is_open())
        return;

    _data.clear();

    auto line    = std::string{ };
    auto numRows = 0U;

    while (std::getline(in, line))
    {
        auto ss    = std::istringstream{ line };
        auto row   = std::vector<float>();
        auto value = std::string{ };

        while (std::getline(ss, value, ','))
        {
            row.push_back(std::stof(value));
        }

        _data.push_back(row);

        numRows++;
    }

    in.close();

    // _numRows = numRows;
    _numRows = _data.size();

    if (_numRows > 0)
    {
        _numCols = (int32_t)(_data[0].size());
    }
    else
    {
        _numCols = 0;
    }
}

template <typename T>
void WidgetWindow::PlotCellValue(std::string_view formatter, const T value)
{
    // 다음 컬럼으로 이동(TableNextRow()는 drawTable()에 있음)
    // 최초 그릴 때도 선행되어야 함.
    ImGui::TableNextColumn();

    ImGui::Text(formatter.data(), value);
}

void WidgetWindow::SetPopupLayout()
{
    ImVec2 popupCenterPos;
    {
        popupCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - kPopupSize.x / 2.0f;
        popupCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - kPopupSize.y / 2.0f;
    }

    ImGui::SetNextWindowPos(popupCenterPos);
    ImGui::SetNextWindowSize(kPopupSize);
}
