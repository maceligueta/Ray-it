#include <iostream>
#include <fstream>
#include "DxfParser.hpp"




int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " filename.dxf" << std::endl;
		return 1;
	}
	std::cout << "Parsing file " << argv[1] << "..." << std::endl;
	std::ifstream f(argv[1]);
	std::shared_ptr<Dxf::Drawing> drawing;
	try
	{
		drawing = Dxf::Parser::parse(Dxf::Parser::dataSourceFromStdStream(f));
	}
	catch (const Dxf::Parser::Error & exc)
	{
		std::cerr << "Parser error at line " << exc.lineNumber() << ": " << exc.message() << std::endl;
		return 1;
	}
	catch (const std::exception & exc)
	{
		std::cerr << "Exception: " << exc.what() << std::endl;
		return 1;
	}
	std::cout << "File parsed successfully." << std::endl;
	std::cout << "Layers:" << std::endl;
	for (const auto & lay: drawing->layers())
	{
		std::cout << "  " << lay->name() << "\t(num entities: " << lay->objects().size() << ")" << std::endl;
	}
	std::cout << "All done." << std::endl;
	return 0;
}
