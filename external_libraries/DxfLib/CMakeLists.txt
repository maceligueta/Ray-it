cmake_minimum_required(VERSION 3.1)

project (DxfLib LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (SRCS
	Src/DataSource.cpp
	Src/DxfDrawing.cpp
	Src/DxfParser.cpp
	Src/DxfWriter.cpp
	Src/LineExtractor.cpp
)

set (HDRS
	Src/DataSource.hpp
	Src/DxfDrawing.hpp
	Src/DxfParser.hpp
	Src/DxfWriter.hpp
	Src/LineExtractor.hpp
)

add_library(DxfLib STATIC ${SRCS} ${HDRS})
target_link_libraries(DxfLib fmt-header-only)
target_include_directories(DxfLib INTERFACE Src)





# Tests:

enable_testing()

add_executable(DxfDrawingTest
	Tests/DxfDrawingTest.cpp
)
target_link_libraries(DxfDrawingTest DxfLib TestHelpers)

add_test(NAME DxfDrawingTest
	COMMAND DxfDrawingTest
)





add_executable(LineExtractorTest
	Tests/LineExtractorTest.cpp
)
target_link_libraries(LineExtractorTest DxfLib TestHelpers)

add_test(NAME LineExtractorTest
	COMMAND LineExtractorTest
)





add_executable(DxfParserTest
	Tests/DxfParserTest.cpp
)
target_link_libraries(DxfParserTest DxfLib TestHelpers)

add_test(NAME DxfParserTest
	COMMAND DxfParserTest
)





add_executable(DxfFileParser
	Tests/DxfFileParser.cpp
)
target_link_libraries(DxfFileParser DxfLib)

add_test(NAME DxfFileTest_Polyline
	COMMAND DxfFileParser ${CMAKE_CURRENT_SOURCE_DIR}/Tests/TestData/Polyline.dxf
)

add_test(NAME DxfFileTest_Polyline3D
	COMMAND DxfFileParser ${CMAKE_CURRENT_SOURCE_DIR}/Tests/TestData/Polyline3D.dxf
)





add_executable(DxfWriterTest
	Tests/DxfWriterTest.cpp
)
target_link_libraries(DxfWriterTest DxfLib TestHelpers)

add_test(NAME DxfWriterTest
	COMMAND DxfWriterTest
)
