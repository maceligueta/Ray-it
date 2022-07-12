#pragma once

#include <string>
#include "DxfDrawing.hpp"
#include "DataSource.hpp"
#include "LineExtractor.hpp"





namespace Dxf
{
namespace Parser
{




/** Parses the DXF data from the specified data source.
Returns the DXF drawing contained within.
Throws a Dxf::Parser::Error exception upon an error.
May throw other exceptions coming from the underlying systems, such as when reading the data source. */
std::shared_ptr<Drawing> parse(DataSource && aDataSource);

/** Parses the DXF data from the specified data source, until it reads the complete layer list, then returns the names of the layers.
Is faster than the full parse, because the layer list is at the top of the file.
Throws a Dxf::Parser::Error exception upon an error.
May throw other exceptions coming from the underlying systems, such as when reading the data source. */
std::vector<std::string> parseLayerList(DataSource && aDataSource);





}  // namespace Parser
}  // namespace Dxf
