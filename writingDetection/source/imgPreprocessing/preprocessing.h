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
  
  #define HIST_STEP 16 
  #define HIST_SIZE 256 / HIST_STEP
  
  class Preprocessing {

    private:
      
      
      
      
      //
      // The entire images processed and not processed.
      //
      
      /**
       * The source matrix.
       * Type = CV_8UC3
       */
      Mat source;
      
      /**
       * The cranerryThresholdedImage.
       * Type = CV_8UC1
       */
      Mat cran;
      
      
      
      
      
      
      //
      // Small Segment values 
      //
      
      /**
       * The binary image of the small segment.
       * Type = CV_8UC1.
       * 255 = activated
       * 2 = not yet visited
       * 0 = not in scope
       */
      Mat smallSegmentBin;
      
      /**
       * Derivative image of the small segment taken from source
       */
      Mat smallSegmentDer;
      
      /**
       * The displacement of the smallSegment in row
       * (smallSegment*.at(r, c) ^ source.at(r + smallSegmentRowDisplace, c);
       * in case the image was not stretched.
       * 
       * Formular including smallSegmentStretch
       * Abbrev:  smallSegmentStretch =: str, smallSegmentRowDisplace =: rd
       *          smallSegment =: sS, source =: src
       *
       *    sS (r , c) ^ src(r / str + rd, c / str);
       *    src(r , c) ^ sS ((r - rd) * str, c * str);
       * 
       */
      int smallSegmentRowDisplace;
      
      /**
       * The stretch factor in both row and column.
       */
      double smallSegmentStretch;
      
      
      
      
      
      //
      //
      //
      
      
      /**
       * Surrounding rectangle.
       */
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
      int localHistogram [HIST_SIZE][HIST_SIZE][HIST_SIZE]; 
      
      
      // histogram which indicates the propability of a RGB combi to be 
      // foreground
      float probHistogram [HIST_SIZE][HIST_SIZE][HIST_SIZE]; 
      
      
      
      
      
      
      
      //
      // Other values that are used for computation
      //
      
      /**
       * The threshold for the first part of the segmentation with the 
       * Canerry image.
       */      
      int derivThreshold;
      
      
      /**
       * The location of the user's click.
       */
      int row, col;
      



      //////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////
      ///////////////////////////FUNCTIONS//////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////


      void extractSegment();
        bool startPercecution( int row, int col);
          void derivative(Mat src, Mat& dst);
          void percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol);


        void percecuteTwo(Mat&, Mat, Vec3b, int, int, int&, int&, int&, int&, bool, bool, bool);
        void extractLetters(Mat, Mat, vector<Mat>&, int, double, Vec3b, bool, bool, bool);
     
     

      //
      // Testing
      void printHistogram();    
    
    
    
    
    
    
        
      //
      // Utilities  
      
      /**
       * Convert coordinates of entire image to coordinates of small-segment img.
       * @see smallSegmentRowDisplace
       */
      void cvtEC2SC(int eR, int eC, int& sR, int& sC);
      
      /**
       * Convert coordinates of small-segment-image to coordinates of entire img.
       * @see smallSegmentRowDisplace
       */
      void cvtSC2EC(int sR, int sC, int& eR, int& eC);
      
      
      /**
 * (r,c) -> (cvtEc2SC(r,c)) + (h_r, w_c) 
       */
      void cvtEC2Letter(int eR, int eC, int& lR, int& lC);
            
    public:
      Preprocessing(Mat xsource, int row, int col);
      ~Preprocessing();
  };

  
#endif
