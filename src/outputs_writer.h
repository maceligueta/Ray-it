#ifndef __Ray_it__OutputsWriter__
#define __Ray_it__OutputsWriter__

#include <stdio.h>
#include "mesh.h"
#include "gid_output.h"


class OutputsWriter {
public:

    OutputsWriter(){};

    void PrintResultsInGidFormat(const Mesh& mesh, const std::vector<Antenna>& antennas, const std::string& file_name, const TypeOfResultsPrint& print_type) {
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Printing results in GiD Post-process format... ";
        GidOutput gid_printer;
        gid_printer.PrintResults(mesh, antennas, file_name, print_type);
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "  done!"<<std::endl;
    }

    void PrintResultsInMatlabFormat(const Mesh& mesh, const std::vector<Antenna>& antennas, const TypeOfResultsPrint& print_type) {
        std::ofstream Xfo(CURRENT_WORKING_DIR + "/" + "X.mat");
        std::ofstream Yfo(CURRENT_WORKING_DIR + "/" + "Y.mat");
        std::ofstream Zfo(CURRENT_WORKING_DIR + "/" + "Z.mat");
        std::ofstream Vfo(CURRENT_WORKING_DIR + "/" + "V.mat");

        std::string X0buffer = "";
        std::string X1buffer = "";
        std::string X2buffer = "";
        std::string Y0buffer = "";
        std::string Y1buffer = "";
        std::string Y2buffer = "";
        std::string Z0buffer = "";
        std::string Z1buffer = "";
        std::string Z2buffer = "";
        std::string Vbuffer = "";

        std::vector<double> nodal_values(mesh.mNodes.size());
        std::vector<int> num_triangles_per_node(mesh.mNodes.size());

        const size_t num_tris = mesh.mTriangles.size();
        for(size_t i=0; i<num_tris; i++) {
            for(int j=0; j<3; ++j){
                nodal_values[mesh.mTriangles[i]->mNodeIndices[j]] += mesh.mTriangles[i]->mIntensity;
                num_triangles_per_node[mesh.mTriangles[i]->mNodeIndices[j]]++;
            }
        }
        for(size_t i=0; i<mesh.mNodes.size(); i++) {
            nodal_values[i] /= num_triangles_per_node[i];
        }

        for(int i=0; i<mesh.mRowsOfStructuredMeshNodes; i++) {
            for (int j=0; j<mesh.mColumnsOfStructuredMeshNodes; j++){
                X0buffer += std::to_string(mesh.mNodes[i*mesh.mColumnsOfStructuredMeshNodes+j][0]) + "  ";
                Y0buffer += std::to_string(mesh.mNodes[i*mesh.mColumnsOfStructuredMeshNodes+j][1]) + "  ";
                Z0buffer += std::to_string(mesh.mNodes[i*mesh.mColumnsOfStructuredMeshNodes+j][2]) + "  ";
                Vbuffer  += std::to_string(nodal_values[i*mesh.mColumnsOfStructuredMeshNodes+j]) + "  ";
            }
            X0buffer += "\n";
            Y0buffer += "\n";
            Z0buffer += "\n";
            Vbuffer += "\n";
        }

        Xfo<<X0buffer<<"\n"<<X1buffer<<"\n"<<X2buffer<<"\n";
        Yfo<<Y0buffer<<"\n"<<Y1buffer<<"\n"<<Y2buffer<<"\n";
        Zfo<<Z0buffer<<"\n"<<Z1buffer<<"\n"<<Z2buffer<<"\n";
        Vfo<<Vbuffer;
    }

};
#endif