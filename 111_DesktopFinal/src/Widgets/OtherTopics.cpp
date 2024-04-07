#include "OtherTopics.h"

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

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Size 1"))
        {
            ImGui::SeparatorText("Main 11");

            ImGui::Text("1");

            ImGui::SeparatorText("Main 12");

            ImGui::Text("2");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Size 2"))
        {
            ImGui::SeparatorText("Main 21");

            ImGui::Text("1");

            ImGui::SeparatorText("Main 22");

            ImGui::Text("2");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
