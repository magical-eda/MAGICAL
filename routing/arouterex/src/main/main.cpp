#include "global/define.h"

#ifndef AROUTER_DRAFT_TEST
#include "ARouter.h"
// Core router functions
int main(int argc, char* argv[])
{
    PROJECT_NAMESPACE::ARouter()(argc, argv);
    return 0;
}

#else

// Drafts for testing
int main(int argc, char** argv)
{
    using namespace PROJECT_NAMESPACE;
    return 0;
}
#endif
