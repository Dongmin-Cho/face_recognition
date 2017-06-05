#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <dlib/opencv.h>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
using namespace cv;
using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------
std::vector<cv::Point2d> get_2d_image_points(full_object_detection &d)
{
      std::vector<cv::Point2d> image_points;
      image_points.push_back( cv::Point2d( d.part(30).x(), d.part(30).y() ) );    // Nose tip
      image_points.push_back( cv::Point2d( d.part(8).x(), d.part(8).y() ) );      // Chin
      image_points.push_back( cv::Point2d( d.part(36).x(), d.part(36).y() ) );    // Left eye left corner
      image_points.push_back( cv::Point2d( d.part(45).x(), d.part(45).y() ) );    // Right eye right corner
      image_points.push_back( cv::Point2d( d.part(48).x(), d.part(48).y() ) );    // Left Mouth corner
      image_points.push_back( cv::Point2d( d.part(54).x(), d.part(54).y() ) );    // Right mouth corner
      return image_points;
}
int main(int argc, char** argv)
{  
    try
    {
        // This example takes in a shape model file and then a list of images to
        // process.  We will take these filenames in as command line arguments.
        // Dlib comes with example images in the examples/faces folder so give
        // those as arguments to this program.
        if (argc == 1)
        {
            cout << "Call this program like this:" << endl;
            cout << "./face_landmark_detection_ex faces/*.jpg" << endl;
            cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
            cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
            return 0;
        }

        // We need a face detector.  We will use this to get bounding boxes for
        // each face in an image.
        frontal_face_detector detector = get_frontal_face_detector();
        // And we also need a shape_predictor.  This is the tool that will predict face
        // landmark positions given an image and face bounding box.  Here we are just
        // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
        // as a command line argument.
        shape_predictor sp;
        deserialize("shape_predictor_68_face_landmarks.dat") >> sp;


        image_window win, win_faces;

        std::vector<cv::Point3d> model_points;
        model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));
        model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));
        model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));
        model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));
        model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
        model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));
        std::vector<cv::Point3d> face_direction;
        // Loop over all the images provided on the command line.
        for (int i = 1; i < argc; ++i)
        {
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;

            
            load_image(img, argv[i]);
            cv::Mat im = cv::imread(argv[i]);
            // Make the image larger so we can detect small faces.
            pyramid_up(img);
            
            ///////////////////////////////////////////////////
            // Camera internals
            double focal_length = im.cols; // Approximate focal length.
            Point2d center = cv::Point2d(im.cols/2,im.rows/2);
            cv::Mat camera_matrix = (cv::Mat_<double>(3,3) << focal_length, 0, center.x, 0 , focal_length, center.y, 0, 0, 1);
            cv::Mat dist_coeffs = cv::Mat::zeros(4,1,cv::DataType<double>::type); // Assuming no lens distortion
                                
            //cout << "Camera Matrix " << endl << camera_matrix << endl ;
            // Output rotation and translation
            cv::Mat rotation_vector; // Rotation in axis-angle form
            cv::Mat translation_vector;

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            std::vector<rectangle> dets = detector(img);
            cout << "Number of faces detected: " << dets.size() << endl;

            // Now we will go ask the shape_predictor to tell us the pose of
            // each face we detected.
            std::vector<full_object_detection> shapes;
            
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                full_object_detection shape = sp(img, dets[j]);
                cout << "number of parts: "<< shape.num_parts() << endl;
                
                std::vector<cv::Point2d> image_points = get_2d_image_points(shape);
                
                // Solve for pose
                cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);
                // Project a 3D point (0, 0, 1000.0) onto the image plane.
                // We use this to draw a line sticking out of the nose
                     
                std::vector<cv::Point3d> nose_end_point3D;
                std::vector<cv::Point2d> nose_end_point2D;
                nose_end_point3D.push_back(cv::Point3d(0,0,1000.0));
                
                projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);
     
     
                //cv::line(im,image_points[0], nose_end_point2D[0], cv::Scalar(255,0,0), 2);
                     
                //cout << "Rotation Vector " << endl << rotation_vector << endl;
                //cout << "Translation Vector" << endl << translation_vector << endl;
                             
                cout <<  nose_end_point2D << endl;// this is what we want!!!! 
                if(center.x< nose_end_point2D[0].x){
                 if(center.y+500<nose_end_point2D[0].y){
                    cout<<"집중 중 맞음!"<<endl;
                 }
                 else{
                    cout<<"아래 봄"<<endl;
                 }
                }
                else{
                    cout<<"딴데 봄"<<endl;
                }
                
                ofstream fout;
                fout.open("points.txt",ios::app);
                
                fout<<nose_end_point2D<<endl;
                fout.close();

                // You get the idea, you can get all the face part locations if
                // you want them.  Here we just store them in shapes so we can
                // put them on the screen.
                shapes.push_back(sp(img,dets[j]));

            }

            // Now let's view our face poses on the screen.
           // win.clear_overlay();
           // win.set_image(img);
           // win.add_overlay(render_face_detections(shapes));

            // We can also extract copies of each face that are cropped, rotated upright,
            // and scaled to a standard size as shown here:
            //dlib::array<array2d<rgb_pixel> > face_chips;
            //extract_image_chips(img, get_face_chip_details(shapes), face_chips);
            //win_faces.set_image(tile_images(face_chips));
            

            cout << "Hit enter to process the next image..." << endl;
            cin.get();
        }
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------

