#ifndef __Ray_itgid_output
#define __Ray_itgid_output

#include <fstream>
#include <iomanip>
#include "constants.h"

class GidOutput {

    public:

    GidOutput(){}
    static void PrintResults(const Mesh& mesh, const std::vector<Antenna>& antennas, const std::string& file_name, const TypeOfResultsPrint& print_type=RESULTS_ON_NODES) {
        std::ofstream gidmshfo(file_name + ".post.msh");
        std::ofstream gidresfo(file_name + ".post.res");

        gidmshfo <<std::setprecision(8);
        gidmshfo <<"MESH "<<"dimension "<<3<<" ElemType Triangle Nnode 3\n";
        gidmshfo <<"Coordinates\n";
        gidmshfo <<"#node number   coord_x   coord_y coord_z";
        gidmshfo <<" \n";

        const size_t num_nodes = mesh.mNodes.size();
        for(size_t i=0; i<num_nodes; i++) {
            const Vec3& node_i = mesh.mNodes[i];
            gidmshfo << i+1<<"   "<<node_i[0]<<"   "<<node_i[1]<< "   "<<node_i[2]<<"\n";
        }

        for(size_t i=0; i<antennas.size(); i++) {
            const Vec3& node_i = antennas[i].mCoordinates;
            gidmshfo << i+num_nodes+1<<"   "<<node_i[0]<<"   "<<node_i[1]<< "   "<<node_i[2]<<"\n";
        }

        gidmshfo <<"end coordinates\n\n";

        gidmshfo <<"Elements\n";

        const size_t num_tris = mesh.mTriangles.size();

        for(size_t i=0; i<num_tris; i++) {
            gidmshfo << i+1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[0] + 1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[1] + 1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[2] + 1;
            gidmshfo <<"   "<< "1";
            gidmshfo <<"\n";

        }
        gidmshfo <<"end elements\n\n";

        gidmshfo <<"MESH "<<"dimension "<<3<<" ElemType Sphere Nnode 1\n";
        gidmshfo <<"Coordinates\n";
        gidmshfo <<"#node number   coord_x   coord_y coord_z";
        gidmshfo <<" \n";

        for(size_t i=0; i<antennas.size(); i++) {
            const Vec3& node_i = antennas[i].mCoordinates;
            gidmshfo << i+num_nodes+1<<"   "<<node_i[0]<<"   "<<node_i[1]<< "   "<<node_i[2]<<"\n";
        }

        gidmshfo <<"end coordinates\n\n";

        gidmshfo <<"Elements\n";

        for(size_t i=0; i<antennas.size(); i++) {
            gidmshfo << i+num_tris + 1;
            gidmshfo <<"   "<< i + num_nodes + 1;
            gidmshfo <<"   "<< "1";
            gidmshfo <<"   "<< "2";
            gidmshfo <<"\n";

        }
        gidmshfo <<"end elements\n\n";


        gidmshfo <<"MESH "<<"dimension "<<3<<" ElemType Linear Nnode 2\n";
        gidmshfo <<"Coordinates\n";
        gidmshfo <<"#node number   coord_x   coord_y coord_z";
        gidmshfo <<" \n";
        //const Vec3& node2 = mesh.mTriangles[28]->mCenter;
        //gidmshfo << num_nodes+antennas.size()+ 1<<"   "<<node2[0]<<"   "<<node2[1]<< "   "<<node2[2]<<"\n";
        gidmshfo <<"end coordinates\n\n";
        gidmshfo <<"Elements\n";
        //gidmshfo << num_tris + antennas.size() + 1;
        //gidmshfo <<"   "<< num_nodes + 1;
        //gidmshfo <<"   "<< num_nodes+antennas.size()+ 1;
        //gidmshfo <<"   "<< "3";
        //gidmshfo <<"\n";
        gidmshfo <<"end elements\n\n";


        gidresfo <<std::setprecision(8);
        gidresfo <<"GID Post Results File 1.0 \n";

        if(print_type == RESULTS_ON_NODES) {
            gidresfo <<"Result \"Intensity\" \"Analysis/time\" "<<0.0<<" Scalar OnNodes\n";
            gidresfo <<"Values\n";
            for(size_t i=0; i<mesh.mNodes.size(); i++) {
                gidresfo << i+1<<"   "<<mesh.mNodes[i].mIntensity<<"\n";
            }
            gidresfo <<"End Values\n";
        } else {
            gidresfo <<"GaussPoints \"Triangle1GausPoint\" ElemType Triangle\n";
            gidresfo <<"Number Of Gauss Points: 1\n";
            gidresfo <<"Natural Coordinates: internal\n";
            gidresfo <<"end gausspoints\n";
            gidresfo <<"Result \"Intensity\" \"Analysis/time\" "<<0.0<<" Scalar OnGaussPoints \"Triangle1GausPoint\" \n";
            gidresfo <<"Values\n";
            for(size_t i=0; i<mesh.mTriangles.size(); i++) {
                gidresfo << i+1<<"   "<<mesh.mTriangles[i]->mIntensity<<"\n";
            }
            gidresfo <<"End Values\n";
        }
    }
};

#endif