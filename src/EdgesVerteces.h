#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

class Vertex
{
    int index = 0;
public:
    bool Ready = true;
    static int counter;
    sf::CircleShape shape;
    int HierarchyIndex = 0;

    // DFS depth first search
    bool Visited = false;
    int ComponentId = 0;
    int X, Y;
 
    Vertex(float Radius)
    {
        index = ++counter;
        shape.setRadius(Radius);
        shape.setFillColor(sf::Color::Green);
    }
    Vertex(float Radius, int Id)
    {
        index = Id;
        if (counter < Id) counter = Id;
        shape.setRadius(Radius);
        shape.setFillColor(sf::Color::Green);

        Ready = false;
    }
    void SetPosition(int x, int y)
    {
        X = x;
        Y = y;
        shape.setPosition(X, Y);
    }

    bool operator == (const Vertex& c) 
    {
        if (index == c.index)
            return true;
        return false;
    }
    
    int GetIndex() const
    {
        return index;
    }
};
class Edge
{
public:
    sf::VertexArray line;
    Vertex* EndVertex = nullptr;
    Vertex* StartVertex = nullptr;
    int StartInd = 0;
    int EndInd = 0;


    Edge()
    {
        line.setPrimitiveType(sf::LinesStrip);
        line.append(sf::Vector2f(10, 0));
        line.append(sf::Vector2f(200, 200));
    }
};
