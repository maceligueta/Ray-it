// DxfParserTest.cpp

// Tests the DxfParser class

#include "DxfParser.hpp"
#include <sstream>
#include "TestHelpers.h"





#define UNUSED(X) ((void)X)





static void testEmpty()
{
	auto dataSource = [](char * aDestBuffer, size_t aSize)
	{
		UNUSED(aDestBuffer);
		UNUSED(aSize);
		return 0;
	};
	auto drawing = Dxf::Parser::parse(dataSource);
}





static void testMinimal()
{
	fmt::print("Testing minimal DXF...\n");

	static const char * minimal = " \t 0 \t  \nEOF";
	std::stringstream ss(minimal);
	auto drawing = Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(ss));
	TEST_NOTNULL(drawing);
}





static void testInvalid()
{
	fmt::print("Testing invalid DXF...\n");

	static const char * invalid = "a\nEOF";
	std::stringstream ssInvalid(invalid);
	TEST_THROWS(Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(ssInvalid)), Dxf::Parser::Error);
}





static void testIncomplete()
{
	fmt::print("Testing incomplete DXF...\n");

	static const char * incomplete = "0\nE";
	std::stringstream ssIncomplete(incomplete);
	TEST_THROWS(Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(ssIncomplete)), Dxf::Parser::Error);
}





static void testLayerList()
{
	fmt::print("Testing layer list parsing...\n");

	static const char * dxf =
		"0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nVPORT\n0\nVPORT\n2\ntestVport\n0\nENDTAB\n0\nTABLE\n2\nLAYER\n"
		"0\nLAYER\n2\nLayer1\n62\n7\n"
		"0\nLAYER\n2\nLayer2\n62\n3\n"
		"0\nENDTAB\n0\nENDSEC\n0\nEOF\n";
	std::stringstream ss(dxf);
	auto layerList = Dxf::Parser::parseLayerList(Dxf::Parser::dataSourceFromStdStream(ss));
	TEST_EQUAL(layerList.size(), 2u);
	TEST_EQUAL(layerList[0], "Layer1");
	TEST_EQUAL(layerList[1], "Layer2");

	ss.seekg(0);
	auto drawing = Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(ss));
	TEST_NOTNULL(drawing);
	TEST_EQUAL(drawing->layers().size(), 2u);
	TEST_EQUAL(drawing->layers()[0]->name(), "Layer1");
	TEST_EQUAL(drawing->layers()[0]->defaultColor(), 7);
	TEST_EQUAL(drawing->layers()[1]->name(), "Layer2");
	TEST_EQUAL(drawing->layers()[1]->defaultColor(), 3);
}





static void testPolyline()
{
	fmt::print("Testing polyline parsing...\n");

	static const char * dxf =
		"0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nVPORT\n0\nVPORT\n2\ntestVport\n0\nENDTAB\n0\nTABLE\n2\nLAYER\n"
		"0\nLAYER\n2\nLayer1\n62\n7\n"
		"0\nENDTAB\n0\nENDSEC\n"
		"0\nSECTION\n2\nENTITIES\n0\nPOLYLINE\n8\nLayer1\n"
		"0\nVERTEX\n10\n \t 0.23 \n20\n1.34\n30\n2.45\n"
		"0\nVERTEX\n10\n \t 1.23 \n20\n2.34\n30\n3.45\n"
		"0\nVERTEX\n10\n \t 2.23 \n20\n3.34\n30\n4.45\n"
		"0\nVERTEX\n10\n \t 3.23 \n20\n4.34\n30\n5.45\n"
		"0\nVERTEX\n10\n \t 4.23 \n20\n5.34\n30\n6.45\n"
		"0\nSEQEND\n0\nENDSEC\n"
		"0\nEOF";
	std::stringstream ss(dxf);
	auto drawing = Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(ss));
	TEST_NOTNULL(drawing);
	TEST_EQUAL(drawing->layers().size(), 1u);
	auto layer1 = drawing->layerByName("Layer1");
	TEST_NOTNULL(layer1);
	TEST_EQUAL(layer1->name(), "Layer1");
	TEST_EQUAL(layer1->objects().size(), 1u);
	auto obj1 = layer1->objects()[0];
	TEST_EQUAL(obj1->mObjectType, Dxf::otPolyline);
	auto pl1 = std::static_pointer_cast<Dxf::Polyline>(obj1);
	TEST_EQUAL(pl1->mVertices.size(), 5u);
}





IMPLEMENT_TEST_MAIN("DxfParserTest",
	testEmpty();
	testLayerList();
	testMinimal();
	testPolyline();
	testInvalid();
	testIncomplete();
)
