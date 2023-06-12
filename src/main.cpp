
#include <iostream>
#include <list>
#include <sstream>
#include <fstream>

#include "imgui_internal.h"

#include "LoadSave.hpp"
#include "Interface.hpp"
#include "EventManager.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>




int main() 
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Graph Creator");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  
    GraphProperties Properties;
    sf::RectangleShape Wiewport;
    Wiewport.setPosition(0, 0);
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            ProcessEvent(window, event, Properties);
        }

        ImGui::SFML::Update(window, Properties.deltaClock.restart());

        BuildInterface(Properties);

        auto WindowSize = window.getSize();

        Wiewport.setSize(sf::Vector2f(WindowSize.x - Properties.RightPanelSize.x, WindowSize.y - Properties.BottomPanelSize.y));
        Wiewport.setFillColor(Properties.ColorBackground);

        window.clear();
      
        window.draw(Wiewport);
        for (auto& i : Properties.Verteces)
        {
            i.shape.setPosition(i.X- Properties.OffsetX, i.Y - Properties.OffsetY);
            window.draw(i.shape);
        }
        for (auto& i : Properties.Edges)
        {
            if (i.EndVertex != nullptr)
            {
                i.line[1].position = i.EndVertex->shape.getPosition();
                i.line[0].position = i.StartVertex->shape.getPosition();
                i.line[1].position.x += Properties.Radius ;
                i.line[1].position.y += Properties.Radius ;
                i.line[0].position.x += Properties.Radius ;
                i.line[0].position.y += Properties.Radius ;
            }
            window.draw(i.line);
        }
        ImGui::SFML::Render(window);
        window.display();

    }

    ImGui::SFML::Shutdown();

    return 0;
}
