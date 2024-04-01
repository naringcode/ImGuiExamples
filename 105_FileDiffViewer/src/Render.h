#pragma once

#include <string>

#include <vector>

class WidgetWindow
{
public:
    using FileContents = std::vector<std::string>;

public:
    WidgetWindow()
        : _filePath1("text1.txt"), _filePath2("text2.txt"),
          _fileContents1({}), _fileContents2({}),
          _diffResult1({}), _diffResult2({})
    { }

public:
    void Draw(const std::string& windowName, int systemWindowWidth, int systemWindowHeight);

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

void Render(WidgetWindow& window, int systemWindowWidth, int systemWindowHeight);
