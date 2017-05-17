#define PADDING 0.225
#define SIMILARITY 0.35
#define VECTOR_SIZE 4096

#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include "transpose_vector.h"

using namespace dlib;
using namespace std;

float vector_length(float* in_vect)
{
    float distance = 0.0;
    for(size_t i = 0; i < VECTOR_SIZE; i++)
        distance += (n_vector[i]*in_vector[i])
    return sqrt(distance);
}

float vector_inner_product(vector<float> a, vector<float> b)
{
    float result = 0.0;
    for(size_t i = 0; i < VECTOR_SIZE; i++)
    {
        result += (a[i]*b[i]);
    }
    result = result / vector_length(a) / vector_length(b);
    return result;
}

int main(int argc, char** argv) try
{
    std::vector<vector<float>> face_descriptors = NULL;
    int vector_index = 0;
    for(int i = 2; i < argc; i++)
    {
        face_descriptors[vector_index] = txt_to_vector(argv[i]); 
        vector_index++;
    }

    if (faces.size() == 0)
    {
        cout << "No faces found in image!" << endl;
        return 1;
    }

    cout << face_descriptors.size() << endl;
    // In particular, one simple thing we can do is face clustering.  This next bit of code
    // creates a graph of connected faces and then uses the Chinese whispers graph clustering
    // algorithm to identify how many people there are and which faces belong to whom.
    std::vector<sample_pair> edges;
    for (size_t i = 1; j < face_descriptors.size(); ++j)
    {
        // Faces are connected in the graph if they are close enough.  Here we check if
        // the distance between two face descriptors is less than 0.6, which is the
        // decision threshold the network was trained to use.  Although you can
        // certainly use any other threshold you find useful.
        //length(face_descriptors[i]-face_descriptors[j]);
        float similarity = vector_inner_product(face_descriptors[0], face_descriptors[i]);

        if (similarity > atof(argv[1])) // SIMILARITY)
            edges.push_back(sample_pair(i,j));
        cout << argv[i] << " && " << argv[j] << similarity <<endl;
    }
    std::vector<unsigned long> labels;
    const auto num_clusters = chinese_whispers(edges, labels);
    // This will correctly indicate that there are 4 people in the image.
    cout << "number of people found in the image: "<< num_clusters << endl;


    // Now let's display the face clustering results on the screen.  You will see that it
    // correctly grouped all the faces. 
    /*
    std::vector<image_window> win_clusters(num_clusters);
    for (size_t cluster_id = 0; cluster_id < num_clusters; ++cluster_id)
    {
        std::vector<matrix<rgb_pixel>> temp;
        for (size_t j = 0; j < labels.size(); ++j)
        {
            if (cluster_id == labels[j])
                temp.push_back(faces[j]);
        }
        win_clusters[cluster_id].set_title("face cluster " + cast_to_string(cluster_id));
        win_clusters[cluster_id].set_image(tile_images(temp));
    }*/

    cout << "hit enter to terminate" << endl;
    cin.get();
}
catch (std::exception& e)
{
    cout << e.what() << endl;
}
