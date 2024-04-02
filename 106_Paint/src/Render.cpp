#include "Render.h"

#include <fstream>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/Paint/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);
    
    drawMenu();
    drawCanvas();

    ImGui::End();
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}

void WidgetWindow::drawMenu()
{
    /**
     * 104_TextEditor에서 했던 것과 동일함.
     */
    const bool kCtrlPressed = ImGui::GetIO().KeyCtrl;
    const bool kSPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S));
    const bool kLPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_L));
    // const bool kCPressed    = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C));

    if (ImGui::Button("Save") || (kCtrlPressed && kSPressed))
    {
        ImGui::OpenPopup("Save Image");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load") || (kCtrlPressed && kLPressed))
    {
        ImGui::OpenPopup("Load Image");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        clearCanvas();
    }

    drawColorButtons();
    drawSizeSettings();

    drawSavePopup();
    drawLoadPopup();
}

void WidgetWindow::drawCanvas()
{
    // _canvasPos = ImGui::GetCursorPos(); // 전체화면으로 그렸을 때에만 동작
    _canvasPos = ImGui::GetCursorScreenPos(); // 절대좌표로 반환함

    const float kBorderThickness = 1.5f;
    const auto  kButtonSize = ImVec2{ _canvasSize.x + 2.0f * kBorderThickness, 
                                      _canvasSize.y + 2.0f * kBorderThickness }; // 캔버스 크기 + 경계 부분의 크기

    // ImGui::Button("##canvas", kButtonSize);
    ImGui::InvisibleButton("##canvas", kButtonSize);

    // 보이지 않는 버튼을 눌렀을 때 위치를 트랙킹할 수 있어야 함.
    const auto kMousePos        = ImGui::GetMousePos();
    const auto kIsMouseHovering = ImGui::IsItemHovered();

    if (kIsMouseHovering && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        // 캔버스 내 순수한 상대적인 위치만 계산(보이지 않는 버튼은 캔버스의 크기 + 경계 부분의 크기 합임)
        const auto kPoint = ImVec2(kMousePos.x - _canvasPos.x - kBorderThickness,
                                   kMousePos.y - _canvasPos.y - kBorderThickness);

        // const auto kPoint = ImVec2(kMousePos.x - _canvasPos.x,
        //                            kMousePos.y - _canvasPos.y);

        _points.push_back({ kPoint, _currentDrawColor, _pointDrawSize });
    }

    auto drawList = ImGui::GetWindowDrawList();

    for (const auto& [point, color, size] : _points)
    {
        // 캔버스의 위치에 그려야 할 위치를 더하는 것으로 보정
        const auto kPos = ImVec2(_canvasPos.x + kBorderThickness + point.x,
                                 _canvasPos.y + kBorderThickness + point.y);

        // const auto kPos = ImVec2(_canvasPos.x + point.x,
        //                          _canvasPos.y + point.y);

        drawList->AddCircleFilled(kPos, size, color);
    }

    // 경계 부분 그리기
    const auto kBorderMin = _canvasPos;
    const auto kBorderMax = ImVec2{ _canvasPos.x + kButtonSize.x - kBorderThickness,
                                    _canvasPos.y + kButtonSize.y - kBorderThickness };

    drawList->AddRect(kBorderMin, kBorderMax, IM_COL32(255, 255, 255, 255), 0.0f, ImDrawFlags_RoundCornersAll, kBorderThickness);
}

void WidgetWindow::drawColorButtons()
{
    static constexpr auto kOrange = ImVec4{ 1.0f, 0.5f, 0.0f, 1.0f };

    const bool kIsSelectedRed = (_currentDrawColor == ImColor{ 255, 0, 0 });
    const bool kIsSelectedGreen = (_currentDrawColor == ImColor{ 0, 255, 0 });
    const bool kIsSelectedBlue = (_currentDrawColor == ImColor{ 0, 0, 255 });
    const bool kIsSelectedWhite = (_currentDrawColor == ImColor{ 255, 255, 255 });

    const auto kNonePresetColor = (!kIsSelectedRed && !kIsSelectedGreen && !kIsSelectedBlue && !kIsSelectedWhite);

    if (kIsSelectedRed)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
    }

    if (ImGui::Button("Red"))
    {
        _currentDrawColor = ImColor{ 255, 0, 0 };
    }

    if (kIsSelectedRed)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (kIsSelectedGreen)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
    }

    if (ImGui::Button("Green"))
    {
        _currentDrawColor = ImColor{ 0, 255, 0 };
    }

    if (kIsSelectedGreen)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (kIsSelectedBlue)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
    }

    if (ImGui::Button("Blue"))
    {
        _currentDrawColor = ImColor{ 0, 0, 255 };
    }

    if (kIsSelectedBlue)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (kIsSelectedWhite)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
    }

    if (ImGui::Button("White"))
    {
        _currentDrawColor = ImColor{ 255, 255, 255 };
    }

    if (kIsSelectedWhite)
    {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    if (kNonePresetColor)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
    }

    // ImGui::ColorEdit3("##picker", reinterpret_cast<float*>(&_currentDrawColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    
    if (ImGui::Button("Choose"))
    {
        ImGui::OpenPopup("Color Picker");
    }
    
    // BeginPopupModal()은 새로운 창을 만드는 느낌이 강하다.
    if (ImGui::BeginPopup("Color Picker"))
    {
        // ColorEdit과 ColorPicker의 차이는 직접 주석처리해서 실행하는 방식으로 확인할 것
        ImGui::ColorPicker3("##picker", reinterpret_cast<float*>(&_currentDrawColor));
    
        ImGui::EndPopup();
    }

    if (kNonePresetColor)
    {
        ImGui::PopStyleColor();
    }
}

void WidgetWindow::drawSizeSettings()
{
    ImGui::Text("Draw Size");
    ImGui::SameLine();

    // 여분의 Canvas 크기까지를 Width로 설정
    ImGui::PushItemWidth(_canvasSize.x - ImGui::GetCursorPosX());
    {
        ImGui::SliderFloat("##drawSize", &_pointDrawSize, 1.0f, 10.0f);
    }
    ImGui::PopItemWidth();
}

void WidgetWindow::drawSavePopup()
{
    const bool kEscPressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    ImVec2 popupCenterPos;
    {
        popupCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - kPopupSize.x / 2.0f;
        popupCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - kPopupSize.y / 2.0f;
    }

    ImGui::SetNextWindowPos(popupCenterPos);
    ImGui::SetNextWindowSize(kPopupSize);

    if (ImGui::BeginPopupModal("Save Image", nullptr, kPopupFlags))
    {
        ImGui::InputText("Filename", _filenameBuffer, sizeof(_filenameBuffer));

        if (ImGui::Button("Save", kPopupButtonSize))
        {
            saveToImageFile(_filenameBuffer);

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

    ImVec2 popupCenterPos;
    {
        popupCenterPos.x = ImGui::GetIO().DisplaySize.x / 2.0f - kPopupSize.x / 2.0f;
        popupCenterPos.y = ImGui::GetIO().DisplaySize.y / 2.0f - kPopupSize.y / 2.0f;
    }

    ImGui::SetNextWindowPos(popupCenterPos);
    ImGui::SetNextWindowSize(kPopupSize);

    if (ImGui::BeginPopupModal("Load Image"))
    {
        ImGui::InputText("Filename", _filenameBuffer, sizeof(_filenameBuffer));

        if (ImGui::Button("Load", kPopupButtonSize))
        {
            loadFromImageFile(_filenameBuffer);

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

void WidgetWindow::saveToImageFile(std::string_view filename)
{
    // 파일 열기
    auto out = std::ofstream{ filename.data(), std::ios::binary };

    if (!out || !out.is_open())
        return;

    const size_t kPointCount = _points.size();

    // 바이너리 데이터는 "<<"가 아닌 write()로 쓴다.
    out.write((const char*)(&kPointCount), sizeof(kPointCount));

    for (const auto& [point, color, size] : _points)
    {
        out.write((const char*)(&point), sizeof(point));
        out.write((const char*)(&color), sizeof(color));
        out.write((const char*)(&size), sizeof(size));
    }

    out.close();
}

void WidgetWindow::loadFromImageFile(std::string_view filename)
{
    // 파일 읽기
    auto in = std::ifstream{ filename.data(), std::ios::binary }; // binary로 개방

    if (!in || !in.is_open())
        return;

    size_t pointCount = std::size_t{ 0 };

    in.read((char*)(&pointCount), sizeof(pointCount));

    for (std::size_t i = 0; i < pointCount; i++)
    {
        auto point = ImVec2{ };
        auto color = ImColor{ };
        auto size  = float{ };

        in.read((char*)(&point), sizeof(point));
        in.read((char*)(&color), sizeof(color));
        in.read((char*)(&size),  sizeof(size));

        _points.push_back({ point, color, size });
    }

    in.close();
}

void WidgetWindow::clearCanvas()
{
    _points.clear();
}
