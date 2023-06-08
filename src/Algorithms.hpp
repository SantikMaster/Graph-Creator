bool DFS(std::list<Vertex>& Verteces, std::list<Edge>& Edges, Vertex* Vert, int ComponentId)
{
    if (Vert->Visited == true)
    {
        return false;
    }
    Vert->Visited = true;
    Vert->ComponentId = ComponentId;
    for (auto Ed = Edges.begin(); Ed != Edges.end(); ++Ed)
    {
        if (Ed->EndVertex == nullptr)
        {
            continue;
        }
        if (Ed->StartInd == Vert->GetIndex())
        {
            DFS(Verteces, Edges, Ed->EndVertex, ComponentId);
        }
        if (Ed->StartVertex == nullptr)
        {
            continue;
        }
        if (Ed->EndInd == Vert->GetIndex())
        {
            DFS(Verteces, Edges, Ed->StartVertex, ComponentId);
        }
    }
    return true;
}

void ConnectedComponent(std::list<Vertex>& Verteces, std::list<Edge>& Edges)
{
    int ComponentId = 1;
    for (auto Vert = Verteces.begin(); Vert != Verteces.end(); ++Vert)
    {
        Vert->ComponentId = 0;
        Vert->Visited = false;
    }
    for (auto Vert = Verteces.begin(); Vert != Verteces.end(); ++Vert)
    {

        if (DFS(Verteces, Edges, &*Vert, ComponentId))
        {
            ComponentId++;
        }
    }
    for (auto Vert = Verteces.begin(); Vert != Verteces.end(); ++Vert)
    {
        switch (Vert->ComponentId)
        {
        case 1:
            Vert->shape.setFillColor(sf::Color::Green);
            break;
        case 2:
            Vert->shape.setFillColor(sf::Color::Blue);
            break;
        case 3:
            Vert->shape.setFillColor(sf::Color::Magenta);
            break;
        case 4:
            Vert->shape.setFillColor(sf::Color::Cyan);
            break;
        case 5:
            Vert->shape.setFillColor(sf::Color::Red);
            break;
        case 6:
            Vert->shape.setFillColor(sf::Color::Yellow);
            break;
        default:
            Vert->shape.setFillColor(sf::Color::White);
            break;
        }
    }
}
bool ClickOnImGui(int MouseX, int  MouseY, int ImGuiWindowX, int  ImGuiWindowY, int  ImGuiWindowSizeX, int  ImGuiWindowSizeY)
{
    if (MouseY < 20)
            return true;
    if ((MouseX > ImGuiWindowX && MouseX < ImGuiWindowX + ImGuiWindowSizeX) &&
        (MouseY > ImGuiWindowY && MouseY < ImGuiWindowY + ImGuiWindowSizeY)
        )
        return true;

    return false;
}

bool FindVertexOnTheCLick(const std::list<Vertex>& Verteces,
    std::list<Vertex>::const_iterator& Result,
    int MouseX, int MouseY, int Radius, sf::Vector2f& CirclePosition)
{
    for (auto it = Verteces.begin(); it != Verteces.end(); ++it)
    {
        CirclePosition = (*it).shape.getPosition();

        float SquareDistance = (CirclePosition.x - MouseX) * (CirclePosition.x - MouseX)
            + (CirclePosition.y - MouseY) * (CirclePosition.y - MouseY);
        if (SquareDistance < Radius * Radius)
        {
            Result = it;
            return true;
        }
    }
    return false;
}


bool FindEdgesOnTheCLick(const std::list<Edge>& Lines,
    std::list<Edge>::const_iterator& Result,
    int MouseX, int MouseY, int Radius)
{
    for (auto it = Lines.begin(); it != Lines.end(); ++it)
    {
        auto Bounds = (*it).line.getBounds();

        if (!Bounds.contains(MouseX + Radius, MouseY + Radius))
            continue;

        if (Bounds.height < Radius || Bounds.width < Radius)
        {

            Result = it;
            return true;
        }
        auto LineStart = (*it).line[0].position;
        auto LineEnd = (*it).line[1].position;

        if (abs((MouseX + Radius - LineStart.x) / (LineEnd.x - LineStart.x)
            - (MouseY + Radius - LineStart.y) / (LineEnd.y - LineStart.y)) < 0.1)
        {
            Result = it;
            return true;
        }
    }
    return false;
}
