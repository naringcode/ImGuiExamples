#include "DiffViewer.h"

#include <fstream>
#include <format>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

#undef max

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/DiffViewer/src

void DiffViewerWindow::Draw(const std::string& windowName, bool* open)
{
    ImGui::SetNextWindowPos(this->GetNextPos());
    ImGui::SetNextWindowSize(this->GetNextSize());

    ImGui::Begin(windowName.c_str(), open, kMainWindowFlags);

    drawSelection();
    drawDiffView();
    drawStatistics();

    ImGui::End();
}

void DiffViewerWindow::drawSelection()
{
    ImGui::InputText("Left", &_filePath1);
    ImGui::SameLine();

    if (ImGui::Button("Save##Left"))
    {
        saveFileContents(_filePath1, _fileContents1);
    }

    ImGui::InputText("Right", &_filePath2);
    ImGui::SameLine();

    if (ImGui::Button("Save##Right"))
    {
        saveFileContents(_filePath2, _fileContents2);
    }

    if (ImGui::Button("Compare"))
    {
        _fileContents1 = loadFileContents(_filePath1);
        _fileContents2 = loadFileContents(_filePath2);

        createDiff();
    }
}

void DiffViewerWindow::drawDiffView()
{
    static constexpr float kSwapWidth = 40.0f;

    const auto kParentSize = ImVec2{ ImGui::GetContentRegionAvail().x, 500.0f };
    const auto kChildSize  = ImVec2{ kParentSize.x / 2.0f - kSwapWidth, kParentSize.y }; // 40.0f는 Swap의 영역

    const auto kSwapSize = ImVec2{ kSwapWidth, kChildSize.y };

    // Deactivate the Padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

    ImGui::BeginChild("Parent", kParentSize, ImGuiChildFlags_Border);

    ImGui::BeginChild("Diff1", kChildSize);
    {
        for (std::size_t i = 0; i < _fileContents1.size(); i++)
        {
            if (false == _diffResult1[i].empty())
            {
                ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", _fileContents1[i].data());
            }
            else
            {
                ImGui::Text("%s", _fileContents1[i].data());
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    const auto kLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const auto kButtonSize = ImVec2{ 15.0f, kLineHeight };
    
    ImGui::BeginChild("Swap", kSwapSize, ImGuiChildFlags_Border);
    {
        for (std::size_t i = 0; i < _diffResult1.size(); i++)
        {
            const auto kLeftLabel  = std::format("<##{}", i);
            const auto kRightLabel = std::format(">##{}", i);

            if (false == _diffResult1[i].empty() || false == _diffResult2[i].empty())
            {
                if (ImGui::SmallButton(kLeftLabel.data()))
                {
                    // 한쪽에 대입할 수 있어야 함
                    if (_fileContents1.size() > i && _fileContents2.size() > i)
                    {
                        _fileContents1[i] = _fileContents2[i];
                    }
                    else if (_fileContents2.size() > i)
                    {
                        _fileContents1.push_back(_fileContents2[i]);
                    }

                    createDiff();
                }

                ImGui::SameLine();

                if (ImGui::SmallButton(kRightLabel.data()))
                {
                    // 한쪽에 대입할 수 있어야 함
                    if (_fileContents1.size() > i && _fileContents2.size() > i)
                    {
                        _fileContents2[i] = _fileContents1[i];
                    }
                    else if (_fileContents1.size() > i)
                    {
                        _fileContents2.push_back(_fileContents1[i]);
                    }

                    createDiff();
                }
            }
            else
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + kLineHeight);
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Diff2", kChildSize);
    {
        for (std::size_t i = 0; i < _fileContents2.size(); i++)
        {
            if (false == _diffResult2[i].empty())
            {
                ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", _fileContents2[i].data());
            }
            else
            {
                ImGui::Text("%s", _fileContents2[i].data());
            }
        }
    }
    ImGui::EndChild();

    ImGui::EndChild(); // Parent

    ImGui::PopStyleVar();
}

void DiffViewerWindow::drawStatistics()
{
    auto diffLineCount = std::size_t{ 0 };

    for (const auto& line : _diffResult1)
    {
        if (false == line.empty())
        {
            diffLineCount++;
        }
    }

    // Last Row
    // ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 20.0f);
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y);

    ImGui::Text("Diff Line Count : %d", diffLineCount);
}

DiffViewerWindow::FileContents DiffViewerWindow::loadFileContents(const std::string_view& filePath)
{
    auto contents = FileContents{ };
    auto in = std::ifstream{ filePath.data() };

    if (in.is_open())
    {
        auto line = std::string{ };

        while (std::getline(in, line))
        {
            contents.push_back(line);
        }

        in.close();
    }

    return contents;
}

void DiffViewerWindow::saveFileContents(const std::string_view& filePath, const FileContents& fileContents)
{
    auto out = std::ofstream{ filePath.data() };

    if (out.is_open())
    {
        auto line = std::string{ };

        for (const auto& line : fileContents)
        {
            out << line << '\n';
        }

        out.close();
    }
}

void DiffViewerWindow::createDiff()
{
    _diffResult1.clear();
    _diffResult2.clear();

    const auto kMaxNumLines = std::max(_fileContents1.size(), _fileContents2.size());

    for (std::size_t i = 0; i < kMaxNumLines; i++)
    {
        const std::string line1 = i < _fileContents1.size() ? _fileContents1[i] : "EMPTY";
        const std::string line2 = i < _fileContents2.size() ? _fileContents2[i] : "EMPTY";

        if (line1 != line2)
        {
            _diffResult1.push_back(line1);
            _diffResult2.push_back(line2);
        }
        else
        {
            _diffResult1.push_back("");
            _diffResult2.push_back("");
        }
    }
}
