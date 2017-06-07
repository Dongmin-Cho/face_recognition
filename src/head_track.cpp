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
#include <dlib/clustering.h>
#include <cstdio>
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
//----------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{  
    try
    {
        if (argc == 1)
        {
            cout << "Call this program like this:" << endl;
            cout << "./face_landmark_detection_ex faces/*.jpg" << endl;
            cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
            cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
            return 0;
        }

        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor sp;
        deserialize("shape_predictor_68_face_landmarks.dat") >> sp;


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
            //cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;

            
            load_image(img, argv[i]);
            cv::Mat im = cv::imread(argv[i]);
            // Make the image larger so we can detect small faces.
            pyramid_up(img);
            
            // Camera internals
            double focal_length = im.cols; // Approximate focal length.
            Point2d center = cv::Point2d(im.cols/2,im.rows/2);
            cv::Mat camera_matrix = (cv::Mat_<double>(3,3) << focal_length, 0, center.x, 0 , focal_length, center.y, 0, 0, 1);
            cv::Mat dist_coeffs = cv::Mat::zeros(4,1,cv::DataType<double>::type); // Assuming no lens distortion
            /*사람들 시선이 집중되어야 하는 영역이 있을거니까
              대략 그 구역 내로 u,v좌표가 나오면 잘 보고 있는 것으로... ㅇㅇ
              칠판이나 스크린은 점이 아니라 면이니까 각 사람들마다 u,v 거리가 좀 있대도 괜찮을 것 같고
              해당 영역에 집중하면 반드시 얼굴 방향벡터(u,v)좌표가 넘지 못하는 한계가 있을테니....?*/                    
            //cout << "Camera Matrix " << endl << camera_matrix << endl ;
            // Output rotation and translation
            cv::Mat rotation_vector; // Rotation in axis-angle form
            cv::Mat translation_vector;

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            std::vector<rectangle> dets = detector(img);
           // cout << "Number of faces detected: " << dets.size() << endl;
            if(dets.size() ==0){
             cout<<"-1"<<endl;
             return -1;
             // 검출 0명.
            }
            // Now we will go ask the shape_predictor to tell us the pose of
            // each face we detected.
            std::vector<full_object_detection> shapes;
            typedef matrix<double,2,1> sample_type;
            typedef radial_basis_kernel<sample_type> kernel_type;
            int kc_num = sqrt(dets.size()/2);
            if(kc_num<2){
                if(dets.size()==1){
                    cout<<"100%"<<endl;
                    return 0;
                }
                else{
                kc_num =2;
                }
            }
            kcentroid<kernel_type> kc(kernel_type(0.1),0.01,kc_num);
            kkmeans<kernel_type> test(kc);

            std::vector<sample_type> samples;
            std::vector<sample_type> initial_centers;

            sample_type m;
            
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                full_object_detection shape = sp(img, dets[j]);
               // cout << "number of parts: "<< shape.num_parts() << endl;
                
                std::vector<cv::Point2d> image_points = get_2d_image_points(shape);
                
                // Solve for pose
                cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);
                // Project a 3D point (0, 0, 1000.0) onto the image plane.
                // We use this to draw a line sticking out of the nose
                     
                std::vector<cv::Point3d> nose_end_point3D;
                std::vector<cv::Point2d> nose_end_point2D;
                nose_end_point3D.push_back(cv::Point3d(0,0,1000.0));
                
                projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);
     

                //cout << "Rotation Vector " << endl << rotation_vector << endl;
                //cout << "Translation Vector" << endl << translation_vector << endl;
                             
               // cout <<  nose_end_point2D << endl;// this is what we want!!!!
                
                //make some samples near the origin
                
              
                m(0) =sqrt(fabs(center.x*center.x - nose_end_point2D[0].x*nose_end_point2D[0].x));
                m(1) =sqrt(fabs(center.y*center.y - nose_end_point2D[0].y*nose_end_point2D[0].y));
                samples.push_back(m);
            
            /*  
                ofstream fout;
                fout.open("points.txt",ios::app);
                
                fout<<nose_end_point2D<<endl;
                fout.close();
            */
                shapes.push_back(sp(img,dets[j]));

            }
             // tell the kkmeans object we made that we want to run k-means with k set to 3.
            test.set_number_of_centers(kc_num);
            if(kc_num<samples.size()){
                pick_initial_centers(kc_num, initial_centers, samples, test.get_kernel());
            }
            else{
                initial_centers = samples;
            }
            test.train(samples, initial_centers);
            int focus_count[kc_num]={0};
            for (unsigned long i = 0; i < samples.size(); ++i){
              
                focus_count[test(samples[i])]++;
            }
           
            int max = focus_count[0];
            for(int i=1;i<kc_num;i++){
                if(max<focus_count[i])
                    max = focus_count[i];
            }
            string focus;
            char percent[6];
            sprintf(percent,"%.2f",((float)max/samples.size()*100));
            focus = percent;
            focus+="%";
            cout<<focus<<endl;
            std::vector<unsigned long> assignments = spectral_cluster(kernel_type(0.1), samples, kc_num);
           // cout<<mat(assignments)<<endl;
            // Now let's view our face poses on the screen.
           // win.clear_overlay();
           // win.set_image(img);
           // win.add_overlay(render_face_detections(shapes));

            // We can also extract copies of each face that are cropped, rotated upright,
            // and scaled to a standard size as shown here:
            //dlib::array<array2d<rgb_pixel> > face_chips;
            //extract_image_chips(img, get_face_chip_details(shapes), face_chips);
            //win_faces.set_image(tile_images(face_chips));
            

           // cout << "Hit enter to process the next image..." << endl;
           // cin.get();
        }
    }
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

// ----------------------------------------------------------------------------------------


