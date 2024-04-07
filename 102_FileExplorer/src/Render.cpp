#include "Render.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h> // std::string�� TextInput()�� �����ϱ� ����

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

    // ������ �׸� ��ġ�� ����
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
        // �θ� ��ΰ� ���� ���� ���� ��η� �̵��ϱ�.
        if (_currentPath.has_parent_path())
        {
            _currentPath = _currentPath.parent_path();
        }
    }

    ImGui::SameLine();

    // ��θ� string()���� �������ؼ� ����ؾ� ��.
    ImGui::Text("Current Directory: %s", _currentPath.string().c_str());
}

void WidgetWindow::drawContents()
{
    /**
     * std::filesystem�� �ý����� ����Ƽ�� ���ڵ� ��Ŀ� ���� ���� ��θ� ó����.
     * 
     * Windows �迭�� �ַ� UTF-16���� ���ڵ��ϰ� Unix/Linux �迭�� �ַ� UTF-8�� ���ڵ���.
     * 
     * codecvt ����� �ִ� ������ deprecated �����̴� �� ����� ���� �� �ȴ�.
     * ��� �ü�� �������� �����ϴ� ����� ������ �Ѵ�(WideCharToMultiByte()).
     *
     * ####################################################################################################
     * 
     * MultiByte ������ "���� ���� ����"�̰� WideChar�� "���� ���� ����"�̴�.
     * 
     * MultiByte ����
     * - ASCII (7��Ʈ�� ǥ���Ǵ� �̱� ǥ�� �ڵ�)
     * - ANSI
     * - UTF-8 (���� ���� ���ڵ� ������� �����ڵ带 ǥ��)
     * - ��Ÿ ��Ƽ����Ʈ ���� ���ڵ� ���
     * 
     * WideChar ����
     * - UTF-16 (�����ڵ� ���ڸ� 2����Ʈ�� ǥ���ϴ� ���ڵ� ���)
     * - UTF-32 (�����ڵ� ���ڸ� 4����Ʈ�� ǥ���ϴ� ���ڵ� ���)
     * - wchar_t (�÷����� ���� �ٸ����� ���� UTF-16 �Ǵ� UTF-32�� ���ڵ���)
     * 
     * ####################################################################################################
     * 
     * ������Ʈ �Ӽ��� "���� ����"�� ���� "�����ڵ� ���� ���� ���"�� "��Ƽ����Ʈ ���� ���� ���"���� ������.
     * 
     * 1. "�����ڵ� ���� ���� ���"�� ���� UTF-16�� ������� �ϴ� wchar_t ���� ������ ����ϰڴٴ� ���� �ǹ�.
     *    - ���ڸ� 16��Ʈ�� ���ڵ��ϰڴٴ� ���� �ǹ�.
     * 
     * 2. "��Ƽ����Ʈ ���� ���� ���"�� ���� ASCII ���� ������ ������� �ϴ� char ���� ������ ����ϰڴٴ� ���� �ǹ�.
     *    - ASCII ���� �� �ٱ��� ���ڸ� ó���� �� ������ �̶� UTF-8�� ���Ե�.
     * 
     * �� �� ������ ���� Windows API�� ���α׷����� ������ �� ū �ǹ̰� ����.
     * "�����ڵ� ���� ���� ���"�� �ᵵ ASCII ��� ���α׷����� �� �� �ְ�
     * "��Ƽ����Ʈ ���� ���� ���"�� �ᵵ UTF-16 ������� ���α׷����� �� �� �ִ�.
     * �̰� ȥ���Ǵ� �����̴� �ϰ��ǰ� ���α׷����� �����ϴ� ���� �߿��ϴ�.
     */
    // �ش� ��ο� �ִ� ������ ������ ������ ��� ���� ��θ� ��ȸ��.
    // ���� �ý����� �ݺ��ڸ� �ٷ� �� entry��� �̸��� ���� ����.
    for (const auto& entry : fs::directory_iterator(_currentPath))
    {
        const bool isDirectory = entry.is_directory();
        const bool isFile      = entry.is_regular_file();

        const bool isSelected = (entry.path() == _selectedEntry);

        // ���� ��ο� ��ġ�� ���� �̸��� ������ ����
        std::wstring entryNameW = entry.path().filename().wstring();
        
        if (isDirectory)
        {
            entryNameW = L"[D]" + entryNameW;
        }
        else if (isFile)
        {
            entryNameW = L"[F]" + entryNameW;
        }

        // ���� ũ�� ���
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, entryNameW.c_str(), -1, nullptr, 0, nullptr, nullptr);

        if (0 == utf8Len)
        {
            ImGui::Text("Failed to calculate UTF-8 string length.");

            continue;
        }

        // UTF-8 ���ڿ��� ������ ���� ����
        std::string entryName(utf8Len, '\0');

        // ��ȯ ����
        if (0 == WideCharToMultiByte(CP_UTF8, 0, entryNameW.c_str(), -1, entryName.data(), utf8Len, nullptr, nullptr))
        {
            ImGui::Text("Failed to convert UTF-16 to UTF-8.");

            continue;
        }

        // ImGui::Text("%s", entryName.c_str());

        if (ImGui::Selectable(entryName.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            // Directory �����̶�� �̵�
            if (isDirectory)
            {
                // "/="�� ����� ���ο� �κ��� �߰��ϴ� �������̴�.
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

        // �� ĭ ����
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

        // ���⼭ ���� ���� �ƴ� ���߿� ���ڴٰ� ǥ���ϴ� ��
        ImGui::OpenPopup("Rename File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        _isDeleteDialogOpened = true;

        // ���⼭ ���� ���� �ƴ� ���߿� ���ڴٰ� ǥ���ϴ� ��
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
    // �ü������ �⺻ �����ͷ� ���� ����� �ٸ���.
#ifdef _WIN32
    // "" �̰� ���� �� ��¥���� Ȯ���ϱ� ���� �ڵ�
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
    // OpenPopup()�� �����ߴ� �Ͱ� ������ �̸� ����
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
    // OpenPopup()�� �����ߴ� �Ͱ� ������ �̸� ����
    if (ImGui::BeginPopupModal("Delete File", &_isDeleteDialogOpened))
    {
        ImGui::Text("Are you sure you want to delete %s?", _selectedEntry.filename().string().c_str());

        if (ImGui::Button("Yes"))
        {
            if (deleteFile(_selectedEntry))
            {
                // ���� ������ �������� ����.
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
