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

  using namespace cv;
  using namespace std;
  
  static int histStep = 17; 
  static int histSize = 255 / histStep;
  
  class Preprocessing {

    private:
      
      int thresh1;
      Mat source;

      //
      // Surrounding rectangle. Might be used in the future.
      int minR;
      int maxR;
      int minC;
      int maxC;
      
      // Taks:        Get background - Foureground relationship.
      //
      // For all approaches it is firstly necessary only to regard the sgement
      // that has been activated by the preprocessing.
      //
      // Approach 1:  Generate Histogram (with low resolution) and take the local
      //                                  two biggest maxima.
      //              The histogram is generated on-the-fly in percecution method.
      int localHistogram [1][2][3];

      void derivative(Mat src, Mat& dst);
      void percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol);
      bool startPercecution(Mat& binResult, int row, int col, Mat orig, int& shiftRow, double& stretch);

      void percecuteTwo(Mat&, Mat, Vec3b, int, int, int&, int&, int&, int&, bool, bool, bool);
      void extractLetters(Mat, Mat, vector<Mat>&, int, double, Vec3b, bool, bool, bool);
    
    public:
      Preprocessing(Mat xsource);
      ~Preprocessing();
      void extractSegment(int y, int x);
  };

  
#endif
