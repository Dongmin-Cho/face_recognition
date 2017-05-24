/*
call like ./check_attendance

*/

#define SIMILARITY 0.7
#define STUDENT_DIR "/home/wj/work/Im_Here/Web/public/faceImage/"

#include <dlib/gui_widgets.h>
#include <dirent.h>
#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include "calc.h"

using namespace dlib;
using namespace std;

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
        temp_dir = get_files(STUDENT_DIR + std_num + "/", ".txt");
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
        to_jpg += ".png";
        cout<< to_jpg;
    }
    cout << endl;

    return 0;
}
catch (std::exception& e)
{
    cout << e.what() << endl;
}
