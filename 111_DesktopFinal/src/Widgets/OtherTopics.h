#pragma once

#include <string>

#include "WindowBase.h"

class OtherTopics : public WindowBase
{
public:
    OtherTopics() = default;
    virtual ~OtherTopics() = default;

public:
    void Draw(const std::string& windowName, bool* open = nullptr) override final;
};
