using namespace cv;
Mat _cropFaceImagebyEYE(Mat inIm, Point2f EyelocL, Point2f EyelocR, int cropWidth, int cropHeight, float feyewidRatio = 0.4f, float feyehLevel = 0.38f);
Mat _eye2cvMat(Point2f lp, Point2f rp);
