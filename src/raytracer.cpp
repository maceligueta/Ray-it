#include <stdio.h>
#include <vector>

#include "stl_reader.h"

#include "scene_io.h"
#include "Timer.h"
#include "vector.h"
#include "mesh.h"
#include "ray.h"
#include "kdTree.h"
#include "tests_manager.h"
#include "gid_output.h"


unsigned int echo_level = 1;


void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type) {
    if(echo_level > 0) std::cout << "Printing results in GiD Post-process format... ";
    GidOutput gid_printer;
    gid_printer.PrintResults(mesh, file_name, print_type); 
    if(echo_level > 0) std::cout << "  done!"<<std::endl;
}

bool ReadTerrainMesh(Mesh& mesh, const std::string& filename) {


    try {

        if (echo_level > 0) std::cout<<"Reading STL mesh ("<<filename<<")..."<<std::endl;
        stl_reader::StlMesh <float, unsigned int> stl_mesh (filename);        

        if (echo_level > 1) {            
            for(size_t itri = 0; itri < stl_mesh.num_tris(); ++itri) {
                std::cout << "Coords " << itri << ": ";
                for(size_t icorner = 0; icorner < 3; ++icorner) {
                    const float* c = stl_mesh.tri_corner_coords(itri, icorner);               
                    std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
                }
                std::cout << std::endl;
            
                std::cout << "Indices " << itri << ": ( ";
                for(size_t icorner = 0; icorner < 3; ++icorner) {                
                    const unsigned int c =  stl_mesh.tri_corner_ind (itri, icorner);                
                    std::cout << c << " ";
                }        
                std::cout << ")" << std::endl;   

                const float* n = stl_mesh.tri_normal(itri);
                std::cout << "normal of triangle " << itri << ": "
                        << "(" << n[0] << ", " << n[1] << ", " << n[2] << ")\n";
            }
        }        

        float xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
        float xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

        for(int i=0; i<stl_mesh.num_vrts(); i++){
            Vec3f node(stl_mesh.vrt_coords(i)[0], stl_mesh.vrt_coords(i)[1], stl_mesh.vrt_coords(i)[2]);
            mesh.mNodes.push_back(node);
            xmin = fmin(xmin, node[0]);
            ymin = fmin(ymin, node[1]);
            zmin = fmin(zmin, node[2]);
            xmax = fmax(xmax, node[0]);
            ymax = fmax(ymax, node[1]);
            zmax = fmax(zmax, node[2]);
        }

        if (echo_level > 1) {     
            std::cout << "List of nodes and coordinates: "<<std::endl;       
            for(size_t i = 0; i < mesh.mNodes.size(); i++) {
                std::cout<<i<<"  "<<mesh.mNodes[i][0]<<"  "<<mesh.mNodes[i][1]<<"  "<<mesh.mNodes[i][2]<<std::endl;
            }
        }

        float added_tolerance = 0.001f * fmax( xmax-xmin, fmax(ymax-ymin, zmax-zmin));
        mesh.mBoundingBox[0] = Vec3f(xmin-added_tolerance, ymin-added_tolerance, zmin-added_tolerance);
        mesh.mBoundingBox[1] = Vec3f(xmax+added_tolerance, ymax+added_tolerance, zmax+added_tolerance);

        for(int i = 0; i < stl_mesh.num_tris(); i++){
            Vec3f N(stl_mesh.tri_normal(i)[0], stl_mesh.tri_normal(i)[1], stl_mesh.tri_normal(i)[2]);
            mesh.mNormals.push_back(N);
                                    
            Triangle* t = new Triangle(mesh); 
            t->mNodeIndices[0] = stl_mesh.tri_corner_ind(i, 0);
            t->mNodeIndices[1] = stl_mesh.tri_corner_ind(i, 1);
            t->mNodeIndices[2] = stl_mesh.tri_corner_ind(i, 2);
            t->p0 = mesh.mNodes[t->mNodeIndices[0]];
            t->p1 = mesh.mNodes[t->mNodeIndices[1]];
            t->p2 = mesh.mNodes[t->mNodeIndices[2]];
            t->SetEdgesAndPrecomputedValues(); 
            t->mId = i;

            mesh.mTriangles.push_back(t);
        }

        if (echo_level > 0) std::cout<<"File "<<filename<<" was read correctly. "<<mesh.mNodes.size()<<" nodes and "<<mesh.mTriangles.size()<<" elements."<<std::endl;
        
        if(echo_level > 0) std::cout << "Building KD-tree... " << std::endl;
        KDTreeNode* root= new KDTreeNode();    
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());
        if(echo_level > 0) std::cout << "Finished building KD-tree."<< std::endl;

    } catch (std::exception& e) {
        std::cout<<e.what()<<std::endl;
        return false;
    }
    
    return true;
}

int main(int argc, char *argv[]) {

    if(argc != 2) {
        std::cout<<"Type one (and only one) argument specifying the STL file containing the terrain. If the argument is 'tests' the code will run the tests."<<std::endl;
        return 1;
    }

    Timer total_timer;
    total_timer.start();

    Mesh mesh;   

    std::string filename = argv[1];
    if(filename == "tests") {
        RunTests();
        return 1;
    }

    if(!ReadTerrainMesh(mesh, filename)) return 0;

    if(echo_level > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

    Vec3f origin(0.0f, 0.0f, 3.0f);

    /* for(size_t i = 0; i<mesh.mNodes.size(); i++) {
        Vec3f vec_origin_to_node = Vec3f(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
        Ray test_ray(origin, vec_origin_to_node);
        test_ray.Intersect(mesh);
        const float distance_squared = vec_origin_to_node[0] * vec_origin_to_node[0] + vec_origin_to_node[1] *vec_origin_to_node[1] + vec_origin_to_node[2] * vec_origin_to_node[2];
        if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < 1e-4f) {            
            mesh.mNodes[i].mIntensity = 1.0f / distance_squared;
        } 
    }  */   

    for(size_t i = 0; i<mesh.mTriangles.size(); i++) {
        Vec3f vec_origin_to_triangle_center = Vec3f(mesh.mTriangles[i]->mCenter[0] - origin[0], mesh.mTriangles[i]->mCenter[1] - origin[1], mesh.mTriangles[i]->mCenter[2] - origin[2]);
        Ray test_ray(origin, vec_origin_to_triangle_center);
        test_ray.Intersect(mesh);
        const float distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
        if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < 1e-4f) {            
            mesh.mTriangles[i]->mIntensity = 1.0f / distance_squared;
            test_ray.mPower = mesh.mTriangles[i]->mIntensity * mesh.mTriangles[i]->ComputeArea() * Vec3f::DotProduct(test_ray.mDirection, mesh.mTriangles[i]->mNormal);
        } 
    }          

    if(echo_level > 0) std::cout << "Computation finished."<<std::endl;               

    PrintResultsInGidFormat(mesh, filename, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);

    total_timer.stop();
    if(echo_level > 0) std::cout << "Total time: " << total_timer.getElapsedTimeInMilliSec() << "ms." << std::endl;
    if(echo_level > 0) std::cout << "Process finished normally."<<std::endl; 
    return 1;
}
