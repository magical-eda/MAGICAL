Limbo.Geometry {#Geometry}
---------

[TOC]

# Introduction {#Geometry_Introduction}

Geometry package provides API to adapt various geometry data structures such as [Boost.Polygon](http://www.boost.org/doc/libs/1_55_0/libs/polygon/doc/index.htm) and [Boost.Geometry](http://www.boost.org/doc/libs/1_55_0/libs/geometry/doc/html/index.html) and various specialized geomtric algorithms such as computing area of polygons, conversion of rectilinear polygon to rectangles, etc. 
It is developed in a generic way to be flexible to different data types either in STL or [Boost](http://www.boost.org). 

# Examples {#Geometry_Examples}

## Custom Geometric Classes {#Geometry_Example1}

Define custom classes for points and rectangles and use Polygon2Rectangle to convert polygon to rectangles. 

See documented version: [test/geometry/test_p2r.cpp](@ref test_p2r.cpp)
\include test/geometry/test_p2r.cpp

Compiling and running commands (assuming LIMBO_DIR is valid and limbo library has been properly installed)
~~~~~~~~~~~~~~~~
g++ -o test_p2r test_p2r.cpp -I $LIMBO_DIR/include
./test_p2r benchmarks/polygon1.gp
~~~~~~~~~~~~~~~~
Output drawn with gnuplot
![original polygon](@ref polygon1.png "doc/geometry/polygon1.png")
![test 1 solution](@ref p2r1.png "doc/geometry/p2r1.png")
![test 2 solution](@ref p2r2.png "doc/geometry/p2r2.png")
![test 3 solution](@ref p2r3.png "doc/geometry/p2r3.png")

## Boost.Polygon API {#Geometry_Example2}

Use point and rectangle types in Boost.Polygon. 

See documented version: [test/geometry/test_boostpolygonapi.cpp](@ref test_boostpolygonapi.cpp)
\include test/geometry/test_boostpolygonapi.cpp

Compiling and running commands (assuming LIMBO_DIR is valid and limbo library has been properly installed)
~~~~~~~~~~~~~~~~
g++ -o test_boostpolygonapi test_boostpolygonapi.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include
./test_boostpolygonapi benchmarks/polygon2.gp
~~~~~~~~~~~~~~~~
Output drawn with gnuplot
![original polygon](@ref polygon2.png "doc/geometry/polygon2.png")
![test 1 solution](@ref p2rboost1.png "doc/geometry/p2rboost1.png")
![test 2 solution](@ref p2rboost2.png "doc/geometry/p2rboost2.png")
![test 3 solution](@ref p2rboost3.png "doc/geometry/p2rboost3.png")

## All Examples {#Geometry_Examples_All}

Possible dependencies: 
[Boost](http://www.boost.org).

- [test/geometry/test_boostpolygonapi.cpp](@ref test_boostpolygonapi.cpp)
- [test/geometry/test_p2r.cpp](@ref test_p2r.cpp)

# References {#Geometry_References}

- [limbo/geometry/Geometry.h](@ref Geometry.h)
- [limbo/geometry/Polygon2Rectangle.h](@ref Polygon2Rectangle.h)
- [limbo/geometry/Polygon2RectangleVec.h](@ref Polygon2RectangleVec.h)
- [limbo/geometry/api/BoostPolygonApi.h](@ref BoostPolygonApi.h)
- [limbo/geometry/api/GeoBoostPolygonApi.h](@ref GeoBoostPolygonApi.h)

