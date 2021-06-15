#ifndef RayTracer_gid_output
#define RayTracer_gid_output

#include <fstream>
#include "constants.h"

class GidOutput {
    
    public:

    GidOutput(){}
    static void PrintResults(const Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type=RESULTS_ON_NODES) {
        std::ofstream gidmshfo(file_name + ".post.msh");
        std::ofstream gidresfo(file_name + ".post.res");

        gidmshfo <<"MESH "<<"dimension "<<3<<" ElemType Triangle Nnode 3"<<std::endl;
        gidmshfo <<"Coordinates"<<std::endl;
        gidmshfo <<"#node number   coord_x   coord_y coord_z";        
        gidmshfo <<" "<<std::endl;

        for(size_t i=0; i<mesh.mNodes.size(); i++) {
            gidmshfo << i+1<<"   "<<mesh.mNodes[i][0]<<"   "<<mesh.mNodes[i][1]<< "   "<<mesh.mNodes[i][2]<<std::endl;    
        }
        gidmshfo <<"end coordinates"<<std::endl<<std::endl;  
        
        gidmshfo <<"Elements"<<std::endl;

        for(size_t i=0; i<mesh.mTriangles.size(); i++) {
            gidmshfo << i+1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[0] + 1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[1] + 1;
            gidmshfo <<"   "<< mesh.mTriangles[i]->mNodeIndices[2] + 1;
            gidmshfo <<"   "<< "1";
            gidmshfo <<std::endl;
    
        }
        gidmshfo <<"end elements"<<std::endl<<std::endl;

        gidresfo <<"GID Post Results File 1.0 "<<std::endl;

        if(print_type == RESULTS_ON_NODES) {
            gidresfo <<"Result \"Intensity\" \"Analysis/time\" "<<0.0<<" Scalar OnNodes"<<std::endl;
            gidresfo <<"Values"<<std::endl;
            for(size_t i=0; i<mesh.mNodes.size(); i++) {
                gidresfo << i+1<<"   "<<mesh.mNodes[i].mIntensity<<std::endl;    
            }
            gidresfo <<"End Values"<<std::endl;
        } else {
            gidresfo <<"GaussPoints \"Triangle1GausPoint\" ElemType Triangle"<<std::endl;
            gidresfo <<"Number Of Gauss Points: 1"<<std::endl;
            gidresfo <<"Natural Coordinates: internal"<<std::endl;
            gidresfo <<"end gausspoints"<<std::endl;
            gidresfo <<"Result \"Intensity\" \"Analysis/time\" "<<0.0<<" Scalar OnGaussPoints \"Triangle1GausPoint\" "<<std::endl;
            gidresfo <<"Values"<<std::endl;
            for(size_t i=0; i<mesh.mTriangles.size(); i++) {                                
                //mesh.mTriangles[i]->mIntensity = (mesh.mNodes[mesh.mTriangles[i]->mNodeIndices[0]].mIntensity + mesh.mNodes[mesh.mTriangles[i]->mNodeIndices[1]].mIntensity +mesh.mNodes[mesh.mTriangles[i]->mNodeIndices[2]].mIntensity) / 3.0f;                
                gidresfo << i+1<<"   "<<mesh.mTriangles[i]->mIntensity<<std::endl;    
            }
            gidresfo <<"End Values"<<std::endl;
        }
    }    
};

#endif