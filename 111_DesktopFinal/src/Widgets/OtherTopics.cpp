#include "OtherTopics.h"

#include <imgui_internal.h>

// https://github.com/franneck94/UdemyCppGui/blob/master/2_ImGui/Final/src/OtherTopics.cpp
// Demo���� �����ִ� �����̴� �ڼ��� ���� ���� �ڵ� ����

void OtherTopics::Draw(const std::string& windowName, bool* open)
{
    ImGui::SetNextWindowPos(this->GetNextPos());
    ImGui::SetNextWindowSize(this->GetNextSize());

    ImGui::Begin(windowName.c_str(), open, kMainWindowFlags);

    if (ImGui::TreeNode("Tabbing"))
    {
        static char buf[32] = "hello";

        ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
        ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
        ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
        
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("1");
        ImGui::Text("2");
        ImGui::Text("3");
    }

    // ������ ���� �ڵ带 �ϴܿ� ����.
    // ImGuiTabBar* tabBar = nullptr;

    // ImGuiTabBarFlags_Reorderable�� �����ϸ� ���� �ٸ� ��ġ�� �̵���ų �� ����.
    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None | ImGuiTabBarFlags_Reorderable))
    {
        // ImGuiTabBar* tabBar�� ��ܿ� �������� ���.
        // tabBar = ImGui::GetCurrentTabBar();

        if (ImGui::BeginTabItem("Size 1"))
        {
            ImGui::SeparatorText("Main 11");

            ImGui::Text("1");

            ImGui::SeparatorText("Main 12");

            ImGui::Text("2");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Size 2", nullptr))
        {
            ImGui::SeparatorText("Main 21");

            ImGui::Text("1");

            ImGui::SeparatorText("Main 22");

            ImGui::Text("2");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Size 3"))
        {
            ImGui::SeparatorText("Main 31");

            ImGui::Text("1");

            ImGui::SeparatorText("Main 32");

            ImGui::Text("2");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // https://github.com/ocornut/imgui/issues/5853
    // https://github.com/ocornut/imgui/issues/2410
    // BeginTabBar()�� �ִ� ������ �����Ͽ� ������ ��.
    ImGuiID      tabID  = ImGui::GetCurrentWindow()->GetID("##tabs");
    ImGuiTabBar* tabBar = ImGui::GetCurrentContext()->TabBars.GetOrAddByKey(tabID);

    if (nullptr != tabBar)
    {
        ImGui::Separator();

        ImGui::Text("Tab Size : %d", tabBar->Tabs.size());  

        ImGuiTabItem* tabItem = ImGui::TabBarFindTabByOrder(tabBar, 0);
        {
            ImGui::Text("%s", ImGui::TabBarGetTabName(tabBar, tabItem));

            ImGui::SameLine();

            if (ImGui::Button("Focus##01"))
            {
                ImGui::TabBarQueueFocus(tabBar, tabItem);
            }
        }

        tabItem = ImGui::TabBarFindTabByOrder(tabBar, 1);
        {
            ImGui::Text("%s", ImGui::TabBarGetTabName(tabBar, tabItem));

            ImGui::SameLine();

            if (ImGui::Button("Focus##02"))
            {
                ImGui::TabBarQueueFocus(tabBar, tabItem);
            }
        }

        tabItem = ImGui::TabBarFindTabByOrder(tabBar, 2);
        {
            ImGui::Text("%s", ImGui::TabBarGetTabName(tabBar, tabItem));

            ImGui::SameLine();

            if (ImGui::Button("Focus##03"))
            {
                ImGui::TabBarQueueFocus(tabBar, tabItem);
            }
        }
    }

    ImGui::End();
}
