
#include "preprocessing.h"

void preprocessing(Mat ximg) {
  
  
  int threshold = 200;
  int thresholdMin = -30;
  
  ximg.convertTo(ximg, CV_32FC1);
  
  Mat complexGradientImage = calcDirectionalGrad(ximg, 0.3);
  vector<Mat> grad;
  split(complexGradientImage, grad);
  int detect = 255;
  Mat gradResult = Mat::zeros(ximg.rows, ximg.cols, CV_8UC1);
  for (int i = 0; i < gradResult.rows; i++) {
    for (int j = 0; j < gradResult.cols; j++) {
      if (grad[0].at<Vec3b>(i,j)[0] >= threshold
          || grad[0].at<Vec3b>(i,j)[1] >= threshold
          || grad[0].at<Vec3b>(i,j)[2] >= threshold
          || grad[1].at<Vec3b>(i,j)[0] >= threshold
          || grad[1].at<Vec3b>(i,j)[1] >= threshold
          || grad[1].at<Vec3b>(i,j)[2] >= threshold) {
        gradResult.at<uchar>(i, j) = detect;
      } else if (i > 0 && (grad[0].at<Vec3b>(i,j)[0] <= thresholdMin
          || grad[0].at<Vec3b>(i,j)[1] <= thresholdMin
          || grad[0].at<Vec3b>(i,j)[2] <= thresholdMin)) {
        gradResult.at<uchar>(i - 1, j) = detect;
      } else if (j > 0 && (grad[1].at<Vec3b>(i,j)[0] <= thresholdMin
          || grad[1].at<Vec3b>(i,j)[1] <= thresholdMin
          || grad[1].at<Vec3b>(i,j)[2] <= thresholdMin)) {
        gradResult.at<uchar>(i, j - 1) = detect;
      }
    }
  }
  showImage(gradResult, "gradResult", 0);
  

}
