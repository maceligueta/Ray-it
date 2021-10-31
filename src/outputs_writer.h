#ifndef __Ray_it__OutputsWriter__
#define __Ray_it__OutputsWriter__

#include <stdio.h>
#include "mesh.h"
#include "gid_output.h"


class OutputsWriter {
public:

    OutputsWriter(){};

    void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type) {
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Printing results in GiD Post-process format... ";
        GidOutput gid_printer;
        gid_printer.PrintResults(mesh, file_name, print_type);
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "  done!"<<std::endl;
    }

};
#endif /* defined(__Ray_it__OutputsWriter__) */