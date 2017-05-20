#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>

#define FACE_SIZE 224 
#define FACE_PADDING 0.8

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------
// This program normalize face in the photo and saves it *.jpg in the "output" directory
// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{  
    try
    {
        if (argc == 1)
        {
            cout << "call like this" << endl;
            cout << "./crop out_dir input_jpg_files" <<endl;
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
                cout << "number of parts: "<< shape.num_parts() << endl;
                cout << "pixel position of first part:  " << shape.part(0) << endl;
                cout << "pixel position of second part: " << shape.part(1) << endl;
                // You get the idea, you can get all the face part locations if
                // you want them.  Here we just store them in shapes so we can
                // put them on the screen.
                shapes.push_back(shape);
            }
            
            dlib::array<array2d<rgb_pixel> > face_chips;
            extract_image_chips(img,get_face_chip_details(shapes, FACE_SIZE, FACE_PADDING), face_chips);
           
            string output = argv[i];
            output.erase(0,output.find_last_of("/"));
            output = argv[1] + output;
            
            if(face_chips.size() != 0)
                for(size_t k = 0; k < face_chips.size(); k++)
                {
                    dlib::array<array2d<rgb_pixel> > personal_face;
                    personal_face.push_back(face_chips[k]);
                    output.erase(output.find(".")-1,output.size());
                    output += to_string(k+1) + ".jpg";
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
