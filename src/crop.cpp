#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <iostream>
#include <algorithm>
#include "calc.h"

#define FACE_SIZE 224 
#define FACE_PADDING 0.25

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
            load_image(img, argv[i]);
            // Make the image larger so we can detect small faces.
            pyramid_up(img);

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            std::vector<rectangle> dets = detector(img);
            cout << "Number of faces detected: " << dets.size() << endl;

            // Now we will go ask the shape_predictor to tell us the pose of
            // each face we detected.
            std::vector<full_object_detection> shapes;
            std::vector<rectangle> rect;
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                full_object_detection shape = sp(img, dets[j]);
                shapes.push_back(shape);
            }
            
            dlib::array<array2d<rgb_pixel> > face_chips;
            extract_image_chips(img,get_face_chip_details(shapes, FACE_SIZE, FACE_PADDING), face_chips);
           
            
            for(size_t k = 0; k < face_chips.size(); k++)
            {
                std::string output = argv[1];
                dlib::array<array2d<rgb_pixel> > personal_face;
                personal_face.push_back(face_chips[k]);
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
                save_png(tile_images(personal_face),output);
            }
        }
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}
