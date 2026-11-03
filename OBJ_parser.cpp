//Imports are here
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "file_util.h"
#include "Vector3.hpp"
#include "Triangle.hpp"

using namespace std;

//main function, probably contain args for the parser, should it be a function instead???
int main ()
{   
    //string of filename to parse. replace this later. or have it be user input?
    string file_name = "*.obj";

    //Vector that contains all the lines of the .obj file.
    vector<string> fileLines = readLines(file_name);

    //4 vectors, 2 vectors of vector3's, 1 vector of string and 1 custom structs. 
    //should the vertex and vnorm vectors be merged.
    vector<Vector3> vertex_vec = {};
    vector<Vector3> vnorm_vec = {};
    //vector<Vector3> v_vn_vec = {}; 
    //vector<pair<float, float>> vtext_vec = {};
    //vector<array<float, 2>> vtext_vec = {};
    vector<Vector3> vtext_vec = {};
    vector<Triangle> constructed_vec = {};

    //parsing through the fileLines vector contents, using split to get the parts of the line.
    for(int i = 0; i < fileLines.size(); i++)
    {
        //string containing one line to be split upon. 
        string fileLine = fileLines.at(i);

        //calls split, which returns all the tokens from a line in a vector.
        char delim = ' ';
        vector<string> fileLineTokens = split(fileLine, delim);
        
        //ignores non f/v/vt/vn lines. 
        string lineType = fileLineTokens.at(0).c_str();

        //ignores 'junk' values, saves valid lines to their respective vector buffers.
        if(lineType == "v") 
        {   
            //saves vertex info as a Vector3 to be saved in another vector_vec buffer.
            Vector3 vertex = {stof(fileLineTokens.at(1)), 
                            stof(fileLineTokens.at(2)), stof(fileLineTokens.at(3))};
            
            //saves vertex to vertex_vec vector. 
            vertex_vec.push_back(vertex);
            //v_vn_vec.push_back(vertex);
            
        } else if(lineType == "vn") 
        {
            //save normal info in vector 3 
            Vector3 normal = {stof(fileLineTokens.at(1)), 
                            stof(fileLineTokens.at(2)), stof(fileLineTokens.at(3))};

            //saves normal to vnorm_vec buffer.
            vnorm_vec.push_back(normal);
            //v_vn_vec.push_back(normal);

        } else if(lineType == "vt") 
        {
            //constructs temp array to add to the 
            //array<float, 2> tempArr = {stof(fileLineTokens.at(1)), stof(fileLineTokens.at(2))};
            Vector3 temp_vec = {stof(fileLineTokens.at(1)), 
                            stof(fileLineTokens.at(2)), 0.0f};

            //adds tempArr to vtext_vec vector.
            vtext_vec.push_back(temp_vec); 


        } else if(lineType == "f") 
        {
            //array holding all converted index values for one face.
            int faceIdx[9];

            // -> v/vt/vn
            //"f x/x/x x/x/x x/x/x" -> "x/x/x" "x/x/x" "x/x/x"
            //flt should have length 4, adjust parameters to get right indexing.
            for (int x = 1; x <= fileLineTokens.size() - 1; x++) 
            {
                //splits flt one of the face "x/x/x" values.
                vector<string> faceTokens = split(fileLineTokens.at(x), '/');

                //faceTokens = "x/x/x", grab all three x's and convert to integers.
                //minus one the values to get them zero indexed.
                for (int y = 0; y < faceTokens.size(); y++)
                {
                    //minus one to get zero indexed index.
                    faceIdx[y] = stoi(faceTokens.at(y)) - 1; 
                }   
            }
            //could add size check, 1="x/", 2="x//x" or "x/x", 3="x/x/x"
            //x//x returns a x, " " and x. so check size and check idx 1.

            //should run three times 
            for (int z = 0; z < size(faceIdx) / 3; z++) {
                //
                int idxOffset = 0;

                //should there be 3 triangles???
                //grabs v/vn/vt lines from vertex_vec based of faceIdx, and respective vector.
                // Triangle temp_tri = Triangle{Vector3{vertex_vec.at(faceIdx[0 + idxOffset])},
                //                              Vector3{vertex_vec.at(faceIdx[1 + IdxOffset])}, 
                //                              Vector3{vertex_vec.at(faceIdx[2 + idxOffset])}};

                // Triangle temp_tri = Triangle{Vector3{v_vn_vec.at(faceIdx[0 + idxOffset])},
                //                              Vector3{v_vn_vec.at(faceIdx[1 + idxOffset])}, 
                //                              Vector3{v_vn_vec.at(faceIdx[2 + idxOffset])}};

                //grabs v/vn/vt lines from vertex_vec based of faceIdx, and respective vector.
                Triangle temp_tri = Triangle{Vector3{vertex_vec.at(faceIdx[0 + idxOffset])},
                                             Vector3{vnorm_vec.at(faceIdx[1 + idxOffset])}, 
                                             Vector3{vtext_vec.at(faceIdx[2 + idxOffset])}};
    
                //forms the tuple of vector3 that will be stored. 
                //tuple<Vector3, Vector3, Vector3> t1({faceVal[0], faceVal[1], faceVal[2]}, 
                //                                    {faceVal[3], faceVal[4], faceVal[5]}, 
                //                                    {faceVal[6], faceVal[7], faceVal[8]});

                //
                idxOffset += 3;

                //saves tuple into constructed_vec vector.
                constructed_vec.push_back(temp_tri);
            }

        } else 
        {
            //is a junk value, break? go to next value???
            break;
        }
    } 
}