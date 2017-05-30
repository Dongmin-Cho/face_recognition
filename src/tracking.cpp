#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <time.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace dlib;
using namespace std;

std::vector<float> tracking(string in_file_path){

    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor sp;
    deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

    array2d<rgb_pixel> img;
    load_image(img, in_file_path);
    std::vector<dlib::rectangle> dets = detector(img);
    std::vector<full_object_detection> shapes;
    std::vector<dlib::rectangle> rect;
    std::vector<float> direction_set;

    for (unsigned long j = 0; j < dets.size(); ++j)
    {
        full_object_detection shape = sp(img, dets[j]);
        shapes.push_back(shape);
    }

    for (size_t k = 0; k < shapes.size(); k++)
	{
		float sum_left_x = 0;
		float sum_left_y = 0;
		for(size_t left = 36; left <= 41; left ++)
		{
			sum_left_x += shapes[k].part(left).x();
			sum_left_y += shapes[k].part(left).y();
		}
		float sum_right_x = 0;
		float sum_right_y = 0;
		for(size_t right = 42; right <= 47; right ++)
		{
			sum_right_x += shapes[k].part(right).x();
			sum_right_y += shapes[k].part(right).y();
        }
        sum_left_x/=6;
        sum_left_y/=6;

        sum_right_x/=6;
        sum_right_y/=6;

        float btw_eyes_x = shapes[k].part(27).x();
        float btw_eyes_y = shapes[k].part(27).y();

        float left_distance = sqrt(pow(sum_left_x - btw_eyes_x,2) + pow(sum_left_y - btw_eyes_y,2));
        float right_distance = sqrt(pow(sum_right_x - btw_eyes_x,2) + pow(sum_right_y - btw_eyes_y,2));

        float direction = 1 - (left_distance/right_distance);

        float result = btw_eyes_x - (btw_eyes_y/direction);
        cout<<result<<endl;
        direction_set.push_back(result);

    }
    return direction_set;

}

int main(int argc,char**argv){
    if(argc==1){
        cout<<"call like this"<<endl;
        cout<<"./tracking input_files"<<endl;
        cout<<"It returns direction set with type of vector<float>"<<endl;
        return -1;
    }
    tracking(argv[1]);
    cout<<"find head pose!!!!"<<endl;
    return 0;
}
