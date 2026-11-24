/// Include the image write library and enable it
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <charconv>
#include <time.h>
#include "Vector3.hpp"
#include "Triangle.hpp"
#include "OBJ_parser.hpp"
#include "omp.h"

using namespace std;

/// Width of the final image
const int w = 640;

/// Height of the final image
const int h = 640;

/// The 1d array for storing the pixel we have generated
//set every area as default to background color to move singular computation 
//every time down below in the main loop???
unsigned char pixels[w * h * 3];

//for timing.
double start_time, end_time = 0; 

// //Left Wall
// Triangle triangle01 = Triangle{Vector3{-1, 1, -2}, Vector3{-1, 1, -1}, Vector3{-1, -1, -2}};
// Triangle triangle02 = Triangle{Vector3{-1, 1, -1}, Vector3{-1, -1, -1}, Vector3{-1, -1, -2}};

// //Right Wall
// Triangle triangle03 = Triangle{Vector3{1, 1, -1}, Vector3{1, 1, -2}, Vector3{1, -1, -2}};
// Triangle triangle04 = Triangle{Vector3{1, -1, -1}, Vector3{1, 1, -1}, Vector3{1, -1, -2}};

// //Top Wall
// Triangle triangle05 = Triangle{Vector3{-1, 1, -1}, Vector3{-1, 1, -2}, Vector3{1, 1, -2}};
// Triangle triangle06 = Triangle{Vector3{1, 1, -1}, Vector3{-1, 1, -1}, Vector3{1, 1, -2}};

// //Bottom Wall
// Triangle triangle07 = Triangle{Vector3{-1, -1, -2}, Vector3{-1, -1, -1}, Vector3{1, -1, -2}};
// Triangle triangle08 = Triangle{Vector3{-1, -1, -1}, Vector3{1, -1, -1}, Vector3{1, -1, -2}};

// //Back Wall
// Triangle triangle09 = Triangle{Vector3{-1, 1, -2}, Vector3{-1, -1, -2}, Vector3{1, -1, -2}};
// Triangle triangle10 = Triangle{Vector3{1, 1, -2}, Vector3{-1, 1, -2}, Vector3{1, -1, -2}};

// vector<Triangle> triangles = {
//     triangle01, triangle02,
//     triangle03, triangle04,
//     triangle05, triangle06,
//     triangle07, triangle08,
//     triangle09, triangle10}; 

string shapeName = "torus"; //cin???

vector<Triangle> triangles = object_parser("OBJ\\" + shapeName + ".obj"); 
//vector or tuple of three arrays? which one is more preformant???
//x = [A.x B.x ... N.x] 
//y = [A.y B.y ... N.y]
//z = [A.z B.z ... N.z]
//are memory accesses slower or faster then struct member accessing???

//what to do with the texture (u, v) mapping??? 

//put background color here
Vector3 background_color = Vector3{255, 255, 255};

#pragma omp declare simd 
float trace_ray_distance(Vector3 origin, Vector3 direction)
{
  float closest_t = INFINITY;

  for (const Triangle &triangle: triangles)
  {

    //Find where we hit the plane
    Vector3 edge_one = triangle.one - triangle.two;
    Vector3 edge_two = triangle.three - triangle.two;
    Vector3 p = edge_two.cross(edge_one);
    Vector3 abc = p.normalize();
    float d = -abc.dot(triangle.one);

    float t = -(abc.dot(origin) + d) / abc.dot(direction);

    if (t > closest_t || t <= 0) {
      continue;
    }

    Vector3 collision_point = direction * t + origin;

    Vector3 minus_one = collision_point - triangle.one;
    Vector3 minus_two = collision_point - triangle.two;
    Vector3 minus_three = collision_point - triangle.three;

    float triangle_area = p.length() / 2;

    float area_one = minus_two.cross(minus_three).length() / 2;
    float area_two = minus_three.cross(minus_one).length() / 2;
    float area_three = minus_one.cross(minus_two).length() / 2;

    float u = area_one / triangle_area;
    float v = area_two / triangle_area;
    float a = area_three / triangle_area;

    float sum = u + v + a;

    float diff = abs(1 - sum);

    if (diff > .00001 || isnan(sum))
    {
      //I'm not in the triangle, continue?
      continue;
    }
    else
    {
      //in the triangle
      closest_t = t;
    }
  }
  return closest_t;
}

//bounces had b = 2000, but program would not even run.
#pragma omp declare simd 
Vector3 trace_ray(Vector3 origin, Vector3 direction, int bounces = 3)
{
  Vector3 color = Vector3{background_color.x, background_color.y, background_color.z};
  //std::cout << "inside trace ray after color is set." << endl;
  if(bounces <= 0) 
  {
    return background_color;
  }

  float closest_t = 1000000;
  //std::cout << "in trace_ray before for loop." << endl;
  for (const Triangle &triangle : triangles)
  {
    //Find where we hit the plane
    Vector3 edge_one = triangle.one - triangle.two;
    Vector3 edge_two = triangle.three - triangle.two;
    Vector3 p = edge_two.cross(edge_one);
    Vector3 abc = p.normalize();
    float d = -abc.dot(triangle.one);

    float t = -(abc.dot(origin) + d) / abc.dot(direction);

    if (t > closest_t || t <= 0) 
    {
      continue;
    }

    Vector3 collision_point = direction * t + origin;

    Vector3 minus_one = collision_point - triangle.one;
    Vector3 minus_two = collision_point - triangle.two;
    Vector3 minus_three = collision_point - triangle.three;

    float triangle_area = p.length() / 2;

    float area_one = minus_two.cross(minus_three).length() / 2;
    float area_two = minus_three.cross(minus_one).length() / 2;
    float area_three = minus_one.cross(minus_two).length() / 2;

    float u = area_one / triangle_area;
    float v = area_two / triangle_area;
    float a = area_three / triangle_area;

    float sum = u + v + a;

    float diff = abs(1 - sum);

    if (diff > .00001 || isnan(sum))
    {
      // I'm not in the triangle, continue?
      continue;
    }
    else
    {
      // I'm in the triangle
      closest_t = t;

      Vector3 base_color = Vector3{255, 255, 255};
      Vector3 light_direction = Vector3{.5, 1, .5}.normalize();

      Vector3 ambient = base_color * .1;
      Vector3 normal = p.normalize();

      float strength = max(0.f, light_direction.dot(normal));
      //float strength = abs(light_direction.dot(normal));

      Vector3 diffuse = base_color * strength;
      if(trace_ray_distance(collision_point, light_direction) != INFINITY)
      {
        diffuse = Vector3{0,0,0};
      }

      Vector3 reflective = Vector3{0, 0, 0};

      if(true)
      {
        Vector3 reflective_direction = direction - normal * 2 * normal.dot(direction);
        //convert recursion to iterative? 
        reflective = trace_ray(collision_point, reflective_direction, bounces - 1) * .5;
        diffuse *= .5;
        ambient *= .5;
      }

      Vector3 _color = ambient + diffuse + reflective;

      color = _color;
     
    }
  }
  return color;
}

vector<std::string> fast_to_str(vector<float> vals) 
{
  vector<std::string> str_array = {};

  //should convert all float values.
  for(const float &v: vals) 
  {
    char buf[20];
    auto [ptr, ec] = to_chars(buf, buf + 20, v);

    //make std::string
    std::string converted(buf, ptr);
    str_array.push_back(converted);

    //manually clear/zero out buffer.
    std::fill(std::begin(buf), std::end(buf), 0);
  }

  //returns vector of all four vectors. 
  return str_array;
}

//intial multi-threading?
//1-> split work inside main into a # of threads?
////split input into n sub-tiles??? 680 x 680 -> 10 68 x 68 tiles
////would have to convert from inter-tile index to intra-tile main index for pixel assignment.
//amd simd??? would need to re-write how vector3 functions work?
//would need to re-write with structure of arrays (arrays of x, y, z)
//each thread n could get a nth of the total work, would to break up array/work. 

int main()
{
  //sub-folder name.
  filesystem::path subf = "ImageOutput";

  //
  if(!filesystem::exists(subf)) {
    filesystem::create_directories(subf);
  }


  start_time = omp_get_wtime();
  //cout << "start of main!" << endl;

  //testing for triangles.
  // for (const Triangle &triangle: triangles) 
  // {
  //   cout << triangle;
  // }

  Vector3 cam_origin = Vector3{0, 0, 10};
  //Vector3 camera_look_at = Vector3{0, 0, 0};
  //Vector3 camera_up = Vector3{0, 1, 0};

  //std::cout << "this is before looping starts" << endl;

  
  //OpenMP enabled, sped up program execution by ~50%.
  #pragma omp parallel for simd collapse(2) 
  for (int y = 0; y < h; y++)
  {
    //While going down, loop over every pixel going right
    //#pragma omp simd
    for (int x = 0; x < w; ++x)
    {

      //cout << "how times the program runs." << endl;

      /// Figure out our index in the 1d array of pixels
      int index = y * w * 3 + x * 3;

      /// The value of the red channel (0-255)
      unsigned char r = background_color.x;

      /// The value of the green channel (0-255)
      unsigned char g = background_color.y;

      /// The value of the blue channel (0-255)
      unsigned char b = background_color.z;

      //cout << "this is before ray is computed" << endl;

      //How raytrace?
      Vector3 target = Vector3{x/(float)w*2-1, -(y/(float)h*2-1), 0};
      Vector3 temp = (target - cam_origin);
      Vector3 ray = temp.normalize();

      //cout << "this is before ray is traced to figure out color." << endl;

      //color for pixel. 
      Vector3 color = trace_ray(cam_origin, ray);

      //cout << "this is after ray has been traced." << endl;

      r = color.x;
      g = color.y;
      b = color.z;

      /// Assign the r, g, and b values to the pixel at index.
      pixels[index] = r;
      pixels[index + 1] = g;
      pixels[index + 2] = b;
    }
  }
  
  //preformance time.
  end_time = omp_get_wtime();
  float spd = end_time - start_time; 
  cout << "Time elapsed: " << spd << endl;

  //timestamp for image.
  // std::time_t now = std::time(nullptr);
  // const tm* t = std::ctime(&now); 
  // char buf [90];
  // strftime(buf, 90, "%a-%b-%d-%r-%G", t);
  // string time_stamp(buf);

  //why are there so many time libraries??????
  // time_t rawtime;
  // struct tm * tminfo;
  // char buf[120];
  // time(&rawtime);
  // tminfo = localtime(&rawtime);
  // strftime(buf, 120, "%a-%b-%d-%r-%G", tminfo);
  // string t_stamp(buf);

  //...
  std::time_t now = std::time(nullptr);
  std::tm* localTime = std::localtime(&now); 
  char buf[100];
  strftime(buf, sizeof(buf), "%a%b%d%r%G", localTime);
  string date(buf);
  // cout << to_string(spd) + "," + to_string(cam_origin.x) + "," + to_string(cam_origin.y) 
  // + "," + to_string(cam_origin.z) << endl;
  //for conversion function 
  vector<float> in = {spd, cam_origin.x, cam_origin.y, cam_origin.z}; 

  //function call
  vector<string> out = fast_to_str(in);

  //timestamp_preformance_x_y_z_shapeName.png
  filesystem::path name = date + out.at(0) + "_" + out.at(1) + "_" + out.at(2) + "_" 
  + out.at(3) + "_" + shapeName + ".png";
  filesystem::path fullpath = subf / name;

  //Write the final image as a png.
  int result = stbi_write_png(fullpath.string().c_str(), w, h, 3, pixels, w * 3);


  cout << "Did the image write?: " << result << endl;
  cout << "timestamp: " + date << endl;
  return 0;
}
