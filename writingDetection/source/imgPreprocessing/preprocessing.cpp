
#include "preprocessing.h"

void preprocessing(Mat ximg) {
  
  int threshold = 200;
  int thresholdMin = 50;
  
  Mat complexGradientImage = calcDirectionalGrad(ximg, SIGMA);
  Mat gradResult = Mat::zeros(ximg.rows, ximg.cols, CV_8UC1);
  std:: cout << complexGradientImage.type() << ", "<< CV_64FC2 << "\n";
  std:: cout << complexGradientImage.type() << ", "<< CV_16SC2 << "\n";
  std:: cout << complexGradientImage.type() << ", "<< CV_16UC1 << "\n";
  std:: cout << complexGradientImage.type() << ", "<< CV_16SC1 << "\n";
  std:: cout << complexGradientImage.type() << ", "<< CV_32FC2 << "\n";
  std:: cout << complexGradientImage.type() << ", "<< CV_32FC1 << "\n";
  exit(1);
  for (int i = 0; i < gradResult.rows; i++) {
    for (int j = 0; j < gradResult.cols; j++) {
      if (complexGradientImage.at<Vec2f>(i,j)[0] >= threshold
          || gradResult.at<Vec2f>(i,j)[1] >= threshold) {
        gradResult.at<uchar>(i, j) = 1;
      } else if (complexGradientImage.at<Vec2f>(i,j)[0] <= thresholdMin) {
        gradResult.at<uchar>(i - 1, j) = 1;
      } else if (complexGradientImage.at<Vec2f>(i,j)[1] <= thresholdMin) {
        gradResult.at<uchar>(i, j - 1) = 1;
      }
    }
  }
  showImage(gradResult, "gradResult", 0);
  
  
  


}
