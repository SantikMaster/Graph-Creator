#pragma once
#include "EdgesVerteces.h"
int Vertex::counter;
void TextOut(const std::string& Text, const std::string& Name, const std::string& Mode = "a")
{
    // #include <stdio.h>
    FILE* stream;

    stream =
        fopen(Name.c_str(), Mode.c_str());

    char* p;
    p = (char*)Text.c_str();
    if (stream != NULL)
    {
        fprintf(stream, p, Name);
        fclose(stream);
    }
    return;
}

void SaveGraph(const std::list<Vertex>& Circles, const std::list<Edge>& Lines, const std::string& Name)
{
    TextOut("<UNITCELL name=\"graph\" dimension=\"2\">\n", Name, "w");
    for (auto& i : Circles)
    {
        int index = i.GetIndex();
        std::string Str = "<VERTEX id = \"" + std::to_string(index)
            + "\" type = \"0\"></VERTEX>\n";
        TextOut(Str, Name);
    }
    for (auto i = Lines.begin(); i != Lines.end(); ++i)
    {
        if (i->EndVertex != nullptr)
        {
            std::string Str = "<EDGE type = \"0\"><SOURCE vertex = \""
                + std::to_string(i->StartVertex->GetIndex()) +
                "\"/><TARGET vertex = \""
                + std::to_string(i->EndVertex->GetIndex()) +
                "\" offset = \"0\"/></EDGE>\n";
            TextOut(Str, Name);
        }
    }
}

void LoadGraph(std::list<Vertex>& Circles, std::list<Edge>& Edges, const std::string& Path, float Radius)
{
    std::ifstream stream(Path);

    Circles.clear();
    Edges.clear();
    std::string line;
    std::stringstream ss;
    std::string str_number, str_number_end;


    while (getline(stream, line))
    {
        if (line.find("VERTEX id") != std::string::npos)
        {
            str_number = "";
            int index = line.find_first_of("0123456789");
            for (int i = 0; i < 1000; i++)
            {
                if (!isdigit(line.at(index + i)))
                {
                    break;
                }
                str_number += line.at(index + i);
            }
            try
            {
                int VertId = stoi(str_number);
                Vertex TempVert(Radius, VertId);
                Circles.push_back(TempVert);
            }
            catch (...)
            {
                std::cout << "can't read file - vertex id";
            }
        }

        int EdgeIndex = 0;
        if ((EdgeIndex = line.find("SOURCE vertex")) != std::string::npos)
        {
            str_number = "";
            str_number_end = "";
            EdgeIndex += std::string("SOURCE vertex").length();
            int index = line.find_first_of("0123456789", EdgeIndex);
            for (int i = 0; i < 1000; i++)
            {
                if (!isdigit(line.at(index + i)))
                {
                    break;
                }
                str_number += line.at(index + i);
            }

            EdgeIndex = line.find("TARGET vertex");
            EdgeIndex += std::string("TARGET vertex").length();
            index = line.find_first_of("0123456789", EdgeIndex);
            for (int i = 0; i < 1000; i++)
            {
                if (!isdigit(line.at(index + i)))
                {
                    break;
                }
                str_number_end += line.at(index + i);
            }

            try
            {
                int EdgeStartInd = stoi(str_number);
                int EdgeEndInd = stoi(str_number_end);
                int HierarchyIndex = 0;

                Vertex* StartVert = nullptr;
                Vertex* EndVert = nullptr;
                for (auto Vert = Circles.begin(); Vert != Circles.end(); ++Vert)
                {
                    if ((Vert->GetIndex() == EdgeStartInd) && !(Vert->Ready))
                    {
                        Vert->HierarchyIndex = 1;
                        Vert->Ready = true;
                        HierarchyIndex = 1;
                        StartVert = &*Vert;
                    }
                    else if ((Vert->GetIndex() == EdgeStartInd) && (Vert->Ready))
                    {
                        HierarchyIndex = Vert->HierarchyIndex;
                        StartVert = &*Vert;
                    }
                }
                for (auto Vert = Circles.begin(); Vert != Circles.end(); ++Vert)
                {
                    if ((Vert->GetIndex() == EdgeEndInd) && !(Vert->Ready))
                    {
                        Vert->HierarchyIndex = HierarchyIndex + 1;
                        Vert->Ready = true;
                        EndVert = &*Vert;
                    }
                    else if ((Vert->GetIndex() == EdgeEndInd) && (Vert->Ready))
                    {
                        EndVert = &*Vert;
                    }
                }
                Edge NewEdge;

                NewEdge.StartVertex = StartVert;
                if (StartVert != nullptr)
                    NewEdge.StartInd = StartVert->GetIndex();
                NewEdge.EndVertex = EndVert;
                if (EndVert != nullptr)
                {
                    NewEdge.EndInd = EndVert->GetIndex();
                    Edges.push_back(NewEdge);
                }

            }
            catch (...)
            {
                std::cout << "can't read file - edges id";
            }
        }
    }
    int counter = 0;
    for (auto Vert = Circles.begin(); Vert != Circles.end(); ++Vert)
    {
        if (Vert->Ready != true)
        {
            counter++;
            Vert->HierarchyIndex = 0;
            Vert->Ready = true;
            Vert->SetPosition(100, 100 + 200 * counter + rand() % 100);
        }
    }
    int HI = 1;
    counter = 1;

    int count = 0;
    while (counter > 0)
    {
        count++;
        counter = 0;
        for (auto Vert = Circles.begin(); Vert != Circles.end(); ++Vert)
        {
            if (Vert->HierarchyIndex == HI)
            {
                counter++;
                Vert->SetPosition(100 + 200 * (Vert->HierarchyIndex), 100 + 200 * counter + rand() % 100);
            }
        }
        HI++;
    }

    for (auto Lines = Edges.begin(); Lines != Edges.end(); ++Lines)
    {
        if (&(*Lines).EndVertex == nullptr) break;

        auto StartPosition = &(*Lines).StartVertex->shape.getPosition();
        auto EndPosition = &(*Lines).EndVertex->shape.getPosition();

        (*Lines).line[0].position = sf::Vector2f(StartPosition->x + Radius, StartPosition->y + Radius);
        (*Lines).line[1].position = sf::Vector2f(EndPosition->x + Radius, EndPosition->y + Radius);
    }
}
