Limbo.Parsers.GdsiiParser {#Parsers_GdsiiParser}
---------

[TOC]

# Introduction {#Parsers_GdsiiParser_Introduction}

[GDSII](https://en.wikipedia.org/wiki/GDSII) is an industry standard for data exchange of integrated circuits (IC) layout artwork. 
It is a binary file format representing planar geometric shapes, text labels, and other information about the layout in hierarchical form. 
The manual can be found in [here](http://www.buchanan1.net/stream_description.html). 
The parser consists of three parts, reader, writer and database for full chip parsing. 
@ref GdsParser::GdsReader provides API to read GDSII files (.gds or .gds.gz with Boost and Zlib support). 
@ref GdsParser::GdsWriter provides API to write GDSII files (.gds or .gds.gz with Boost and Zlib support). 
These two parts are basic functionalities for read and write in GDSII format. 
@ref GdsParser::GdsDB is a database to store layout elements and provides easy API to read, write and flatten full layouts. 

# Examples {#Parsers_GdsiiParser_Examples}

## Stream Reader {#Parsers_GdsiiParser_StreamReader}

See documented version: [test/parsers/gdsii/test_reader.cpp](@ref gdsii/test_reader.cpp)
\include test/parsers/gdsii/test_reader.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_reader test_reader.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lgdsparser
# read a file 
./test_reader benchmarks/test_reader.gds
~~~~~~~~~~~~~~~~

If BOOST_DIR and ZLIB_DIR have been defined as environment variables when building Limbo library, one can compile with support to compression files. 
~~~~~~~~~~~~~~~~
g++ -o test_reader test_reader.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lgdsparser -L $BOOST_DIR/lib -lboost_iostreams -L $ZLIB_DIR -lz
# read a compressed file 
./test_reader benchmarks/test_reader_gz.gds.gz
~~~~~~~~~~~~~~~~

## Stream Writer {#Parsers_GdsiiParser_StreamWriter}

See documented version: [test/parsers/gdsii/test_writer.cpp](@ref gdsii/test_writer.cpp)
\include test/parsers/gdsii/test_writer.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_writer test_writer.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lgdsparser
# write to a file 
./test_writer test_writer.gds
~~~~~~~~~~~~~~~~

If BOOST_DIR and ZLIB_DIR have been defined as environment variables when building Limbo library, one can compile with support to compression files. 
~~~~~~~~~~~~~~~~
g++ -o test_writer test_writer.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lgdsparser -L $BOOST_DIR/lib -lboost_iostreams -L $ZLIB_DIR -lz
# write to a compressed file 
./test_writer test_writer_gz.gds.gz
~~~~~~~~~~~~~~~~

## Stream GDSII Database {#Parsers_GdsiiParser_StreamGdsDB}

See documented version: [test/parsers/gdsii/test_gdsdb.cpp](@ref gdsii/test_gdsdb.cpp)
\include test/parsers/gdsii/test_gdsdb.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_gdsdb test_gdsdb.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -L $LIMBO_DIR/lib -lgdsparser -lgdsdb
# read a file and test flatten 
./test_gdsdb benchmarks/test_reader.gds test_gdsdb.gds test_gdsdb_flat.gds TOPCELL
~~~~~~~~~~~~~~~~

If BOOST_DIR and ZLIB_DIR have been defined as environment variables when building Limbo library, one can compile with support to compression files. 
~~~~~~~~~~~~~~~~
g++ -o test_gdsdb test_gdsdb.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -L $LIMBO_DIR/lib -lgdsparser -lgdsdb -L $BOOST_DIR/lib -lboost_iostreams -L $ZLIB_DIR -lz
# read a compressed file and test flatten  
./test_gdsdb benchmarks/test_reader_gz.gds.gz test_gdsdb_gz.gds.gz test_gdsdb_gz_flat.gds.gz TOPCELL
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_GdsiiParser_Examples_All}

- [test/parsers/gdsii/test_reader.cpp](@ref gdsii/test_reader.cpp)
- [test/parsers/gdsii/test_writer.cpp](@ref gdsii/test_writer.cpp)
- [test/parsers/gdsii/test_gdsdb.cpp](@ref gdsii/test_gdsdb.cpp)

# References {#Parsers_GdsiiParser_References}

- [limbo/parsers/gdsii/stream/GdsRecords.h](@ref GdsRecords.h)
- [limbo/parsers/gdsii/stream/GdsReader.h](@ref GdsReader.h)
- [limbo/parsers/gdsii/stream/GdsWriter.h](@ref GdsWriter.h)
- [limbo/parsers/gdsii/stream/GdsDriver.h](@ref GdsDriver.h)
- [limbo/parsers/gdsii/gdsdb/GdsIO.h](@ref GdsIO.h)
- [limbo/parsers/gdsii/gdsdb/GdsObjects.h](@ref GdsObjects.h)
- [limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h](@ref GdsObjectHelpers.h)
