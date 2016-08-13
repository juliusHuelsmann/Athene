#ifndef _UTILS_CPP_
#define _UTILS_CPP_


  #include <string>
  #include <iostream>
  #include <opencv2/opencv.hpp>
  #include <fstream>
  #include "Constants.h"
  
  using namespace std;
  using namespace cv;



  float getNormPowMax(Mat input);
  void writeToFile(char* name, Mat text);
  Mat calcDirectionalGrad(Mat& image, double sigma);
  Mat rotateAndScale(Mat& image, double angle, double scale);
  void circShift(Mat& in, Mat& out, int dx, int dy);
  void showImage(Mat& img, string win, double dur);
void showBin(Mat& img, string win, double dur);
  std::string toString(int i);



#endif
