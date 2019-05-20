#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  if (RUN_ALL_TESTS()) {
    return 1;
  }
}
