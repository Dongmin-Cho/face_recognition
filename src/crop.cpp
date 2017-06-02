#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <iostream>
#include <algorithm>
#include <dlib/opencv.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "calc.h"
#include "crop_face_img.h"

#define FACE_SIZE 300
#define FACE_PADDING 0.65

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------
// This program normalize face in the photo and saves it *.*g in the "output" directory
// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    try
    {
        if (argc == 1)
        {
            cout << "call like this" << endl;
            cout << "./crop out_dir input_img_files" <<endl;
            return 0;
        }

        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor sp;
        deserialize("shape_predictor_68_face_landmarks.dat") >> sp;


        // Loop over all the images provided on the command line.
        for (int i = 2; i < argc; ++i)
        {
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;
            cv::Mat faceimg = cv::imread(argv[i]);
            assign_image(img, cv_image<rgb_pixel>(faceimg));

            //load_image(img, argv[i]);
            // Make the image larger so we can detect small faces.
            pyramid_up(img);

            //load_image(img, in_file_path);

            std::vector<dlib::rectangle> dets = detector(img);

            std::vector<full_object_detection> shapes;
            std::vector<dlib::rectangle> rect;

          if(dets.size() == 0)
          {
            unlink(argv[i]);
            return -1;
          }
          cout << "process checking" << endl;
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
            cv::Point2f eye_loc_L(sum_left_x/6, sum_left_y/6), eye_loc_R(sum_right_x/6, sum_right_y/6);
            cv::Mat input;
            faceimg = toMat(img);
            faceimg = _cropFaceImagebyEYE(faceimg, eye_loc_L, eye_loc_R, FACE_SIZE, FACE_SIZE, 0.4f, 0.38f);


            std::string output = argv[1];
            std::vector<std::string> temp_dir;
            temp_dir = get_files(output, ".png");
            std::vector<int> temp_int;
            for(size_t i = 0; i < temp_dir.size(); i++)
            {
                std::string temp_name;
                temp_name = temp_dir[i].substr(temp_dir[i].find_last_of("/")+1,
                temp_dir[i].find(".png")-temp_dir[i].find_last_of("/")-1);
                temp_int.push_back(stoi(temp_name));
            }
            sort(temp_int.begin(), temp_int.end());

            size_t j = 0;
            for(/*nothing*/; j < temp_int.size(); j++)
            {
                if(j == temp_int[j]-1)
                    continue;
                else
                    break;
            }
            output += to_string(j+1) + ".png";
            cout << "save " << output << "       " << temp_dir.size() << endl;
            cv::imwrite(output,faceimg);
          }
        }
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}
