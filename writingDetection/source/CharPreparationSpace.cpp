//============================================================================
// Name        : Aia3.cpp
// Author      : Ronny Haensch
// Version     : 1.0
// Copyright   : -
// Description : 
//============================================================================

#include "CharPreparationSpace.h"
//
// constructor: Stores the specified values and calls function for
//              Computing the houghPreparation using the specified values.
CharPreparationSpace::CharPreparationSpace(Vec2d ximgSize, vector<double> xscales, 
    vector<double> xrotations, std:: string ximg) {

  //
  // store values
  this->scales = xscales;
	this->rotations = xrotations;
	this->imgSize = ximgSize;

  //
  // load image
  this->offlineImg = imread(ximg, 0);
  if (!this->offlineImg.data){
		std:: cerr << "Error loading img from \t" << ximg << "\n";
		std:: cerr << "The program exits now.";
	  std:: cin.get();
		exit(-1);
  }
  
  // convert 8U to 32F
  this->offlineImg.convertTo(this->offlineImg, CV_32FC1);

  preprocess();
}
	
//
// constructor: Stores the specified values and calls function for
//              Computing the houghPreparation using the specified values.
CharPreparationSpace::CharPreparationSpace(Vec2d ximgSize, std:: string ximg) {

      vector<double> scales;
      vector<double> rotations;
      Vec2d imgSize;
      Mat offlineImg;
  //
  // store value
  this->imgSize = ximgSize;

  //
  // Compute normal values
  this->scales = vector<double>(8);
	int rScaleHalf = this->scales.size() / 2;
  for (int j = 0; j < rScaleHalf; j++){
    this->scales[j] = rScaleHalf - j;
    this->scales[j + rScaleHalf] = 1.0 / (j + 2);
  }

  
	this->rotations = vector<double>(1);
	this->rotations[0] = 0;
	

  //
  // load image
  this->offlineImg = imread(ximg, 0);
  if (!this->offlineImg.data){
		std:: cerr << "Error loading img from \t" << ximg << "\n";
		std:: cerr << "The program exits now.";
	  std:: cin.get();
		exit(-1);
  }
  
  // convert 8U to 32F
  this->offlineImg.convertTo(this->offlineImg, CV_32FC1);

  preprocess();
}
		            



void CharPreparationSpace::preprocess() {


  // generate template from template image
  // templ[0] == binary image
  // templ[0] == directional gradient image
  vector<Mat> templ = makeObjectTemplate();
  
  std:: cout << offlineImg.rows << ", " << offlineImg.cols << "\n";
  std:: cout << templ[0].rows << ", " << templ[0].cols << "\n";
  std:: cout << templ[1].rows << ", " << templ[1].cols << "\n";

  #ifdef DEBUG_CHARPREPARATIONSPACE
    showImage(templ[0], "Binary part of template", 0);
    Mat planes[2];
    split(templ[1], planes);
    showImage(planes[0], "gradient image0", 0);
    showImage(planes[1], "gradient image1", 0);
  #endif
  // perfrom general hough transformation
  houghPreparation = generalHough(templ);

}








// computes the hough space of the general hough transform
/*
  gradImage:  the gradient image of the test image
  templ:    the template consisting of binary image and complex-valued directional gradient image
  scaleSteps:  scale resolution
  scaleRange:  range of investigated scales [min, max]
  angleSteps:  angle resolution
  angleRange:  range of investigated angles [min, max)
  return:    the hough space: outer vector over scales, inner vector of angles
*/
vector< vector<Mat> > CharPreparationSpace::generalHough(vector<Mat>& templ){
  
  
  vector< vector<Mat> > hough = vector< vector<Mat> >(); 
  
  
  // For loop that goes through all different scales
  //    (si)  scale index [0-scaleSteps]
  //    (s )  the scale factor.
  for (int si = 0; si < scales.size(); si++) {
    double s = scales[si]; 
    vector<Mat> scaleVec = vector<Mat>();
  
    // For loop that goes through all different angles
    //    (ai)  angle index [0-scaleSteps]
    //    (a )  the angle factor.
    for (int ai = 0; ai < rotations.size(); ai++) {
      double a = rotations[ai];
      
      #ifdef DEBUG_CHARPREPARATIONSPACE
      std:: cout << "anlge:" << a << "\tscale" << s << "\t";
      std:: cout << "percA:" << 100*ai/rotations.size()
                 << "\tpercS" << 100*si/scales.size() << "\n";
      #endif
      
  
      //
      // make FFT Object mask
      Mat fftMask = Mat::zeros(imgSize[0], imgSize[1], CV_32FC2);//TODO: comment the init. out if that works.
      makeFFTObjectMask(templ, s ,a, fftMask);
      scaleVec.push_back(fftMask);

      /*
      // This code snipplet will be used for the voting procedure in case
      // the image is there.
      

      //
      // multiply spectrum (transpose == true) 
      // the dftGradImage is the dft of the gradient image that is checked
      // for detecting characters.
      Mat result1 = Mat::zeros(fftMask.rows, fftMask.cols, fftMask.type());
      cv::mulSpectrums(dftGradImage.clone(), fftMask.clone(), result1, 0, true);



      // 
      // Remove the imaginary part of the soulution
      Mat result = Mat::zeros(result1.rows, result1.cols, CV_32FC2);
      dft(result1, result, DFT_INVERSE + DFT_SCALE + DFT_COMPLEX_OUTPUT);
      Mat resultC1 = Mat::zeros(result.rows, result.cols, CV_32FC1);
      for (int corow = 0; corow < result1.rows; corow++) {
        for (int cocol = 0; cocol < result1.cols; cocol++) {
          resultC1.at<float>(corow, cocol) =  abs(result.at<Vec2f>(corow, cocol)[0]);
        }
      }
      
      // resultC1 is the (current) voting space!
      */
      
    }
    hough.push_back(scaleVec);
  }
  
  return hough;
}




// creates the fourier-spectrum of the scaled and rotated template
/*

Rotieren von U_*
Scale
Normalize

U := U_b*U_i (componentwise)
   => dft(U);



  templ:  the object template; binary image in templ[0], complex gradient in templ[1]
  scale:  the scale factor to scale the template
  angle:  the angle to rotate the template
  fftMask:  the generated fourier-spectrum of the template (initialized outside of this function)
*/
void CharPreparationSpace::makeFFTObjectMask(vector<Mat> templ, double scale, double angle, Mat& fftMask) {

  

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////Rotate and Scale////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //
  // Store the template for not altering the vector with new oritentation and 
  // scale.
  Mat temptemp0 = templ[0].clone();
  Mat temptemp1 = templ[1].clone();
  
  
  //
  // STEP A) Rotate and scale both images
  Mat tempRot0 = rotateAndScale(temptemp0, angle, scale);
  Mat tempRot1 = rotateAndScale(temptemp1, angle, scale);
  
  
  // STEP A2) adapt the edge directions.
  Mat matMagnitude, matAngle;
  vector<Mat> channels(2);
  split(tempRot1, channels);
  Mat real = channels[0];
  Mat imag = channels[1];
 
  cartToPolar(real, imag, matMagnitude, matAngle, false); //TODO: Radian!!!
  
  matAngle = matAngle + Mat::ones(matAngle.rows, matAngle.cols, matAngle.type()) * angle; 

  polarToCart(matMagnitude, matAngle, real, imag, false);
  vector<Mat> cartChannels;
  cartChannels.push_back(real);
  cartChannels.push_back(imag);
  merge(cartChannels, tempRot1);
  
  
  
  
  int sizeR=tempRot1.rows / 2.0;
  int sizeC=tempRot1.cols / 2.0;
  
  
  
  
  
  

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////Larger Matrix and multiplication template///////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //Generate O_i * O_j (get the gradient of those pixels that passed 
  //                    accoring to the binary threshold computation)
  Mat ooo = Mat::zeros(fftMask.rows, fftMask.cols, CV_32FC2); 
  for (int corow = 0; corow < tempRot1.rows; corow++) {
    for (int cocol = 0; cocol < tempRot1.cols; cocol++) {
      float d =(tempRot0.at<float>(corow, cocol));
      ooo.at<Vec2f>(corow, cocol) = Vec2f(1.0 * tempRot1.at<Vec2f>(corow, cocol)[0] * d, 1.0 * tempRot1.at<Vec2f>(corow, cocol)[1] * d);
      
    }
  }
  
  
  
  
  
  
  
  
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////// NORMALIZATION/////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  
  //
  // get the sum for normalization      
  float abssum = 0.0;
  for (int corow = 0; corow < ooo.rows; corow++) {
    for (int cocol = 0; cocol < ooo.cols; cocol++) {
       float cx = ooo.at<Vec2f>(corow, cocol)[0];
       float cy = ooo.at<Vec2f>(corow, cocol)[1];
       abssum =  1.0 * abssum + 1.0 * sqrt(cx * cx + cy * cy);
    }
  }
  if (abssum == 0)  {
    std:: cout << "The program is going to crash due to division by zero error";
    exit(1);
  }
  ooo = ooo / abssum;
  
  
  
  
  
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////// Centring//////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  Mat outputOO = Mat::zeros(ooo.rows, ooo.cols, ooo.type());
  circShift(ooo, outputOO, -sizeC,  -sizeR); // in, out, cols, rows
  
  


      Mat matRet = Mat(outputOO.rows, outputOO.cols, outputOO.type());
    
    
  
  
  
      dft(outputOO, matRet, DFT_COMPLEX_OUTPUT);// filter
      fftMask = matRet;



}





















// creates object template from template image
/*

  Create template that is used for further processing. See return value and
  parameters.
  

  @param templateImage    The template image (grayscale, 32fc1, contains object
                          of interest).

  return:                 vector (template) consisting of 
                            [0] binary edge mask          CV_8U
                            [1] complex gradient image    CV_32FC2
*/
vector<Mat> CharPreparationSpace::makeObjectTemplate() {

  // 
  // Initalize the vector and the matrixes that will be added later on.
  vector<Mat> vecTemplate = vector<Mat>();
  Mat complexGradientImage = calcDirectionalGrad(offlineImg, SIGMA);
  Mat binaryEdgeMask = Mat::zeros(offlineImg.rows, offlineImg.cols, CV_32FC1); 

  // only important for increasing the speed of computation. don't compute the
  // norm inside the loop, but take both sides of the equation by the power of 
  // two
  float powTB = pow(TEMPLATE_TRESH, 2);
  float maxVal = getNormPowMax(complexGradientImage); //TODO: max 2 normed value

  //
  // fill the binary edge mask  
  for (int r = 0; r < complexGradientImage.rows; r++) {
    for (int c = 0; c < complexGradientImage.cols; c++) {
      float dx = complexGradientImage.at<Vec2f>(r, c)[0];
      float dy = complexGradientImage.at<Vec2f>(r, c)[1];
      bool d =  (dx*dx + dy*dy > powTB * maxVal);
      binaryEdgeMask.at<float>(r, c) = 255 * d;
    }  
  }
  
  //
  // Insert the computed matrixes and return the template vector
  vecTemplate.push_back(binaryEdgeMask);
  vecTemplate.push_back(complexGradientImage); 
  return vecTemplate;
}






