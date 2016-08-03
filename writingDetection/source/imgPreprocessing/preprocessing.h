#ifndef _PREPROCESSING_H_
#define _PREPROCESSING_H_


  #include <string>
  #include <iostream>
  #include <opencv2/opencv.hpp>
  #include <opencv2/core/core.hpp>
  #include <opencv2/highgui/highgui.hpp>
  #include <fstream>
  #include "../Utils/Constants.h"
  #include "../Utils/Utils.h"
  #include "opencv2/imgproc/imgproc.hpp"
  #include <stdlib.h>
  #include <stdio.h>

  class Preprocessing {

    private:
      
      int thresh1;
      Mat source;
      void edges(Mat src, Mat& dst);
      void percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol);
void startPercecution(Mat& binResult, int row, int col, Mat orig, int& shiftRow, double& stretch);
    
    public:
      Preprocessing(Mat xsource);
      ~Preprocessing();
      void extractSegment(Mat& src_gray, int y, int x);
  };

  
#endif
