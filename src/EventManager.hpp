#pragma once
#include <list>

#include "imgui.h"
#include "imgui-SFML.h"

#include "EdgesVerteces.h"
#include "Algorithms.h"

#include <SFML/Window/Event.hpp>


enum CurrentState
{
    ADD_VERT = 0,
    ADD_EDGE = 1,
    ERASE = 2,
    EDIT = 3,
};
struct GraphProperties
{
    bool SaveWinVisible = false;
    bool LoadWinVisible = false;
    bool BlockDrawing = false;
    bool DrawLineBegin = true;

    const int Radius = 20;

    float OffsetX = 0, OffsetY = 0, SliderVal = 1000;
    float col[3] = { 0, 1., 0 };

    float CurrentColor;
    sf::Clock deltaClock;
    sf::Color ColorVert = sf::Color::Green;
    sf::Color ColorEdge = sf::Color::White;
    sf::Color ColorBackground = sf::Color::Black;

    ImVec2 BottomPanelSize;
    ImVec2 BottomPanelPos;
    ImVec2 RightPanelSize;
    ImVec2 RightPanelPos;

    std::list<Edge>::iterator ResultEdge;
    std::list<Vertex>::iterator ResultVert;

    Vertex* SelectedVertex = nullptr;
    Edge* SelectedEdge = nullptr;
    std::list<Vertex> Verteces;
    std::list<Edge> Edges;
    sf::Vector2f CirclePosition;
    Edge NewEdge;

    enum Selected
    {
        VERTEX,
        EDGE,
        BACKGROUND,
        NONE
    }Clicked;
    

 
    CurrentState State;
    GraphProperties()
    {
        Clicked = NONE;
        State = CurrentState::ADD_VERT;

        BottomPanelSize = ImVec2(0,0);
        BottomPanelPos = ImVec2(0, 0);
        RightPanelSize = ImVec2(0, 0);
        RightPanelPos = ImVec2(0, 0);
    }
};

void ProcessEvent(sf::RenderWindow &window, sf::Event &event, GraphProperties &Properties)
{

    if (event.type == sf::Event::Closed)
    {
        window.close();
    }
    if (event.type == sf::Event::Resized)
    {
        sf::FloatRect VisibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(VisibleArea));
    }
    if (event.type == sf::Event::MouseButtonPressed)
    {
        int MouseX = event.mouseButton.x - Properties.Radius;
        int MouseY = event.mouseButton.y - Properties.Radius;


        // block everything under the Imgui Window
        if (ClickOnImGui(MouseX + Properties.Radius, MouseY + Properties.Radius, Properties.RightPanelPos.x,
            Properties.RightPanelPos.y, Properties.RightPanelSize.x, Properties.RightPanelSize.y))
            return;
        if (ClickOnImGui(MouseX + Properties.Radius, MouseY + Properties.Radius, Properties.BottomPanelPos.x,
            Properties.BottomPanelPos.y, Properties.BottomPanelSize.x, Properties.BottomPanelSize.y))
            return;
        if (Properties.SaveWinVisible || Properties.LoadWinVisible || Properties.BlockDrawing)
        {
            Properties.BlockDrawing = false;
            return;
        }


        switch (Properties.State)
        {
        case ADD_VERT:
            Properties.DrawLineBegin = true;
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                Vertex Vert(Properties.Radius);
                Properties.ColorVert = sf::Color(Properties.col[0] * 255, Properties.col[1] * 255, Properties.col[2] * 255);
                Vert.SetPosition(MouseX + Properties.OffsetX, MouseY + Properties.OffsetY);
                Vert.shape.setFillColor(Properties.ColorVert);
                Properties.Verteces.push_back(Vert);
            }
            break;

        case ADD_EDGE:
            if (Properties.DrawLineBegin)
            {
                if (FindVertexOnTheCLick(Properties.Verteces, Properties.ResultVert, MouseX, MouseY, Properties.Radius, Properties.CirclePosition))
                {
                    Properties.DrawLineBegin = false;
                    Properties.NewEdge.line[0].position = sf::Vector2f(Properties.CirclePosition.x + Properties.Radius, Properties.CirclePosition.y + Properties.Radius);
                    Properties.NewEdge.StartVertex = &*Properties.ResultVert;
                    Properties.NewEdge.StartInd = (&*Properties.ResultVert)->GetIndex();
                    Properties.ResultVert->shape.setOutlineThickness(3);
                    Properties.ResultVert->shape.setOutlineColor(Properties.ColorEdge);
                }
            }
            else
            {
                if (Properties.ResultVert._Ptr != nullptr)
                    Properties.ResultVert->shape.setOutlineThickness(0);
                Properties.DrawLineBegin = true;

                if (FindVertexOnTheCLick(Properties.Verteces, Properties.ResultVert, MouseX, MouseY, Properties.Radius, Properties.CirclePosition))
                {
                    Properties.NewEdge.line[1].position = sf::Vector2f(Properties.CirclePosition.x + Properties.Radius, Properties.CirclePosition.y + Properties.Radius);

                    Properties.NewEdge.EndVertex = &(*Properties.ResultVert);
                    Properties.NewEdge.EndInd = (&*Properties.ResultVert)->GetIndex();

                    Properties.ColorEdge = sf::Color(Properties.col[0] * 255, Properties.col[1] * 255, Properties.col[2] * 255);
                    Properties.NewEdge.line[0].color = Properties.ColorEdge;
                    Properties.NewEdge.line[1].color = Properties.ColorEdge;
                    if (Properties.NewEdge.line[1].position != Properties.NewEdge.line[0].position)
                        Properties.Edges.push_back(Properties.NewEdge);
                }
            }
            break;

        case ERASE:
            Properties.DrawLineBegin = true;
            if (FindVertexOnTheCLick(Properties.Verteces, Properties.ResultVert, MouseX, MouseY, Properties.Radius, Properties.CirclePosition))
            {
                int index = Properties.ResultVert->GetIndex();
                for (auto it : Properties.Edges)
                {
                    for (auto i = Properties.Edges.begin(); i != Properties.Edges.end(); ++i)
                    {
                        if (i->EndInd == index)
                        {
                            Properties.Edges.erase(i);
                            break;
                        }

                    }
                }
                for (auto it : Properties.Edges)
                {

                    for (auto i = Properties.Edges.begin(); i != Properties.Edges.end(); ++i)
                    {
                        if (i->StartInd == index)
                        {
                            Properties.Edges.erase(i);
                            break;
                        }
                    }
                }
                Properties.Verteces.erase(Properties.ResultVert);
            }
            if (FindEdgesOnTheCLick(Properties.Edges, Properties.ResultEdge, MouseX, MouseY, Properties.Radius))
                Properties.Edges.erase(Properties.ResultEdge);
            break;
        case EDIT:
            Properties.ResultVert._Ptr = nullptr;
            Properties.ResultEdge._Ptr = nullptr;
            FindVertexOnTheCLick(Properties.Verteces, Properties.ResultVert, MouseX, MouseY, Properties.Radius, Properties.CirclePosition);
            FindEdgesOnTheCLick(Properties.Edges, Properties.ResultEdge, MouseX, MouseY, Properties.Radius);
            if (Properties.ResultVert._Ptr != nullptr)
            {
                Properties.Clicked = Properties.VERTEX;

                Properties.SelectedVertex = &(Properties.ResultVert._Ptr->_Myval);
                Properties.col[0] = (float)Properties.ResultVert->shape.getFillColor().r;
                Properties.col[1] = (float)Properties.ResultVert->shape.getFillColor().g;
                Properties.col[2] = (float)Properties.ResultVert->shape.getFillColor().b;

                Properties.col[0] /= 255.;
                Properties.col[1] /= 255.;
                Properties.col[2] /= 255.;

            }
            else if (Properties.ResultEdge._Ptr != nullptr)
            {
                Properties.Clicked = Properties.EDGE;

                Properties.SelectedEdge = &(Properties.ResultEdge._Ptr->_Myval);
                Properties.col[0] = (float)Properties.ResultEdge->line[0].color.r;
                Properties.col[1] = (float)Properties.ResultEdge->line[0].color.g;
                Properties.col[2] = (float)Properties.ResultEdge->line[0].color.b;

                Properties.col[0] /= 255.;
                Properties.col[1] /= 255.;
                Properties.col[2] /= 255.;
            }
            else
            {
                Properties.Clicked = Properties.BACKGROUND;
                Properties.col[0] = Properties.ColorBackground.r;
                Properties.col[1] = Properties.ColorBackground.g;
                Properties.col[2] = Properties.ColorBackground.b;

                Properties.col[0] /= 255.;
                Properties.col[1] /= 255.;
                Properties.col[2] /= 255.;
            }

            break;
        }
    }
    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (Properties.State == 3)
        {
            Properties.ResultVert._Ptr = nullptr;

            for (auto i = Properties.Edges.begin(); i != Properties.Edges.end(); ++i)
            {
                if (i->EndVertex != nullptr)
                {
                    i->line[1].position = i->EndVertex->shape.getPosition();
                    i->line[0].position = i->StartVertex->shape.getPosition();

                    i->line[1].position.x += Properties.Radius;
                    i->line[1].position.y += Properties.Radius;
                    i->line[0].position.x += Properties.Radius;
                    i->line[0].position.y += Properties.Radius;
                }
            }
        }
    }
    if (Properties.State == EDIT)
    {
        if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            int MouseX = event.mouseButton.button - Properties.Radius;
            int MouseY = event.mouseButton.x - Properties.Radius;
            if (Properties.ResultVert._Ptr != nullptr)
            {
                int index = Properties.ResultVert->GetIndex();
                Properties.ResultVert->SetPosition(MouseX + Properties.OffsetX, MouseY + Properties.OffsetY);
            }
        }
    }
}