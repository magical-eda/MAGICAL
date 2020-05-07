#include "global/define.h"
#include "main/IdeaPlaceEx.h"

int main(int argc, char* argv[])
{
    using namespace PROJECT_NAMESPACE;
    // File-based flow
    IdeaPlaceEx _idea;

    // Parsing in input, arguments are from system arguments
    _idea.parseFileBased(argc, argv);
    _idea.solve(-1);
    return 0;
}
