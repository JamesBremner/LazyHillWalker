#include "walker.h"

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