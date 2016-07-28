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
    vector<double> xrotations, std:: string ximg, char xid) {

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
  this->id = xid;

  preprocess();
}
  
//
// constructor: Stores the specified values and calls function for
//              Computing the houghPreparation using the specified values.
CharPreparationSpace::CharPreparationSpace(Vec2d ximgSize, std:: string ximg,
    char xid) {

      vector<double> scales;
      vector<double> rotations;
      Vec2d imgSize;
      Mat offlineImg;
  //
  // store value
  this->imgSize = ximgSize;
  this->id = xid;
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


  //
  // Check if the parameters for the operation are valid
  if (offlineImg.rows > imgSize[0] || offlineImg.cols > imgSize[1]) {
    std:: cout << "Error. The imgSize " << imgSize << " is not suitable for"
               << " the training image size " << offlineImg.rows << ", "
               << offlineImg.cols << "." << "(needs to be >=)\n\n"
               << "The program exits NOW. \n";
    exit(-1);
  }


  // generate template from template image
  // templ[0] == binary image
  // templ[0] == directional gradient image
  this->templ = makeObjectTemplate();

  
  #ifdef DEBUG_CHARPREPARATIONSPACE
    showImage(templ[0], "Binary part of template", 0);
    Mat planes[2];
    split(templ[1], planes);
    showImage(planes[0], "gradient image0", 0);
    showImage(planes[1], "gradient image1", 0);
  #endif
  // perfrom general hough transformation
  houghPreparation = generalHough();

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
vector< vector<Mat> > CharPreparationSpace::generalHough(){
  
  
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
      std:: cout << "Angle:\t" << a << "\tScale:\t" << s << "\t";
      std:: cout << "percA:\t" << 100*ai/rotations.size()
                 << "\tpercS:\t" << 100*si/scales.size() << "\n";
      #endif
      
  
      //
      // make FFT Object mask
      Mat fftMask = Mat::zeros(imgSize[0], imgSize[1], CV_32FC2);//TODO: comment the init. out if that works.
      makeFFTObjectMask(s ,a, fftMask);
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
void CharPreparationSpace::makeFFTObjectMask(double scale, double angle, Mat& fftMask) {

  

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////Rotate and Scale////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //
  // Store the template for not altering the vector with new oritentation and 
  // scale.
  Mat temptemp0 = this->templ[0].clone();
  Mat temptemp1 = this->templ[1].clone();
  
  
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
      Vec2f nV  =Vec2f(1.0 * tempRot1.at<Vec2f>(corow, cocol)[0] * d, 1.0 * tempRot1.at<Vec2f>(corow, cocol)[1] * d);
      ooo.at<Vec2f>(corow, cocol) = nV;
      
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
    
    
  
  #ifdef DEBUG_CHARPREPARATIONSPACE
    Mat planes[2];
    split(outputOO, planes);
    showImage(planes[0], "Preprocessed img -> DFT Re", 0);
    showImage(planes[1], "Preprocessed img -> DFT Im", 0);
  #endif
  
  
  
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

























//
// CALLED IN ONLINE PHASE
//
//




// toDetect already foruier transformed (thus the ffd has only to be done
// once for all letters.
vector< vector<Mat> > CharPreparationSpace::getHoughResult(Mat toDetect) {
  
  
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
      std:: cout << "Angle:\t" << a << "\tScale:\t" << s << "\t";
      std:: cout << "percA:\t" << 100*ai/rotations.size()
                 << "\tpercS:\t" << 100*si/scales.size() << "\n";
      #endif
      
  
      //
      // make FFT Object mask
      Mat fftMask = houghPreparation[si][ai];


      //
      // multiply spectrum (transpose == true) 
      // the dftGradImage is the dft of the gradient image that is checked
      // for detecting characters.
      Mat result1 = Mat::zeros(fftMask.rows, fftMask.cols, fftMask.type());
      cv::mulSpectrums(toDetect.clone(), fftMask.clone(), result1, 0, true);



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
      
      scaleVec.push_back(resultC1);
      // resultC1 is the (current) voting space!
      
      
    }
    hough.push_back(scaleVec);
  }
  
  return hough;
}





// seeks for local maxima within the hough space
/*

*/
void CharPreparationSpace::findHoughMaxima(vector< vector<Mat> >& houghSpace, vector<Scalar>& objList){

    // get maxima over scales and angles
    Mat maxImage = Mat::zeros(houghSpace.at(0).at(0).rows, houghSpace.at(0).at(0).cols, CV_32FC1 );
    for(vector< vector<Mat> >::iterator it = houghSpace.begin(); it != houghSpace.end(); it++){
  for(vector<Mat>::iterator img = (*it).begin(); img != (*it).end(); img++){
      max(*img, maxImage, maxImage);
  }
    }
    // get global maxima
    double min, max;
    minMaxLoc(maxImage, &min, &max);

    // define threshold
    double threshold = OBJ_THRESH * max;

    // spatial non-maxima suppression
    Mat bin = Mat(houghSpace.at(0).at(0).rows, houghSpace.at(0).at(0).cols, CV_32FC1, -1);
    for(int y=0; y<maxImage.rows; y++){
    for(int x=0; x<maxImage.cols; x++){
      // init
      bool localMax = true;
      // check neighbors
      for(int i=-1; i<=1; i++){
        int new_y = y + i;
        if ((new_y < 0) or (new_y >= maxImage.rows)){
          continue;
        }
        for(int j=-1; j<=1; j++){
          int new_x = x + j;
          if ((new_x < 0) or (new_x >= maxImage.cols)){
          continue;
          }
          if (maxImage.at<float>(new_y, new_x) > maxImage.at<float>(y, x)){
          localMax = false;
          break;
          }
        }
        if (!localMax)
          break;
      }
      // check if local max is larger than threshold
      if ( (localMax) and (maxImage.at<float>(y, x) > threshold) ){
        bin.at<float>(y, x) = maxImage.at<float>(y, x);
      }
    }
    }
    
    // loop through hough space after non-max suppression and add objects to object list
    double scale, angle;
    scale = 0;
    for(vector< vector<Mat> >::iterator it = houghSpace.begin(); it != houghSpace.end(); it++, scale++){
    angle = 0;
    for(vector<Mat>::iterator img = (*it).begin(); img != (*it).end(); img++, angle++){
      for(int y=0; y<bin.rows; y++){
        for(int x=0; x<bin.cols; x++){
          if ( (*img).at<float>(y, x) == bin.at<float>(y, x) ){
          // create object list entry consisting of scale, angle, and position where object was detected
          Scalar cur;
          cur.val[0] = scale;
          cur.val[1] = angle;
          cur.val[2] = x;
          cur.val[3] = y;      
          objList.push_back(cur);
          }
        }
      }
    }
    }   
}




// shows the detection result of the hough transformation
/*
  testImage:  the test image, where objects were searched (and hopefully found)
  templ:    the template consisting of binary image and complex-valued directional gradient image
  objList:    list of objects as defined by findHoughMaxima(..)
  scaleSteps:  scale resolution
  scaleRange:  range of investigated scales [min, max]
  angleSteps:  angle resolution
  angleRange:  range of investigated angles [min, max)
*/
void CharPreparationSpace::plotHoughDetectionResult(Mat& testImage, vector<Scalar>& objList){


    // some matrices to deal with color
    Mat red = testImage.clone();
    Mat green = testImage.clone();
    Mat blue = testImage.clone();
    Mat tmp = Mat::zeros(testImage.rows, testImage.cols, CV_32FC1);
      
    // scale and angle of current object
    double scale, angle;

    
    int ju = 0;
    // for all objects
    for(vector<Scalar>::iterator it = objList.begin(); it != objList.end(); it++){
    
      std:: cout << ju * 100 / objList.size() << "\n";
      
      // compute scale and angle of current object
      scale = this->scales[it->val[0]];
      angle = this->rotations[it->val[1]];
      ju++;
      std:: cout << "a1\n";
      // use scale and angle in order to generate new binary mask of template
      std:: cout << this->templ[0].cols << ", " << this->templ[0].rows  << ", " << angle << ", "  << scale << "\n";
      Mat binMask = rotateAndScale(this->templ[0], angle, scale);
      std:: cout << binMask.cols << ", " << binMask.rows << "\n";

      // perform boundary checks
      Rect binArea = Rect(0, 0, binMask.cols, binMask.rows);
      std:: cout << "a2\n";
      Rect imgArea = Rect((*it).val[2]-binMask.cols/2., (*it).val[3]-binMask.rows/2, binMask.cols, binMask.rows);
      std:: cout << "a3\n";
      if ( (*it).val[2]-binMask.cols/2 < 0 ){
        binArea.x = abs( (*it).val[2]-binMask.cols/2 );
        binArea.width = binMask.cols - binArea.x;
        imgArea.x = 0;
        imgArea.width = binArea.width;
      }
      if ( (*it).val[3]-binMask.rows/2 < 0 ){
        binArea.y = abs( (*it).val[3]-binMask.rows/2 );
        binArea.height = binMask.rows - binArea.y;
        imgArea.y = 0;
        imgArea.height = binArea.height;
      }
      if ( (*it).val[2]-binMask.cols/2 + binMask.cols >= tmp.cols ){
        binArea.width = binMask.cols - ( (*it).val[2]-binMask.cols/2 + binMask.cols - tmp.cols );
        imgArea.width = binArea.width;
      }
      if ( (*it).val[3]-binMask.rows/2 + binMask.rows >= tmp.rows ){
        binArea.height = binMask.rows - ( (*it).val[3]-binMask.rows/2 + binMask.rows - tmp.rows );
        imgArea.height = binArea.height;
      }

      std:: cout << "center\n";
      // copy this object instance in new image of correct size
      tmp.setTo(0);

      if (binArea.x < 0 || binArea.y < 0
           || binArea.x + binArea.width > binMask.cols 
           || binArea.y + binArea.height > binMask.rows) {

        std:: cout << binMask.cols << ", "  << binMask.rows << ", " << binArea << "\n";
        std:: cout << "Error1: im continuing\n";
        continue;
      }
      // the problem
      Mat binRoi = Mat(binMask, binArea);

      if (imgArea.x < 0 || imgArea.y < 0
           || imgArea.x + imgArea.width > testImage.cols 
           || imgArea.y + imgArea.height > testImage.rows) {

        std:: cout << testImage.cols << ", "  << testImage.rows << ", " << imgArea << "\n";
        std:: cout << "Error2: im continuing\n";
        continue;
      }
      Mat imgRoi = Mat(tmp, imgArea);
      binRoi.copyTo(imgRoi);

      // delete found object from original image in order to reset pixel values with red (which are white up until now)
      binMask = 1 - binMask;
      imgRoi = Mat(red, imgArea);
      multiply(imgRoi, binRoi, imgRoi);//, 1,CV_32FC1);//TODO: bad argument. types do not matc
      imgRoi = Mat(green, imgArea);
      multiply(imgRoi, binRoi, imgRoi);//, 1,CV_32FC1);//TODO: bad argument. types do not matc
      imgRoi = Mat(blue, imgArea);
      multiply(imgRoi, binRoi, imgRoi);//i, 1,CV_32FC1);//TODO: bad argument. types do not matc

      // change red channel
      red = red + tmp*255;
      std:: cout << "end\n";
    }
    // generate color image
    vector<Mat> color;
    color.push_back(blue);
    color.push_back(green);
    color.push_back(red);
    Mat display;
    merge(color, display);
    // display color image
    showImage(display, "result", 0);
    // save color image
    imwrite("detectionResult.png", display);
}
















//
//
// Getter and setter

vector<double> CharPreparationSpace::getScales() {
  return this->scales;
}
vector<double> CharPreparationSpace::getRotations() {
  return this->rotations;
}
vector<vector<Mat> > CharPreparationSpace::getHoughPreparation() {
  return this->houghPreparation;
}

