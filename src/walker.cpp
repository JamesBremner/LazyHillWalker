#include <iostream>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "ParseSpaceDelimited.h"

/// edge properties
class cEdge
{
public:
    cEdge()
        : myCost(1)
    {
    }
    int myCost;
};

/// node properties
class cNode
{
public:
    cNode(const std::string &name)
        : myName(name)
    {
    }
    cNode()
        : myName("???")
    {
    }
    std::string myName;
    float myHeight;
};

/// declaration of boost graph configuration
using namespace boost;
typedef boost::adjacency_list<
    boost::listS,
    boost::vecS,
    boost::directedS,
    cNode,
    cEdge>
    graph_t;

class cWalker
{
public:
    /// read input
    void read(
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

    void ConstructBoostGraph();

    void Path();
    
    std::string linksText()
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

    std::string pathText()
{
    std::stringstream ss;
    for (auto n : myPath)
        ss << myGraph[n].myName << " -> ";
    ss << "\n";
    return ss.str();
}

private:
    int myColCount;
    int myRowCount;
    std::vector<std::vector<float>> myGrid;
    int myStartCol;
    int myStartRow;
    int myEndCol;
    int myEndRow;
    graph_t myGraph;
    std::vector<int> myPath;

    /** Find node by name
 * @return index of node found
 * 
 * If node by same name already exists, return its index
 * otherwise add a new node and return its index
 */
    int findoradd(const std::string &name)
    {
        int n = find(name);
        if (n < 0)
            n = add_vertex(name, myGraph);
        return n;
    }

    int find(const std::string &name)
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
    void AddLink(int n, int m, float cost)
    {
        myGraph[add_edge(n, m, myGraph).first].myCost = cost;
        myGraph[add_edge(m, n, myGraph).first].myCost = cost;
    }
    std::string name(int col, int row)
    {
        return std::to_string(col+1) + "_" + std::to_string(row+1);
    }
};

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

main(int argc, char *argv[])
{
    // intoduce ourself
    std::cout << "Lazy Hill Walker\n";

    // check command line
    if (argc != 2)
    {
        std::cout << "usage: walker <inputfilename>\n";
        exit(0);
    }

    // construct the walker
    cWalker walker;

    // read input file
    walker.read(argv[1]);

    // construct graph with orthogonal links costed by elevation change
    walker.ConstructBoostGraph();

    // find the minimum cost path
    walker.Path();
    std::cout 
    << "Path found: "
    << walker.pathText();
}