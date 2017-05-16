#define PADDING 0.35
#define SIMILARITY 0.35

#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <fstream>

using namespace dlib;
using namespace std;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET> 
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------

std::vector<matrix<rgb_pixel>> jitter_image(
    const matrix<rgb_pixel>& img
);

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv) try
{
    // The first thing we are going to do is load all our models.  First, since we need to
    // find faces in the image we will need a face detector:
    frontal_face_detector detector = get_frontal_face_detector();
    // We will also use a face landmarking model to align faces to a standard pose:  (see face_landmark_detection_ex.cpp for an introduction)
    shape_predictor sp;
    deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
    // And finally we load the DNN responsible for face recognition.
    anet_type net;
    deserialize("dlib_face_recognition_resnet_model_v1.dat") >> net;

	for(int i=1; i<argc; i++){
    	std::vector<matrix<rgb_pixel>> faces;
    	matrix<rgb_pixel> img;
	
    	std::string path = argv[i];
		cout<<path<<endl;
    	load_image(img, argv[i]);
		
		
    	// Run the face detector on the image of our action heroes, and for each face extract a
   		// copy that has been normalized to 150x150 pixels in size and appropriately rotated
    	// and centered.
    	for (auto face : detector(img))
    	{
        	auto shape = sp(img, face);
        	matrix<rgb_pixel> face_chip;
        	extract_image_chip(img, get_face_chip_details(shape,150,PADDING), face_chip);
        	faces.push_back(move(face_chip));
        	// Also put some boxes on the faces so we can see that the detector is finding
        	// them.
   		}

    	if (faces.size() == 0)
    	{
        	cout << "No faces found in image!" << endl;
    	}
		else{
			// This call asks the DNN to convert each face image in faces into a 128D vector.
    		// In this 128D vector space, images from the same person will be close to each other
   			// but vectors from different people will be far apart.  So we can use these vectors to
    		// identify if a pair of images are from the same person or from different people.  
    		std::vector<matrix<float,0,1>> face_descriptors = net(faces);

			//.txt	
			int index = path.find(".jpg");
			string fileName = path;
			fileName = fileName.erase(index,fileName.length()-index);
    		for(size_t i = 0; i < face_descriptors.size(); i++)
   			{
        		ofstream out(fileName+to_string(i)+".txt");
				if(out.is_open()){
					out << face_descriptors[i];
				}
				cout<<trans(face_descriptors[i])<<endl;
    		}
		}
	}
}
catch (std::exception& e)
{
    cout << e.what() << endl;
}
