#include <gtest/gtest.h>
#include <iostream>

std::string UNITTEST_TOP_DIR; ///< Global variable to store unittest top directory

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if (argc != 2)
    {
        std::cerr << "Please specify unittest top directory" << std::endl;
        exit(0);
    }
    UNITTEST_TOP_DIR = std::string(argv[1]);

    return RUN_ALL_TESTS();
}
