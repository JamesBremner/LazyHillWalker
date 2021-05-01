
#include "walker.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "ParseSpaceDelimited.h"

void cWalker::read(
        const std::string &fname)
    {
        std::ifstream inf(fname);
        if (!inf.is_open())
        {
            std::cout << "cannot open " << fname << "\n";
            exit(1);
        }
        myGrid.clear();
        myRowCount = 0;
        myColCount = -1;
        std::string line;
        while (std::getline(inf, line))
        {
            std::cout << line << "\n";
            auto token = ParseSpaceDelimited(line);
            if (!token.size())
                continue;
            switch (token[0][0])
            {
            case 'h':
            {
                if (myColCount == -1)
                    myColCount = token.size() - 1;
                else if (token.size() - 1 != myColCount)
                    throw std::runtime_error("Bad column count");
                std::vector<float> row;
                for (int k = 1; k < token.size(); k++)
                    row.push_back(atof(token[k].c_str()));
                myGrid.push_back(row);
            }
            break;
            case 's':
                if (token.size() != 3)
                    throw std::runtime_error("Bad start");
                myStartCol = atoi(token[1].c_str()) - 1;
                myStartRow = atoi(token[2].c_str()) - 1;
                break;
            case 'e':
                if (token.size() != 3)
                    throw std::runtime_error("Bad end");
                myEndCol = atoi(token[1].c_str()) - 1;
                myEndRow = atoi(token[2].c_str()) - 1;
                break;
            }
        }
        myRowCount = myGrid.size();
    }

    void cWalker::ConstructBoostGraph()
    {
        //std::cout << "ConstructBoostGraph " << myColCount << " " << myRowCount << "\n";
        for (int row = 0; row < myRowCount; row++)
        {
            for (int col = 0; col < myColCount; col++)
            {
                int n = findoradd(name(col, row));
                myGraph[n].myHeight = myGrid[row][col];
            }
        }

        for (int row = 0; row < myRowCount; row++)
            for (int col = 0; col < myColCount; col++)
            {
                int n = find(name(col, row));
                if (col > 0)
                {
                    int left = find(name(col - 1, row));
                    float delta = myGrid[row][col] - myGrid[col - 1][row];
                    AddLink(n, left, 1 + delta * delta);
                }
                if (col < myColCount - 1)
                {
                    int right = find(name(col + 1, row));
                    float delta = myGrid[row][col] - myGrid[col + 1][row];
                    AddLink(n, right, 1 + delta * delta);
                }
                if (row > 0)
                {
                    int up = find(name(col, row - 1));
                    float delta = myGrid[row][col] - myGrid[col][row - 1];
                    AddLink(n, up, 1 + delta * delta);
                }
                if (row < myRowCount - 1)
                {
                    int up = find(name(col, row + 1));
                    float delta = myGrid[row][col] - myGrid[col][row + 1];
                    AddLink(n, up, 1 + delta * delta);
                }
            }
    }

    void cWalker::Path()
    {
    // run dijkstra algorithm
    int startNode = find(name(myStartCol, myStartRow));
    std::vector<int> p(num_vertices(myGraph));
    std::vector<int> vDist(num_vertices(myGraph));
    boost::dijkstra_shortest_paths(
        myGraph,
        startNode,
        weight_map(get(&cEdge::myCost, myGraph))
            .predecessor_map(boost::make_iterator_property_map(
                p.begin(), get(boost::vertex_index, myGraph)))
            .distance_map(boost::make_iterator_property_map(
                vDist.begin(), get(boost::vertex_index, myGraph))));

        // pick out path, starting at goal and finishing at start
    int goalnode = find(name(myEndCol,myEndRow));
    myPath.push_back(goalnode);
    int prev = goalnode;
    while (1)
    {
        //std::cout << prev << " " << p[prev] << ", ";
        int next = p[prev];
        myPath.push_back(next);
        if (next == startNode)
            break;
        prev = next;
    }

    // reverse so path goes from start to goal
    std::reverse(myPath.begin(), myPath.end()); 
    }

        std::string cWalker::linksText()
    {
        std::stringstream ss;
        graph_traits<graph_t>::edge_iterator ei, ei_end;
        for (tie(ei, ei_end) = edges(myGraph); ei != ei_end; ++ei)
        {
            ss << "("
               << myGraph[source(*ei, myGraph)].myName << ","
               << myGraph[target(*ei, myGraph)].myName << ","
               << myGraph[*ei].myCost
               << ") ";
        }
        ss << "\n";
        return ss.str();
    }

        std::string cWalker::pathText()
{
    std::stringstream ss;
    for (auto n : myPath)
        ss << myGraph[n].myName << " -> ";
    ss << "\n";
    return ss.str();
}

    int cWalker::findoradd(const std::string &name)
    {
        int n = find(name);
        if (n < 0)
            n = add_vertex(name, myGraph);
        return n;
    }

    int cWalker::find(const std::string &name)
    {
        for (int n = 0; n < num_vertices(myGraph); n++)
        {
            if (myGraph[n].myName == name)
            {
                return n;
            }
        }
        return -1;
    }
    void cWalker::AddLink(int n, int m, float cost)
    {
        myGraph[add_edge(n, m, myGraph).first].myCost = cost;
        myGraph[add_edge(m, n, myGraph).first].myCost = cost;
    }
    std::string cWalker::name(int col, int row)
    {
        return std::to_string(col+1) + "_" + std::to_string(row+1);
    }

