#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "file_util.h"
#include "Vector3.hpp"
#include "Triangle.hpp"

using namespace std;

int main() 
{
    Vector3 w = {1, 2, 3};
    Vector3 x = {3, 4, 0};
    Vector3 z = {0, 2, 12};

    Triangle triT0 = Triangle{w, x, z};

    cout << triT0.one << endl;

    vector<Triangle> test_vector = {};
    test_vector.push_back(triT0);

    Triangle triT1 = Triangle{{0, 3, 3}, {2, 2, 1}, {4, 5, 8}};
    test_vector.push_back(triT1);

    cout << test_vector.size() << endl;
    cout << test_vector.at(0).one << endl;
    cout << test_vector.at(1).one << endl;


}