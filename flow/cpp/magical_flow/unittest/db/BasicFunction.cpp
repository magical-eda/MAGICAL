#include <gtest/gtest.h>
#include "global/global.h"

extern std::string UNITTEST_TOP_DIR;

PROJECT_NAMESPACE_BEGIN

namespace unittest
{
    TEST (OrientCnvTest, North)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::northCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 5), ::PROJECT_NAMESPACE::MfUtil::northCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 205), ::PROJECT_NAMESPACE::MfUtil::northCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 205), ::PROJECT_NAMESPACE::MfUtil::northCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, South)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(103, 205), ::PROJECT_NAMESPACE::MfUtil::southCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 205), ::PROJECT_NAMESPACE::MfUtil::southCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 5), ::PROJECT_NAMESPACE::MfUtil::southCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::southCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, West)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(203, 5), ::PROJECT_NAMESPACE::MfUtil::westCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 105), ::PROJECT_NAMESPACE::MfUtil::westCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::westCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 105), ::PROJECT_NAMESPACE::MfUtil::westCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, East)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(3, 105), ::PROJECT_NAMESPACE::MfUtil::eastCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::eastCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 105), ::PROJECT_NAMESPACE::MfUtil::eastCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 5), ::PROJECT_NAMESPACE::MfUtil::eastCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, FlipNorth)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(103, 5), ::PROJECT_NAMESPACE::MfUtil::flipNorthCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::flipNorthCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 205), ::PROJECT_NAMESPACE::MfUtil::flipNorthCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 205), ::PROJECT_NAMESPACE::MfUtil::flipNorthCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, FlipSouth)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(3, 205), ::PROJECT_NAMESPACE::MfUtil::flipSouthCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 205), ::PROJECT_NAMESPACE::MfUtil::flipSouthCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::flipSouthCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(103, 5), ::PROJECT_NAMESPACE::MfUtil::flipSouthCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, FlipWest)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::flipWestCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 105), ::PROJECT_NAMESPACE::MfUtil::flipWestCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 5), ::PROJECT_NAMESPACE::MfUtil::flipWestCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 105), ::PROJECT_NAMESPACE::MfUtil::flipWestCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, FlipEast)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ll = XY<LocType>(0 , 0);
        XY<LocType> lr = XY<LocType>(100, 0);
        XY<LocType> ul = XY<LocType>(0, 200);
        XY<LocType> ur = XY<LocType>(100, 200);

        EXPECT_EQ(XY<LocType>(203, 105), ::PROJECT_NAMESPACE::MfUtil::flipEastCoordinate(ll, offset, bbox));
        EXPECT_EQ(XY<LocType>(203, 5), ::PROJECT_NAMESPACE::MfUtil::flipEastCoordinate(lr, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 105), ::PROJECT_NAMESPACE::MfUtil::flipEastCoordinate(ul, offset, bbox));
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::flipEastCoordinate(ur, offset, bbox));
    }

    TEST (OrientCnvTest, URTest)
    {
        Box<LocType> bbox = Box<LocType>(0, 0, 100, 200);
        auto offset = XY<LocType>(3, 5);
        XY<LocType> ur = XY<LocType>(100, 200);

        // N
        EXPECT_EQ(XY<LocType>(103, 205), ::PROJECT_NAMESPACE::MfUtil::northCoordinate(ur, offset, bbox));
        // S
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::southCoordinate(ur, offset, bbox));
        // W
        EXPECT_EQ(XY<LocType>(3, 105), ::PROJECT_NAMESPACE::MfUtil::westCoordinate(ur, offset, bbox));
        // E
        EXPECT_EQ(XY<LocType>(203, 5), ::PROJECT_NAMESPACE::MfUtil::eastCoordinate(ur, offset, bbox));
        // FN
        EXPECT_EQ(XY<LocType>(3, 205), ::PROJECT_NAMESPACE::MfUtil::flipNorthCoordinate(ur, offset, bbox));
        // FS
        EXPECT_EQ(XY<LocType>(103, 5), ::PROJECT_NAMESPACE::MfUtil::flipSouthCoordinate(ur, offset, bbox));
        // FW
        EXPECT_EQ(XY<LocType>(203, 105), ::PROJECT_NAMESPACE::MfUtil::flipWestCoordinate(ur, offset, bbox));
        // FE
        EXPECT_EQ(XY<LocType>(3, 5), ::PROJECT_NAMESPACE::MfUtil::flipEastCoordinate(ur, offset, bbox));

    }
}

PROJECT_NAMESPACE_END
