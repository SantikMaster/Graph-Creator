// SFML_matrixtransform.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

int main() 
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        static ImVec4 circleColor{ 1.0f, 0.0f, 1.0f, 0.5f };
        static float circleRadius = 100.f;
        static std::size_t circlePoints = 30;

        sf::CircleShape shape{ circleRadius, circlePoints };
        shape.setFillColor(circleColor);

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        
        
        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
        
        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
           // viewportSize = ImGui::GetWindowSize();
          
            static auto first_time = true;
            if (first_time)
            {
                first_time = false;
        
                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
        
                // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
                //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
                //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
        
                // we now dock our windows into the docking node we made above
                ImGui::DockBuilderDockWindow("Down", dock_id_down);
                ImGui::DockBuilderDockWindow("Right", dock_id_left);
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();
        
        float val = 100;
        ImGui::Begin("Right");
        ImGui::Text("Hello, Right!");
        ImGui::VSliderFloat(" ", ImVec2(15, viewport->Size.y - 150), &val, 4, 100.);
        ImGui::End();
        float vval = 100 - val;
        
        float hval = 0;
        ImGui::Begin("Down");
        ImGui::SetNextItemWidth(viewport->Size.x - 150);
        ImGui::SliderFloat(" ", &hval, 4, 100.);
        ImGui::Text("Hello, down!");
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar;
        ImGui::ColorPicker4("Circle Color", (float*)&circleColor, flags, nullptr);
        
        ImGui::DragFloat("Circle Radius", &circleRadius);
        ImGui::DragInt("Circle Points", (int*)&circlePoints, 1.0f, 3, 500);
        ImGui::End();
        


        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();

    }

    ImGui::SFML::Shutdown();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
