#include "OtherTopics.h"

#include <imgui_internal.h>

// https://github.com/franneck94/UdemyCppGui/blob/master/2_ImGui/Final/src/OtherTopics.cpp
// Demo에도 나와있는 내용이니 자세한 것은 공식 코드 참고

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

    // 가지고 오는 코드를 하단에 적음.
    // ImGuiTabBar* tabBar = nullptr;

    // ImGuiTabBarFlags_Reorderable을 적용하면 탭을 다른 위치로 이동시킬 수 있음.
    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None | ImGuiTabBarFlags_Reorderable))
    {
        // ImGuiTabBar* tabBar를 상단에 정의했을 경우.
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
    // BeginTabBar()에 있는 내용을 참고하여 가지고 옮.
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
