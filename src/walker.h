#include <iostream>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>


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

/// Lazy hill walker finds path with minimum elevation changes
class cWalker
{
public:

    /** read input file
     * @param[in] fname
     */
    void read(
        const std::string &fname);
 
    /// Add orthogonal costed links between nodes
    void ConstructBoostGraph();

    /// Find cheapest path
    void Path();
    
    /// human readable link list
    std::string linksText();

    /// human readable path list
    std::string pathText();

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
    int findoradd(const std::string &name);

    int find(const std::string &name);

    void AddLink(int n, int m, float cost);

    std::string name(int col, int row);

};