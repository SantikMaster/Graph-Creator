#pragma once
#include <iostream>
#include <list>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>

#include "imgui_internal.h"

#include "LoadSave.hpp"
#include "EventManager.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>



enum KindOfWindow
{
    SAVE_XML,
    LOAD_XML,
    SAVE_JPG,
};

void ImGuiWin(bool& WinVisible, KindOfWindow WinState, std::list<Vertex>& Verteces, std::list<Edge>& Edges,
    float Radius, sf::RenderWindow& window, GraphProperties& Properties)
{    
    static std::string StrName;
  //   char Name[128] = "C:/temp/graph1.xml";
    char Name[128] = "C:/temp/graph1.xml";

    switch (WinState)
    {
    case SAVE_XML:
        ImGui::Begin("Save Graph to File");
        ImGui::Text("Enter the name of the file");
       
        StrName = "C:/temp/graph1.xml";
        strcpy(Name, StrName.c_str());
        ImGui::Text("Path: ");
        ImGui::SameLine();

       ImGui::InputText("", Name, IM_ARRAYSIZE(Name));
        if (ImGui::Button("Save"))
        {
            WinVisible = false;
            std::cout << "saved\n";
            SaveGraph(Verteces, Edges, Name);
        }
    break;
    case LOAD_XML:
    {
        ImGui::Begin(" Load From File");
        ImGui::Text("Enter the name of the file");

        StrName = "C:/temp/graph1.xml";
        strcpy(Name, StrName.c_str());
        ImGui::Text("Path: ");
        ImGui::SameLine();
        ImGui::InputText("", Name, IM_ARRAYSIZE(Name));
        if (ImGui::Button("Load"))
        {
            WinVisible = false;
            std::cout << "load\n";
            LoadGraph(Verteces, Edges, Name, Radius);
        }
    }        
    break;
    case SAVE_JPG:
        ImGui::Begin(" Save To File");
        ImGui::Text("Enter the name of the file");
        StrName = "C:/temp/graph1.jpg";
        strcpy(Name, StrName.c_str());
        ImGui::Text("Path: ");
        ImGui::SameLine();
        ImGui::InputText("", Name, IM_ARRAYSIZE(Name));
        if (ImGui::Button("Save"))
        {
            WinVisible = false;
        
            ImGui::End();
                   using namespace std::chrono_literals;
                    std::this_thread::sleep_for(200ms);

                    std::cout << "jpg saved\n";
                    sf::Image Screenshot;
                    Screenshot = window.capture();

                    sf::Vector2u windowSize = window.getSize();
                    sf::Texture texture;

                    texture.loadFromImage(Screenshot, sf::IntRect(0, 20, windowSize.x - Properties.RightPanelSize.x - 5,
                        windowSize.y - Properties.BottomPanelSize.y - 25));

                    sf::Image ScreenshotCut = texture.copyToImage();
                    ScreenshotCut.saveToFile(Name);

         /*   std::thread th([](sf::RenderWindow& window, GraphProperties& Properties, std::string &Name)
            {

            },  window,  Properties, Name);
            th.detach();*/ 
        }
        break;
    }

    ImGui::SameLine();
    if (ImGui::Button("Close"))
    {
        WinVisible = false;
    }
    ImGui::End();
}
void BuildInterface(sf::RenderWindow& window, GraphProperties& Properties)
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    static int State;
    static bool JpgExport;
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

    if (ImGui::BeginMainMenuBar())
    {

        if (ImGui::BeginMenu("System"))
        {
            Properties.BlockDrawing = true;
            if (ImGui::MenuItem("SaveToFile"))
            {
                Properties.SaveWinVisible = true;
                JpgExport = false;
            }
            if (ImGui::MenuItem("LoadFromFile"))
            {
                Properties.LoadWinVisible = true;
            }
            if (ImGui::MenuItem("Export to jpg", "CTRL+Q"))
            {
                JpgExport = true;
                Properties.SaveWinVisible = true;
            }
            if (ImGui::MenuItem("Exit", "CTRL+Q"))
            {
                exit(0);
                Properties.BlockDrawing = false;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Algorithms"))
        {
            Properties.BlockDrawing = true;
            if (ImGui::MenuItem("DFS Connected Components"))
            {
                ConnectedComponent(Properties.Verteces, Properties.Edges);
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Right");
    ImGui::Columns(2, "locations");
    ImGui::SetColumnWidth(0, 35.);
    ImGui::VSliderFloat("##labelVertical", ImVec2(15, viewport->Size.y - Properties.BottomPanelSize.y - 50), &Properties.SliderVal, 0, 1000., "");
    ImGui::NextColumn();
    Properties.RightPanelSize = ImGui::GetWindowSize();
    Properties.RightPanelPos = ImGui::GetWindowPos();
  
    ImGui::RadioButton("Draw Vertex", &State, 0);
    ImGui::RadioButton("Draw Edge", &State, 1);
    ImGui::RadioButton("Erase", &State, 2);
    ImGui::RadioButton("Edit", &State, 3);

    Properties.State = static_cast<CurrentState>(State);

    if (Properties.SaveWinVisible)
    {
        if (!JpgExport)
        {
            ImGuiWin(Properties.SaveWinVisible, SAVE_XML, Properties.Verteces, Properties.Edges, Properties.Radius, window, Properties);
        }
       else
            ImGuiWin(Properties.SaveWinVisible, SAVE_JPG, Properties.Verteces, Properties.Edges, Properties.Radius, window, Properties);


    }
    if (Properties.LoadWinVisible)
    {
        ImGuiWin(Properties.LoadWinVisible,  LOAD_XML, Properties.Verteces, Properties.Edges, Properties.Radius, window, Properties);
    }

    ImGui::End();
    Properties.OffsetY = 1000 - Properties.SliderVal;

    ImGui::Begin("Down");

    Properties.BottomPanelSize = ImGui::GetWindowSize();
    Properties.BottomPanelPos = ImGui::GetWindowPos();
    ImGui::SetNextItemWidth(viewport->Size.x - Properties.RightPanelSize.x - 20);
    ImGui::SliderFloat("##labelHorisontal", &Properties.OffsetX, 0, 1000., "");
    ImGui::Columns(2, "locations");
    ImGui::SetColumnWidth(0, 200);
    ImGui::PushItemWidth(100);
    bool result = ImGui::ColorPicker3("Color", *&Properties.col);
    ImGui::NextColumn();

    if (State == 3)
    {
        switch (Properties.Clicked)
        {
        case Properties.VERTEX:
        {
            ImGui::Text("Vertex");
            int index = Properties.SelectedVertex->GetIndex();
            std::string VertexId = "Id " + std::to_string(index);
            ImGui::Text(VertexId.c_str());

            Properties.ColorVert = sf::Color(Properties.col[0] * 255, Properties.col[1] * 255, Properties.col[2] * 255);

            Properties.SelectedVertex->shape.setFillColor(Properties.ColorVert);
        }
        break;
        case Properties.EDGE:
        {
            ImGui::Text("Edge");
            int StartIndex = Properties.SelectedEdge->StartInd;
            int EndIndex = Properties.SelectedEdge->EndInd;
            std::string EdgeId = "From  " + std::to_string(StartIndex) +
                "  To  " + std::to_string(EndIndex);
            ImGui::Text(EdgeId.c_str());

            Properties.ColorEdge = sf::Color(Properties.col[0] * 255, Properties.col[1] * 255, Properties.col[2] * 255);

            Properties.SelectedEdge->line[0].color = Properties.ColorEdge;
            Properties.SelectedEdge->line[1].color = Properties.ColorEdge;
        }
        break;
        case Properties.BACKGROUND:
            ImGui::Text("Background");

            Properties.ColorBackground = sf::Color(Properties.col[0] * 255, Properties.col[1] * 255, Properties.col[2] * 255);

            if (ImGui::Button("Save"))
            {
                sf::Image Screenshot;
                Screenshot = window.capture();

                sf::Vector2u windowSize = window.getSize();
                sf::Texture texture;

                texture.loadFromImage(Screenshot, sf::IntRect(0,20, windowSize.x - Properties.RightPanelSize.x-5,
                             windowSize .y - Properties.BottomPanelSize.y-25));
 
                sf::Image ScreenshotCut = texture.copyToImage();
                ScreenshotCut.saveToFile("C:/temp/screenshot.jpg");
            }
            break;

        }
    }
    ImGui::End();
}