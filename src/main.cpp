
#include <iostream>
#include <list>
#include <sstream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"


#include "EdgesVerteces.h"
#include "Algorithms.hpp"
#include "LoadSave.hpp"


#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
int Vertex::counter;


void ImGuiWin(bool& WinVisible, bool Save, std::list<Vertex>& Verteces, std::list<Edge>& Edges, float Radius)
{
    if (Save == true)
    {
        ImGui::Begin("Save Graph to File");
    }
    else
    {
        ImGui::Begin(" Load From File");
    }
    ImGui::Text("Enter the name of the file");
    static char Name[128] = "C:/temp/graph1.xml";
    ImGui::Text("Path: ");
    ImGui::SameLine();
    ImGui::InputText("", Name, IM_ARRAYSIZE(Name));
    if (Save == true)
    {
        if (ImGui::Button("Save"))
        {
            WinVisible = false;
            std::cout << "saved\n";
            SaveGraph(Verteces, Edges, Name);
        }
    }
    else
    {
        if (ImGui::Button("Load"))
        {
            WinVisible = false;
            std::cout << "load\n";
            LoadGraph(Verteces, Edges, Name, Radius);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Close"))
    {
        WinVisible = false;
    }
    ImGui::End();
}


int main() 
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Graph Creator");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    bool SaveWinVisible = false;
    bool LoadWinVisible = false;
    bool BlockDrawing = false;

    bool DrawLineBegin = true;
    const int Radius = 20;
    static int State = 0;
    int ImGuiWindowX = 0;
    int ImGuiWindowY = 0;
    int ImGuiWindowSizeX = 0;
    int ImGuiWindowSizeY = 0;

    float OffsetX = 0, OffsetY = 0, SliderVal = 1000;
    float col[3] = {0, 1., 0};

    float CurrentColor;
    sf::Clock deltaClock;
    sf::Color Colorcur = sf::Color::Green;

    std::list<Edge>::iterator ResultEdge;
    std::list<Vertex>::iterator ResultVert;
    std::list<Vertex> Verteces;
    std::list<Edge> Edges;
    sf::Vector2f CirclePosition;
    Edge NewEdge;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                int MouseX = event.mouseButton.x - Radius;
                int MouseY = event.mouseButton.y - Radius;


                // block everything under the Imgui Window
                if (ClickOnImGui(MouseX + Radius, MouseY + Radius, ImGuiWindowX, ImGuiWindowY, ImGuiWindowSizeX, ImGuiWindowSizeY))
                    continue;
                if (SaveWinVisible || LoadWinVisible || BlockDrawing)
                {
                    BlockDrawing = false;
                    continue;
                }
                    

                switch (State)
                {
                case 0:
                    DrawLineBegin = true;
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        Vertex Vert(Radius);
                        Vert.SetPosition(MouseX+OffsetX, MouseY + OffsetY);
                        Vert.shape.setFillColor(Colorcur);
                        Verteces.push_back(Vert);
                    }
                    break;

                case 1:
                    if (DrawLineBegin)
                    {
                        if (FindVertexOnTheCLick(Verteces, ResultVert, MouseX, MouseY, Radius, CirclePosition))
                        {
                            DrawLineBegin = false;
                            NewEdge.line[0].position = sf::Vector2f(CirclePosition.x + Radius, CirclePosition.y + Radius);
                            NewEdge.StartVertex = &*ResultVert;
                            NewEdge.StartInd = (&*ResultVert)->GetIndex();
                            ResultVert->shape.setOutlineThickness(3);
                            ResultVert->shape.setOutlineColor(sf::Color::White);
                        }
                    }
                    else
                    {
                        if (ResultVert._Ptr != nullptr)
                            ResultVert->shape.setOutlineThickness(0);
                        DrawLineBegin = true;

                        if (FindVertexOnTheCLick(Verteces, ResultVert, MouseX, MouseY, Radius, CirclePosition))
                        {
                            NewEdge.line[1].position = sf::Vector2f(CirclePosition.x + Radius, CirclePosition.y + Radius);

                            NewEdge.EndVertex = &(*ResultVert);
                            NewEdge.EndInd = (&*ResultVert)->GetIndex();
                            if (NewEdge.line[1].position != NewEdge.line[0].position)
                                Edges.push_back(NewEdge);
                        }
                    }
                    break;

                case 2:
                    DrawLineBegin = true;
                    if (FindVertexOnTheCLick(Verteces, ResultVert, MouseX, MouseY, Radius, CirclePosition))
                    {
                        int index = ResultVert->GetIndex();
                        for (auto it : Edges)
                        {
                            for (auto i = Edges.begin(); i != Edges.end(); ++i)
                            {
                                if (i->EndInd == index)
                                {
                                    Edges.erase(i);
                                    break;
                                }

                            }
                        }
                        for (auto it : Edges)
                        {

                            for (auto i = Edges.begin(); i != Edges.end(); ++i)
                            {
                                if (i->StartInd == index)
                                {
                                    Edges.erase(i);
                                    break;
                                }
                            }
                        }
                        Verteces.erase(ResultVert);
                    }
                    if (FindEdgesOnTheCLick(Edges, ResultEdge, MouseX, MouseY, Radius))
                        Edges.erase(ResultEdge);
                    break;
                case 3:
                    ResultVert._Ptr = nullptr;
                    FindVertexOnTheCLick(Verteces, ResultVert, MouseX, MouseY, Radius, CirclePosition);
                    break;


                }
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (State == 3)
                {
                    ResultVert._Ptr = nullptr;

                    for (auto i = Edges.begin(); i != Edges.end(); ++i)
                    {
                        if (i->EndVertex != nullptr)
                        {
                            i->line[1].position = i->EndVertex->shape.getPosition();
                            i->line[0].position = i->StartVertex->shape.getPosition();

                            i->line[1].position.x += Radius;
                            i->line[1].position.y += Radius;
                            i->line[0].position.x += Radius;
                            i->line[0].position.y += Radius;
                        }
                    }
                }
            }
            if (State == 3)
            {
                if (event.type == sf::Event::MouseMoved)
                {
                    int MouseX = event.mouseButton.button - Radius;
                    int MouseY = event.mouseButton.x - Radius;
                    if (ResultVert._Ptr != nullptr)
                    {
                        int index = ResultVert->GetIndex();
                        ResultVert->shape.setPosition(MouseX, MouseY);
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());


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
        
        if (ImGui::BeginMainMenuBar()) 
        {
           
            if (ImGui::BeginMenu("System")) 
            {
                BlockDrawing = true;
                if (ImGui::MenuItem("SaveToFile"))
                {
                    SaveWinVisible = true;
                }
                if (ImGui::MenuItem("LoadFromFile"))
                {
                    LoadWinVisible = true;
                }
                if (ImGui::MenuItem("Exit", "CTRL+Q")) 
                {
                    exit(0);
                    BlockDrawing = false;
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Algorithms"))
            {
                BlockDrawing = true;
                if (ImGui::MenuItem("DFS Connected Components"))
                {
                    ConnectedComponent(Verteces, Edges);
                }
               
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Right");
            ImGui::Columns(2, "locations");
            ImGui::SetColumnWidth(0, 35.);
            ImGui::VSliderFloat("##labelVertical", ImVec2(15, viewport->Size.y - 150), &SliderVal, 0, 1000., "");
            ImGui::NextColumn();
            ImGuiWindowX = ImGui::GetWindowPos().x;
            ImGuiWindowY = ImGui::GetWindowPos().y;
            ImGuiWindowSizeX = ImGui::GetWindowSize().x;
            ImGuiWindowSizeY = ImGui::GetWindowSize().y;

            ImGui::RadioButton("Draw Vertex", &State, 0);            
            ImGui::RadioButton("Draw Edge", &State, 1);
            ImGui::RadioButton("Erase", &State, 2);
            ImGui::RadioButton("Edit", &State, 3);
   
    
            if (SaveWinVisible)
            {
                ImGuiWin(SaveWinVisible, true, Verteces, Edges, Radius);
            }
            if (LoadWinVisible)
            {
                ImGuiWin(LoadWinVisible, false, Verteces, Edges, Radius);
            }

        ImGui::End();
        OffsetY = 1000 - SliderVal;
        

        ImGui::Begin("Down");
        ImGui::SetNextItemWidth(viewport->Size.x - 150);
        ImGui::SliderFloat("##labelHorisontal", &OffsetX, 0, 1000., "");
        ImGui::PushItemWidth(100);
        bool result = ImGui::ColorPicker3("Color", col);
        
        Colorcur = sf::Color(col[0] * 255, col[1] * 255, col[2] * 255);

        ImGui::End();
        window.clear();
      
        for (auto& i : Verteces)
        {
            i.shape.setPosition(i.X-OffsetX, i.Y - OffsetY);
            window.draw(i.shape);
        }
        for (auto& i : Edges)
        {
            if (i.EndVertex != nullptr)
            {
                i.line[1].position = i.EndVertex->shape.getPosition();
                i.line[0].position = i.StartVertex->shape.getPosition();
                i.line[1].position.x += Radius ;
                i.line[1].position.y += Radius ;
                i.line[0].position.x += Radius ;
                i.line[0].position.y += Radius ;
            }
            window.draw(i.line);
        }
        ImGui::SFML::Render(window);
        window.display();

    }

    ImGui::SFML::Shutdown();

    return 0;
}
