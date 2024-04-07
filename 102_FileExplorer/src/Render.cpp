#include "Render.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string을 TextInput()에 적용하기 위함

#include <Windows.h>

// https://github.com/franneck94/UdemyCppGui/tree/master/2_ImGui/FileExplorer/src

void WidgetWindow::Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight)
{
    constexpr static auto kWindowFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2{ 10.0f, 10.0f });
    ImGui::SetNextWindowSize(ImVec2{ (float)systemWindowWidth - 20.0f, (float)systemWindowHeight - 20.0f });

    ImGui::Begin(windowName.c_str(), nullptr, kWindowFlags);

    drawMenu();

    ImGui::Separator();

    ImGui::BeginChild("Contents##Child", ImVec2{ 0.0f , ImGui::GetContentRegionAvail().y - 100.0f }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
        drawContents();
    }
    ImGui::EndChild();

    // 다음에 그릴 위치를 조정
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 100.0f);

    ImGui::Separator();
    
    drawActions(); // Renaming or Deleting etc

    ImGui::Separator(); 
    
    drawFilter(); // File Count

    ImGui::End();
}

void WidgetWindow::drawMenu()
{
    if (ImGui::Button("Go Up"))
    {
        // 부모 경로가 있을 때만 상위 경로로 이동하기.
        if (_currentPath.has_parent_path())
        {
            _currentPath = _currentPath.parent_path();
        }
    }

    ImGui::SameLine();

    // 경로를 string()으로 컨버팅해서 사용해야 함.
    ImGui::Text("Current Directory: %s", _currentPath.string().c_str());
}

void WidgetWindow::drawContents()
{
    /**
     * std::filesystem은 시스템의 네이티브 인코딩 방식에 따라 파일 경로를 처리함.
     * 
     * Windows 계열은 주로 UTF-16으로 인코딩하고 Unix/Linux 계열은 주로 UTF-8로 인코딩함.
     * 
     * codecvt 헤더에 있는 내용은 deprecated 상태이니 이 방법은 쓰면 안 된다.
     * 대신 운영체제 차원에서 제공하는 기능을 쓰도록 한다(WideCharToMultiByte()).
     *
     * ####################################################################################################
     * 
     * MultiByte 포맷은 "가변 길이 포맷"이고 WideChar은 "고정 길이 포맷"이다.
     * 
     * MultiByte 포맷
     * - ASCII (7비트로 표현되는 미국 표준 코드)
     * - ANSI
     * - UTF-8 (가변 길이 인코딩 방식으로 유니코드를 표현)
     * - 기타 멀티바이트 문자 인코딩 방식
     * 
     * WideChar 포맷
     * - UTF-16 (유니코드 문자를 2바이트로 표현하는 인코딩 방식)
     * - UTF-32 (유니코드 문자를 4바이트로 표현하는 인코딩 방식)
     * - wchar_t (플랫폼에 따라 다르지만 보통 UTF-16 또는 UTF-32로 인코딩됨)
     * 
     * ####################################################################################################
     * 
     * 프로젝트 속성의 "문자 집합"을 보면 "유니코드 문자 집합 사용"와 "멀티바이트 문자 집합 사용"으로 나뉜다.
     * 
     * 1. "유니코드 문자 집합 사용"을 쓰면 UTF-16을 기반으로 하는 wchar_t 문자 형식을 사용하겠다는 것을 의미.
     *    - 문자를 16비트로 인코딩하겠다는 것을 의미.
     * 
     * 2. "멀티바이트 문자 집합 사용"을 쓰면 ASCII 문자 집합을 기반으로 하는 char 문자 형식을 사용하겠다는 것을 의미.
     *    - ASCII 문자 외 다국어 문자를 처리할 수 있으며 이때 UTF-8도 포함됨.
     * 
     * 이 두 형식은 보통 Windows API로 프로그래밍을 진행할 때 큰 의미가 있음.
     * "유니코드 문자 집합 사용"을 써도 ASCII 기반 프로그래밍을 할 수 있고
     * "멀티바이트 문자 집합 사용"을 써도 UTF-16 기반으로 프로그래밍을 할 수 있다.
     * 이건 혼동되는 사항이니 일관되게 프로그래밍을 진행하는 것이 중요하다.
     */
    // 해당 경로에 있는 폴더와 파일을 포함한 모든 하위 경로를 순회함.
    // 파일 시스템은 반복자를 다룰 때 entry라는 이름을 자주 쓴다.
    for (const auto& entry : fs::directory_iterator(_currentPath))
    {
        const bool isDirectory = entry.is_directory();
        const bool isFile      = entry.is_regular_file();

        const bool isSelected = (entry.path() == _selectedEntry);

        // 하위 경로에 위치한 파일 이름만 가지고 오기
        std::wstring entryNameW = entry.path().filename().wstring();
        
        if (isDirectory)
        {
            entryNameW = L"[D]" + entryNameW;
        }
        else if (isFile)
        {
            entryNameW = L"[F]" + entryNameW;
        }

        // 버퍼 크기 계산
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, entryNameW.c_str(), -1, nullptr, 0, nullptr, nullptr);

        if (0 == utf8Len)
        {
            ImGui::Text("Failed to calculate UTF-8 string length.");

            continue;
        }

        // UTF-8 문자열을 저장할 버퍼 생성
        std::string entryName(utf8Len, '\0');

        // 변환 시작
        if (0 == WideCharToMultiByte(CP_UTF8, 0, entryNameW.c_str(), -1, entryName.data(), utf8Len, nullptr, nullptr))
        {
            ImGui::Text("Failed to convert UTF-16 to UTF-8.");

            continue;
        }

        // ImGui::Text("%s", entryName.c_str());

        if (ImGui::Selectable(entryName.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            // Directory 유형이라면 이동
            if (isDirectory)
            {
                // "/="는 경로의 새로운 부분을 추가하는 연산자이다.
                _currentPath /= entry.path().filename();
            }

            _selectedEntry = entry.path();
        }
    }
}

void WidgetWindow::drawActions()
{
    if (fs::is_directory(_selectedEntry))
    {
        ImGui::Text("Selected Dir : %s", _selectedEntry.string().c_str());
    }
    else if (fs::is_regular_file(_selectedEntry))
    {
        ImGui::Text("Selected File : %s", _selectedEntry.string().c_str());
    }
    else
    {
        ImGui::Text("Nothing Selected!");

        // 한 칸 띄우기
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.0f);
        {
            ImGui::Button("Invisible Button");
        }
        ImGui::PopStyleVar();

        return;
    }

    // --------------------------------------------------

    if (fs::is_regular_file(_selectedEntry) && ImGui::Button("Open"))
    {
        openFileWithDefaultEditor();
    }

    ImGui::SameLine();

    if (ImGui::Button("Rename"))
    {
        _isRenameDialogOpened = true;

        // 여기서 여는 것이 아닌 나중에 열겠다고 표시하는 것
        ImGui::OpenPopup("Rename File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        _isDeleteDialogOpened = true;

        // 여기서 여는 것이 아닌 나중에 열겠다고 표시하는 것
        ImGui::OpenPopup("Delete File");
    }

    renameFilePopup();
    deleteFilePopup();
}

void WidgetWindow::drawFilter()
{
    ImGui::Text("Filter by extension");

    ImGui::SameLine();

    static char extensionFilter[16]{ "\0" };

    ImGui::InputText("##inFilter", extensionFilter, sizeof(extensionFilter));

    if (0 == strlen(extensionFilter))
        return;

    size_t filteredFileCnt = 0;

    for (const auto& entry : fs::directory_iterator(_currentPath))
    {
        if (!fs::is_regular_file(entry))
            continue;

        if (entry.path().extension().string() == extensionFilter)
        {
            filteredFileCnt++;
        }
    }

    ImGui::Text("Number of files: %u", filteredFileCnt);
}

void WidgetWindow::openFileWithDefaultEditor()
{
    // 운영체제마다 기본 에디터로 여는 방식이 다르다.
#ifdef _WIN32
    // "" 이게 들어가는 게 진짜인지 확인하기 위한 코드
    const auto command = R"("start "" ")" + _selectedEntry.string() + "\"";
#elif __APPLE__
    const auto command = "open \"" + _selectedEntry.string() + "\"";
#else
    const auto command = "xdg-open \"" + _selectedEntry.string() + "\"";
#endif

    std::system(command.c_str());
}

void WidgetWindow::renameFilePopup()
{
    // OpenPopup()에 적용했던 것과 동일한 이름 지정
    if (ImGui::BeginPopupModal("Rename File", &_isRenameDialogOpened))
    {
        static char bufferName[512] = "\0";

        ImGui::Text("New name: ");
        ImGui::InputText("##newName", bufferName, sizeof(bufferName));

        if (ImGui::Button("Rename"))
        {
            // append
            fs::path newPath = _selectedEntry.parent_path() / bufferName;

            if (renameFile(_selectedEntry, newPath))
            {
                _isRenameDialogOpened = false;
                _selectedEntry = newPath;

                std::memset(bufferName, 0x00, sizeof(bufferName));
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            _isRenameDialogOpened = false;
        }

        ImGui::EndPopup();
    }
}

void WidgetWindow::deleteFilePopup()
{
    // OpenPopup()에 적용했던 것과 동일한 이름 지정
    if (ImGui::BeginPopupModal("Delete File", &_isDeleteDialogOpened))
    {
        ImGui::Text("Are you sure you want to delete %s?", _selectedEntry.filename().string().c_str());

        if (ImGui::Button("Yes"))
        {
            if (deleteFile(_selectedEntry))
            {
                // 이제 파일은 존재하지 않음.
                _selectedEntry.clear();
            }

            _isDeleteDialogOpened = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("No"))
        {
            _isDeleteDialogOpened = false;
        }

        ImGui::EndPopup();
    }
}

bool WidgetWindow::renameFile(const fs::path& oldPath, const fs::path& newPath)
{
    try
    {
        fs::rename(oldPath, newPath);

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';

        return false;
    }
}

bool WidgetWindow::deleteFile(const fs::path& path)
{
    try
    {
        fs::remove(path);

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';

        return false;
    }
}

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight)
{
    window.Draw("Window Name", systemWindowWidth, systemWindowHeight);
}
