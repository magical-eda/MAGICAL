#include <gtest/gtest.h>
#include "db/TechDB.h"
#include "parser/ParseSimpleTech.h"

extern std::string UNITTEST_TOP_DIR;

PROJECT_NAMESPACE_BEGIN

namespace unittest
{
    /// @brief test simple tech file parser
    class TestSimpleTechParser : public::testing::Test
    {
        protected:
            void SetUp() override
            {
                testFile = UNITTEST_TOP_DIR + "./layer.simple.tech";
            }
        public:
            std::string testFile; ///< The simple tech file
    };
    TEST_F(TestSimpleTechParser, layer)
    {
        TechDB techDB;
        PROJECT_NAMESPACE::PARSE::parseSimpleTechFile(testFile, techDB);
        EXPECT_EQ(techDB.pdkLayerToDb(111), INDEX_TYPE_MAX);
        EXPECT_EQ(techDB.dbLayerToPdk(0), 1);
        EXPECT_EQ(techDB.pdkLayerToDb(1), 0);
        EXPECT_EQ(techDB.layerNameToIdx("PO"), 0);

        EXPECT_EQ(techDB.dbLayerToPdk(1), 10);
        EXPECT_EQ(techDB.pdkLayerToDb(10), 1);
        EXPECT_EQ(techDB.layerNameToIdx("CO"), 1);

        EXPECT_EQ(techDB.dbLayerToPdk(2), 11);
        EXPECT_EQ(techDB.pdkLayerToDb(11), 2);
        EXPECT_EQ(techDB.layerNameToIdx("C1"), 2);

        EXPECT_EQ(techDB.dbLayerToPdk(3), 12);
        EXPECT_EQ(techDB.pdkLayerToDb(12), 3);
        EXPECT_EQ(techDB.layerNameToIdx("C2"), 3);

        EXPECT_EQ(techDB.dbLayerToPdk(4), 13);
        EXPECT_EQ(techDB.pdkLayerToDb(13), 4);
        EXPECT_EQ(techDB.layerNameToIdx("C3"), 4);

        EXPECT_EQ(techDB.dbLayerToPdk(5), 14);
        EXPECT_EQ(techDB.pdkLayerToDb(14), 5);
        EXPECT_EQ(techDB.layerNameToIdx("C4"), 5);

        EXPECT_EQ(techDB.dbLayerToPdk(6), 21);
        EXPECT_EQ(techDB.pdkLayerToDb(21), 6);
        EXPECT_EQ(techDB.layerNameToIdx("M1"), 6);

        EXPECT_EQ(techDB.dbLayerToPdk(7), 22);
        EXPECT_EQ(techDB.pdkLayerToDb(22), 7);
        EXPECT_EQ(techDB.layerNameToIdx("M2"), 7);

        EXPECT_EQ(techDB.dbLayerToPdk(8), 23);
        EXPECT_EQ(techDB.pdkLayerToDb(23), 8);
        EXPECT_EQ(techDB.layerNameToIdx("M3"), 8);

        EXPECT_EQ(techDB.dbLayerToPdk(9), 24);
        EXPECT_EQ(techDB.pdkLayerToDb(24), 9);
        EXPECT_EQ(techDB.layerNameToIdx("M4"), 9);

        EXPECT_EQ(techDB.dbLayerToPdk(10), 25);
        EXPECT_EQ(techDB.pdkLayerToDb(25), 10);
        EXPECT_EQ(techDB.layerNameToIdx("M5"), 10);
    }
}


PROJECT_NAMESPACE_END
