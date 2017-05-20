/*
call like ./check_attendance

*/

#define SIMILARITY 0.7
#define STUDENT_DIR "../sample/"
#define VECTOR_SIZE 4096

#include <dlib/gui_widgets.h>
#include <dirent.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include "transpose_vector.h"

using namespace dlib;
using namespace std;

float vector_length(std::vector<float> in_vector)
{
    float distance = 0.0;
    for(size_t i = 0; i < VECTOR_SIZE; i++)
        distance += (in_vector[i]*in_vector[i]);
    return sqrt(distance);
}

float vector_inner_product(std::vector<float> a, std::vector<float> b)
{
    float result = 0.0;
    for(size_t i = 0; i < VECTOR_SIZE; i++)
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
        if(similarity >= 0.7 && similarity > max)
        {
            max = similarity;
            max_index = i;
        }
    }
    return max_index;
}

std::vector<std::string> get_txt_files(std::string dir)
{
    DIR *dp;
    struct dirent *dirp;
    std::vector<std::string> files;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        cout << "Error opening " << dir << endl;
        return files; 
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string file_name = string(dirp->d_name);
        if((int)file_name.find(".txt") > 0)
        {
            files.push_back(dir + "/" + file_name);
        }
    }
    closedir(dp);
    return files;
}

int main(int argc, char** argv) try
{
    if(argc < 3)
    {
        cout << " not enough arguments, please type like this" <<endl;
        cout << "-> ./check_attendance benchmark_descriptor student_numbers" <<endl;
        return -1;
    }  
    std::vector<std::vector<float> > face_descriptors;
    int vector_index = 0;
    std::vector<std::string> descriptors_dir;
    std::vector<float> benchmark_descriptor = txt_to_vector(argv[1]);
   
    //add directory in folder
    for(size_t i = 2; i < argc; i++)
    {
        std::vector<std::string> temp_dir;
        std::string std_num = argv[i];
        temp_dir = get_txt_files(STUDENT_DIR + std_num);
        for(size_t j = 0; j < temp_dir.size(); j++)
        {   
            std::vector<float> temp_vector = txt_to_vector(temp_dir[j]);
            descriptors_dir.push_back(temp_dir[j]);
            face_descriptors.push_back(temp_vector); 
        }
        vector_index++;
    }

    if (face_descriptors.size() == 0)
    {
        cout << "-1" << endl;
        return 1;
    }
   
    int max_index = search_MAX(face_descriptors, benchmark_descriptor); 
    if(max_index == -1)
        cout<<"absence";
    else
    {
        string to_jpg = descriptors_dir[max_index];
        to_jpg.erase(to_jpg.find(".txt"));
        to_jpg += ".jpg";
        cout<< to_jpg;
    }
    cout << endl;

    return 0;
}
catch (std::exception& e)
{
    cout << e.what() << endl;
}
