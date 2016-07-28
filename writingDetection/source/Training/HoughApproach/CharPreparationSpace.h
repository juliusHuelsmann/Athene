//============================================================================
// Name        : Preparation.h
// Author      : Julius Huelsmann
// Version     : 0.0
// Description : Header file of the class CharPreparationSpace that contains the
//               functionality for generating a scale / and rotation invariant
//               hough voting space for one character
//               that may be used for direct corollation 
//               approach to hough voting. 
// 
//               Requires:
//                 a) Size of the image that is to be generated
//                 b) Different scales, rotations 
//                 c) Path to the sample image data (vector<String>)  
//               Computes:
//                 i) vector<vector <Mat> > hough voting space to be multiplied
//                    with the image that is to be checked. Stores it inside
//                    the class
//               
//               Is part of a c++ project that is communicating with a java
//               program. It receives an approximate location from the java
//               program and extracts the text that is displayed at specified
//               location on screen.
//
//               Problems / Improvements
//                 1) the program won't be able to adapt to different screen 
//                    sizes. Once started, the program has to react to 
//                    different monitor settings / new monitors branched
//                    / different workspace used.
//                 
//                    !!! 
//                        This has been misunderstood. I think i need to extract
//                        snipplets. Their size differes often. Max snipplet 
//                        size!
//                    !!! 
//                    
//                 
//============================================================================

#ifndef _CHAR_PREPARATION_SPACE_
#define _CHAR_PREPARATION_SPACE_

  #include <string>
  #include <iostream>
  #include <opencv2/opencv.hpp>
  #include <opencv2/core/core.hpp>
  #include <opencv2/highgui/highgui.hpp>
  #include <fstream>
  #include "../../Utils/Constants.h"
  #include "../../Utils/Utils.h"

  using namespace std;
  using namespace cv;


  class CharPreparationSpace {

	  public:
	
      //
      // constructor: Stores the specified values and calls function for
      //              Computing the houghPreparation using the specified values.
		  CharPreparationSpace(Vec2d ximgSize, vector<double> xscales, 
		              vector<double> xrotations, string ximg, char xid);
      //
      // constructor: Stores the specified values and calls function for
      //              Computing the houghPreparation using the specified values.
      //              Automatically generates suitable scales and rotations
      //              for the problem.
		  CharPreparationSpace(Vec2d ximgSize, string ximg, char xid);
		              
		  //
		  // destructor
		  ~CharPreparationSpace(void){};
		
		  
		  // 
		  // For image recognition: toDetect is already fourier transformed
		  // !
      vector< vector<Mat> > getHoughResult(Mat toDetect);
      void plotHoughDetectionResult(Mat& testImage, vector<Scalar>& objList);
      void findHoughMaxima(vector< vector<Mat> >& houghSpace, vector<Scalar>& objList);
      
      //
      // Getter methods
      //
      
      // The hough voting space preparation will be stored inside this value
		  vector<vector<Mat> > getHoughPreparation();
		  
		  // Unimportant stuff
      vector<double> getScales();
      vector<double> getRotations();
      
    private:
    
      //
      // The hough voting space preparation will be stored inside this value
      vector<vector<Mat> > houghPreparation;
      
      //
      // Parameters that are stored and will be used for computation
      vector<double> scales;
      vector<double> rotations;
      Vec2d imgSize;
      Mat offlineImg;
      char id; //The identifier of given character.
      vector<Mat> templ;
      
      //
      // Functions for processing
      vector<Mat> makeObjectTemplate();
      void makeFFTObjectMask(double scale, double angle, Mat& fftMask);
      vector< vector<Mat> > generalHough();
      void preprocess();
  
      
  };

#endif
