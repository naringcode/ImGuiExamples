#pragma once

#include <string>

#include <vector>

#include "WindowBase.h"

class DiffViewerWindow : public WindowBase
{
public:
    using FileContents = std::vector<std::string>;

public:
    DiffViewerWindow()
        : _filePath1("text1.txt"), _filePath2("text2.txt"),
          _fileContents1({}), _fileContents2({}),
          _diffResult1({}), _diffResult2({})
    { }

    virtual ~DiffViewerWindow()
    { }

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;

private:
    void drawSelection();
    void drawDiffView();
    void drawStatistics();

    FileContents loadFileContents(const std::string_view& filePath);
    void saveFileContents(const std::string_view& filePath, const FileContents& fileContents);
    void createDiff();


private:
    std::string _filePath1;
    std::string _filePath2;

    FileContents _fileContents1;
    FileContents _fileContents2;

    FileContents _diffResult1;
    FileContents _diffResult2;
};
