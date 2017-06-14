#include <iostream>
#include <fstream>
#include <vector>
#include <dlib/gui_widgets.h>
#include <dirent.h>
#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
#define MAX_SIZE 4096
#define SIMILARITY 0.65
using namespace dlib;
using namespace std;

std::vector<float> txt_to_vector(string path)
{
    std::vector<float> descriptor;

    ifstream in_file(path);
    if(in_file.is_open())
    {
        for(int i = 0; i < MAX_SIZE; i++)
        {
            string str1="";
            getline(in_file,str1);
            descriptor.push_back(stof(str1));
        }
        in_file.close();
    }
    else
    {
        cout << "error in txt to vector" << endl;
    }
        return descriptor;
}

float vector_length(std::vector<float> in_vector)
{
    float distance = 0.0;
    for(size_t i = 0; i < MAX_SIZE; i++)
        distance += (in_vector[i]*in_vector[i]);
    return sqrt(distance);
}

float vector_inner_product(std::vector<float> a, std::vector<float> b)
{
    float result = 0.0;
    for(size_t i = 0; i < MAX_SIZE; i++)
    {
        result += (a[i]*b[i]);
    }
    result = result / vector_length(a) / vector_length(b);
    return result;
}

int search_MAX(std::vector<std::vector<float> > face_descriptors, std::vector<float> bench_descriptor)
{
    float max = -1;
    int max_index = -1;
    for(size_t i = 0; i < face_descriptors.size(); i++)
    {
        if(face_descriptors[i].size() == 0)
            break;
        float similarity = vector_inner_product(bench_descriptor, face_descriptors[i]);
        if(similarity >= SIMILARITY && similarity > max)
        {
            max = similarity;
            max_index = i;
        }
    }
    return max_index;
}

int identificate(std::vector<std::vector<float> > face_descriptors, std::vector<float> bench_descriptor)
{
    float max = -1;
    int max_index = -1;
    for(size_t i = 0; i < face_descriptors.size(); i++)
    {
        if(face_descriptors[i].size() == 0)
            break;
        float similarity = vector_inner_product(bench_descriptor, face_descriptors[i]);
        if(similarity > max)
        {
            max = similarity;
            max_index = i;
        }
    }
    return max_index;
}

std::vector<std::string> get_files(std::string dir,std::string want_to_get)
{
    DIR *dp;
    struct dirent *dirp;
    std::vector<std::string> files;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        return files;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string file_name = string(dirp->d_name);
        if((int)file_name.find(want_to_get) > 0)
        {
            files.push_back(dir + file_name);
        }
    }
    closedir(dp);
    return files;
}
