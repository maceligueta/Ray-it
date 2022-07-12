// LineExtractorTest.cpp

// Tests the DxfParser class

#include "LineExtractor.hpp"
#include <sstream>
#include "TestHelpers.h"





static void testEmpty()
{
	fmt::print("Testing empty data...\n");

	auto dataSource = Dxf::Parser::dataSourceFromString("");
	Dxf::Parser::LineExtractor le(std::move(dataSource));
	TEST_THROWS(le.getNextLine(), Dxf::Parser::Error);
}





static void testSingleLfLine()
{
	fmt::print("Testing singleline (LF) data...\n");

	auto dataSource = Dxf::Parser::dataSourceFromString("SingleLine\n");
	Dxf::Parser::LineExtractor le(std::move(dataSource));
	auto line = le.getNextLine();
	TEST_EQUAL(line, "SingleLine");
	TEST_THROWS(le.getNextLine(), Dxf::Parser::Error);
}





static void testSingleCrLfLine()
{
	fmt::print("Testing singleline (CRLF) data...\n");

	auto dataSource = Dxf::Parser::dataSourceFromString("SingleLine\r\n");
	Dxf::Parser::LineExtractor le(std::move(dataSource));
	auto line = le.getNextLine();
	TEST_EQUAL(line, "SingleLine");
	TEST_THROWS(le.getNextLine(), Dxf::Parser::Error);
}





static void testMixedCrLf()
{
	fmt::print("Testing mixed CRLF and LF data...\n");

	auto dataSource = Dxf::Parser::dataSourceFromString("Line1\r\nLine2\nLine3\r\nLine4\n");
	Dxf::Parser::LineExtractor le(std::move(dataSource));
	auto line1 = le.getNextLine();
	auto line2 = le.getNextLine();
	auto line3 = le.getNextLine();
	auto line4 = le.getNextLine();
	TEST_EQUAL(line1, "Line1");
	TEST_EQUAL(line2, "Line2");
	TEST_EQUAL(line3, "Line3");
	TEST_EQUAL(line4, "Line4");
	TEST_THROWS(le.getNextLine(), Dxf::Parser::Error);
}





IMPLEMENT_TEST_MAIN("LineExtractorTest",
	testEmpty();
	testSingleLfLine();
	testSingleCrLfLine();
	testMixedCrLf();
)
