// DxfDrawingTest.cpp

// Provides basic tests for the DxfDrawing class

#include "DxfDrawing.hpp"
#include "TestHelpers.h"





/** Enable directly processing Dxf::Coords within fmt::format. */
template <>
struct fmt::formatter<Dxf::Coords>: fmt::formatter<double>
{
	// Formats the point p using the parsed format specification (presentation)
	// stored in this formatter.
	template <typename FormatContext>
	auto format(const Dxf::Coords & aCoords, FormatContext & aCtx)
	{
		return format_to(
			aCtx.out(),
			"({}, {}, {})",
			aCoords.mX, aCoords.mY, aCoords.mZ
		);
	}
};





/** Tests the creation of a simple Drawing, with at least one of each object. */
static void testCreation()
{
	Dxf::Drawing dxf;
	auto layer1 = dxf.addLayer("LAYER_1");
	auto layer2 = dxf.addLayer("LAYER_2");
	layer1->addObject(std::make_shared<Dxf::Vertex>(Dxf::Coords(1, 4)));
	layer1->addObject(std::make_shared<Dxf::Point>(Dxf::Coords(3, 2)));
	layer1->addObject(std::make_shared<Dxf::AxisAligned2DEllipse>(Dxf::Coords(5, 5), 1, 2));
	layer1->addObject(std::make_shared<Dxf::Line>(Dxf::Coords(1, 2), Dxf::Coords(3, 4)));
	layer1->addObject(std::make_shared<Dxf::Circle>(Dxf::Coords(5, 5), 1));
	layer1->addObject(std::make_shared<Dxf::Arc>(Dxf::Coords(5, 5), 2, 0, 4));
	layer1->addObject(std::make_shared<Dxf::Text>(Dxf::Coords(4, 1), "Test", 0.5));
	auto polyline = std::make_shared<Dxf::Polyline>();
	polyline->addVertex({2, 3});
	polyline->addVertex({3, 3});
	polyline->addVertex({3, 2});
	layer2->addObject(polyline);
	auto lwPolyline = std::make_shared<Dxf::LWPolyline>();
	lwPolyline->addVertex({1, 3});
	lwPolyline->addVertex({2, 3});
	lwPolyline->addVertex({2, 2});
	layer2->addObject(lwPolyline);
}





static void testDuplicateRemoval()
{
	using namespace Dxf;
	Polyline polyline;
	polyline.addVertex({0, 0});
	polyline.addVertex({0, 0});
	polyline.addVertex({1, 0});
	polyline.addVertex({1, 0});
	polyline.addVertex({1, 1});
	polyline.addVertex({1, 1});
	polyline.addVertex({0, 1});
	polyline.addVertex({0, 0});
	polyline.addVertex({0, 1});
	polyline.removeDuplicateVertices();
	TEST_EQUAL(polyline.mVertices.size(), 6);
	TEST_EQUAL(polyline.mVertices[0].mPos, Coords(0, 0));
	TEST_EQUAL(polyline.mVertices[1].mPos, Coords(1, 0));
	TEST_EQUAL(polyline.mVertices[2].mPos, Coords(1, 1));
	TEST_EQUAL(polyline.mVertices[3].mPos, Coords(0, 1));
	TEST_EQUAL(polyline.mVertices[4].mPos, Coords(0, 0));
	TEST_EQUAL(polyline.mVertices[5].mPos, Coords(0, 1));
}





IMPLEMENT_TEST_MAIN("DxfDrawingTest",
	testCreation();
	testDuplicateRemoval();
)
