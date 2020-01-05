#include <gtest/gtest.h>
#include "db/DesignDB.h"


PROJECT_NAMESPACE_BEGIN

namespace unittest
{

    class DesignDBTest : public ::testing::Test
    {
        protected:
            void SetUp() override
            {
            }
            /// @brief function to init a simple db with easy node hierarchy
            void initSimpleHierarchy();
            DesignDB _db; ///< The db under test
    };

    inline void DesignDBTest::initSimpleHierarchy()
    {
        /*
         * Simple hierarchy. 7 nodes
         * 6->5
         * 6->4
         * 5->2
         * 5->0
         * 4->0
         * 4->1
         * 2->3
         * 3->1
         * The topological ordering: 6 5 4 2 3 1 0
         * or 6 4 5 2 3 1 0
         */
        for (IndexType idx = 0; idx < 7; ++idx)
        {
            _db.allocateCkt();
        }
        auto idx = _db.subCkt(5).allocateNode();
        _db.subCkt(5).node(idx).setSubgraphIdx(2);
        idx = _db.subCkt(5).allocateNode();
        _db.subCkt(5).node(idx).setSubgraphIdx(0);
        idx = _db.subCkt(4).allocateNode();
        _db.subCkt(4).node(idx).setSubgraphIdx(0);
        idx = _db.subCkt(4).allocateNode();
        _db.subCkt(4).node(idx).setSubgraphIdx(1);
        idx = _db.subCkt(2).allocateNode();
        _db.subCkt(2).node(idx).setSubgraphIdx(3);
        idx = _db.subCkt(3).allocateNode();
        _db.subCkt(3).node(idx).setSubgraphIdx(1);
        idx = _db.subCkt(6).allocateNode();
        _db.subCkt(6).node(idx).setSubgraphIdx(5);
        idx = _db.subCkt(6).allocateNode();
        _db.subCkt(6).node(idx).setSubgraphIdx(4);
    }

    // Test whether the find root node function
    TEST_F(DesignDBTest, rootNodeTest)
    {
        initSimpleHierarchy();
        _db.findRootCkt();
        EXPECT_EQ(_db.rootCktIdx(), static_cast<IndexType>(6));
    }
} // End of the unittest namespace

PROJECT_NAMESPACE_END
