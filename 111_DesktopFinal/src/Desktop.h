#pragma once

#include <string>
#include <format>

#include <vector>

#include "WindowBase.h"

#include "Widgets/Plotter.h"
#include "Widgets/Calendar.h"
#include "Widgets/CsvEditor.h"
#include "Widgets/DiffViewer.h"
#include "Widgets/FileExplorer.h"
#include "Widgets/Paint.h"
#include "Widgets/TextEditor.h"
#include "Widgets/OtherTopics.h"

#include "Widgets/Clock.h"

#include "ImageHelper.h"

class Desktop : public WindowBase
{
public:
    struct Icon
    {
        std::string label;
        ImVec2      position;
        bool        popupOpen;

        WindowBase* base = nullptr;

        Icon(std::string_view label, WindowBase* base)
            : label(label), position(ImVec2{ }), popupOpen(false), base(base)
        { }

        void Draw(WindowBase& parent);
    };

public:
    Desktop() : _icons{}, _clock{}
    {
        _bgImageChunk = LoadImageChunk("res/img.png");

        _icons.push_back(Icon{ "Plotter", &_plotter });
        _icons.push_back(Icon{ "Calendar", &_calendar });
        _icons.push_back(Icon{ "DiffViewer", &_diffViewer });
        _icons.push_back(Icon{ "FileExplorer", &_fileExplorer });
        _icons.push_back(Icon{ "Paint", &_paint });
        _icons.push_back(Icon{ "TextEditor", &_textEditor });
        _icons.push_back(Icon{ "CsvEditor", &_csvEditor });
        _icons.push_back(Icon{ "OtherTopics", &_otherTopics });

        LoadTheme();
    }

    virtual ~Desktop()
    {
        if (nullptr != _bgImageChunk)
        {
            UnloadImageChunk(_bgImageChunk);
        }

        SaveTheme();
    }

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;

private:
    void drawBackground();
    void drawDesktop();
    void drawTaskbar();

    void showIconList(bool* open = nullptr);

private:
    std::vector<Icon> _icons;

    ImageChunk* _bgImageChunk;

    PlotterWindow _plotter;
    CalendarWindow _calendar;
    DiffViewerWindow _diffViewer;
    FileExplorerWindow _fileExplorer;
    PaintWindow _paint;
    TextEditorWindow _textEditor;
    CsvEditorWindow _csvEditor;
    OtherTopics _otherTopics;

    ClockWindow _clock;
};

void Render(Desktop& window, int systemWindowWidth, int systemWindowHeight);
