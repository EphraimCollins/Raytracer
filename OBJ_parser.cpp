//Imports are here
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "file_util.h"
#include "Vector3.hpp"
#include "Triangle.hpp"

using namespace std;

// inline std::ostream &operator<<(std::ostream &os, const Vector3 &v)
// {
//     return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
// }

//main function, probably contain args for the parser, should it be a function instead???
int main ()
{   
    //string of filename to parse. replace this later. or have it be user input?
    string file_name = "test_obj.obj";

    //Vector that contains all the lines of the .obj file.
    vector<string> fileLines = readLines(file_name);

    //print testing.
    cout << "There are " << fileLines.size() << " elements in the vector." << endl;

    //4 vectors, 3 vectors of vector3's, final vector of triangles.
    vector<Vector3> vertex_vec = {};
    vector<Vector3> vnorm_vec = {};
    vector<Vector3> vtext_vec = {};
    vector<Triangle> triangle_vec = {};

    //parsing through the fileLines vector contents, using split to get the parts of the line.
    for(int i = 0; i < fileLines.size(); i++)
    {
        //string containing one line to be split upon. 
        string fileLine = fileLines.at(i);

        //print testing.
        cout << "One whole line: " << fileLine << endl;

        //calls split, which returns all the tokens from a line in a vector.
        char delim = ' ';
        vector<string> fileLineTokens = split(fileLine, delim);

        //print testing.
        cout << "Tokens: ";
        for (string t: fileLineTokens)
        {
            cout << t << ",";
        }
        cout << "end" << endl;
        
        //ignores non f/v/vt/vn lines. 
        string lineType = fileLineTokens.at(0); //.c_str();

        //print test.
        cout << "line type: " << lineType << "." << endl;

        //ignores 'junk' values, saves valid lines to their respective vector buffers.
        if(lineType == "v") 
        {   
            //saves vertex info as a Vector3 to be saved in another vector_vec buffer.
            Vector3 vertex = {stof(fileLineTokens.at(1)), 
                              stof(fileLineTokens.at(2)), 
                              stof(fileLineTokens.at(3))};

            //print testing.
            cout << "Vertex:" << vertex << endl;
            
            //saves vertex to vertex_vec vector. 
            vertex_vec.push_back(vertex);
            
        } else if(lineType == "vn") 
        {
            //save normal info in vector 3 
            Vector3 normal = {stof(fileLineTokens.at(1)), 
                              stof(fileLineTokens.at(2)), 
                              stof(fileLineTokens.at(3))};

            //print testing.
            cout << "Normal:" << normal << endl;

            //saves normal to vnorm_vec buffer.
            vnorm_vec.push_back(normal);

        } else if(lineType == "vt") 
        {
            //constructs vector3.
            Vector3 texture = {stof(fileLineTokens.at(1)), 
                                stof(fileLineTokens.at(2)), 
                                0.0f};

            //print testing.
            cout << "texture:" << texture << endl;

            //adds tempArr to vtext_vec vector.
            vtext_vec.push_back(texture); 

        } else if(lineType == "f") 
        {
            //array holding all converted index values for one face.
            int faceIdx[9];

            // -> v/vt/vn
            //"f x/x/x x/x/x x/x/x" -> "x/x/x" "x/x/x" "x/x/x"
            //fileLineTokens should have length 4, adjust parameters to get right indexing.
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
            //should run three times and create three triangles. 
            for (int z = 0; z < size(faceIdx) / 3; z++) 
            {
                //index offset.
                int idxOffset = 0;

                //grabs v/vn/vt lines from vertex_vec based of faceIdx, and respective vector.
                Triangle temp_tri = Triangle{Vector3{vertex_vec.at(faceIdx[0 + idxOffset])},
                                             Vector3{vnorm_vec.at(faceIdx[1 + idxOffset])}, 
                                             Vector3{vtext_vec.at(faceIdx[2 + idxOffset])}};

                //increment the offset.
                idxOffset += 3;

                //saves vertex into constructed_vec vector.
                triangle_vec.push_back(temp_tri);
            }

        } else 
        {
            //is a junk value, break? go to next value???
            cout << "Ignored line: " <<  << endl;
            continue;
        }
    } 

    cout << "does this even run?" << endl;
    cout << triangle_vec.at(0).one << endl;

    //print function for testing.
    //vector -> triangle -> three vectors w/ operator<<() function.
    for (const Triangle &triTemp: triangle_vec) 
    {
        //this should print the three vertexs that make up a triangle.
        cout << triTemp.one << endl;
        cout << triTemp.two << endl;
        cout << triTemp.three << endl;
    }
}