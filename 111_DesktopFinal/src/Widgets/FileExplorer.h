#pragma once

#include <iostream>
#include <string>
#include <codecvt>

#include <filesystem> // C++17

#include "WindowBase.h"

namespace fs = std::filesystem;

class FileExplorerWindow : public WindowBase
{
public:
    // current_path() : 현재 작업 디렉토리의 경로를 반환
    FileExplorerWindow() : _currentPath(fs::current_path()), _selectedEntry(fs::path{ })
    { }

    virtual ~FileExplorerWindow()
    { }

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;

private:
    void drawMenu();
    void drawContents();
    void drawActions();
    void drawFilter();

    void openFileWithDefaultEditor();
    void renameFilePopup();
    void deleteFilePopup();

    bool renameFile(const fs::path& oldPath, const fs::path& newPath);
    bool deleteFile(const fs::path& path);

private:
    fs::path _currentPath;
    fs::path _selectedEntry;

    bool _isRenameDialogOpened = false;
    bool _isDeleteDialogOpened = false;
};
