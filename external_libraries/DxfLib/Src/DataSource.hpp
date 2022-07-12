#pragma once

#include <functional>





namespace Dxf::Parser
{





/** The data source.
This is a function that fills the specified aDestBuffer with next aSize (or fewer) bytes from the input.
The parsing functions call this repeatedly to sequentially read the entire input data.
The return value is the actual number of bytes filled, zero on EOF.
The function should report hard errors by throwing exceptions. */
using DataSource = std::function<size_t (char * /* aDestBuffer */, size_t /* aSize */)>;





/** Convenience helper that adapts std::istream into DataSource. */
DataSource dataSourceFromStdStream(std::istream & aStream);

/** Convenience helper that makes a DataSource that feed in the specified input string. */
DataSource dataSourceFromString(std::string && aInput);





}  // namespace Dxf::Parser
