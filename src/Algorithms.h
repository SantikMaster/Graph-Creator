#pragma once
#include <list>
#include "EdgesVerteces.h"


bool DFS(std::list<Vertex>& Verteces, std::list<Edge>& Edges, Vertex* Vert, int ComponentId);
void ConnectedComponent(std::list<Vertex>& Verteces, std::list<Edge>& Edges);
bool FindEdgesOnTheCLick(const std::list<Edge>& Lines,
    std::list<Edge>::const_iterator& Result,
    int MouseX, int MouseY, int Radius);
bool FindVertexOnTheCLick(const std::list<Vertex>& Verteces,
    std::list<Vertex>::const_iterator& Result,
    int MouseX, int MouseY, int Radius, sf::Vector2f& CirclePosition);
bool ClickOnImGui(int MouseX, int  MouseY, int ImGuiWindowX, int  ImGuiWindowY, int  ImGuiWindowSizeX, int  ImGuiWindowSizeY);

