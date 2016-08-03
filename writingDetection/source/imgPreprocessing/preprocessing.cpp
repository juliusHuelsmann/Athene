
#include "preprocessing.h"

/*


Step 1)     threshold
Step 2)     get the first 'active' point next to the click location
            get the corresponding shape (center, 







Erweiterungen:
  A) Probabilistischer Ansatz: 
      1) ausgewaehlter startpunkt
      2) waehle innerhalb eines Rasters verschiedene Startpunkte.
         Falls groesere Gebilde dazukommen (bei vielen .. )
         dann uebernehmen
         falls kleinere eher uebernehmen.
         beruecksichtigen: 'groesse' der worte (quotient: cols / rows).




*/




















//
//
// Destructor: Ranges all values. 
Preprocessing::~Preprocessing() {
  delete(source); //TODO: Test if that is okay by deleting the inst of Preproc.
}




















//
//
// The Constructor: Saves the parameters, initializes class variables.
Preprocessing::Preprocessing(Mat xsource, int xrow, int xcol) {

  //
  // Store values
  this->derivThreshold = 70;
  this->source = xsource;
  this->row = xrow;
  this->col = xcol;
  

  //
  // Fill the histogram that whill store all rgb values of 
  // the extracted segment. Is used for being able to 
  //    1) Differentiate between background and foreground
  //    2) Get separated characters.  
  for (int bind = 0; bind < HIST_SIZE; bind++) {
    for (int gind = 0; gind < HIST_SIZE; gind++) {
      for (int rind = 0; rind < HIST_SIZE; rind++) {
        localHistogram[bind][gind][rind] = 0;
      }
    }
  }
  
  extractSegment();
}




















// This is the only public function inside the Preprocessing class.
//
// @param row, col  The location of the click inside the image
//
//
void Preprocessing::extractSegment() {



  //
  // Extract edges for being able to start the percecution.
  derivative(source, this->cran);
  bool found = startPercecution(row, col);
  if (!found) {
    std:: cout << "nothing found\n";
    return;
  }
  
  
  
  // dislike long structures. therefore: erode with more col (2nd entry)
  /*
  //TODO: Das hier wieder einkommentieren und ueberpruefen ob bei min max richtig gehandelt wird.
  int morec = 1;
	erode(this->cran, this->cran, getStructuringElement(MORPH_RECT, Size(1, morec)));
	dilate(this->cran, this->cran, getStructuringElement(MORPH_RECT, Size(1, morec)));
  int k = 5;
  int addidil = 8;
	dilate(this->cran, this->cran, getStructuringElement(MORPH_RECT, Size(k + addidil, k + addidil)));
	erode(this->cran, this->cran, getStructuringElement(MORPH_RECT, Size(k, k)));
  minR -= addidil;
  maxR -= addidil;
  minC -= addidil;
  maxC -= addidil;*/
  
  
  // Taks:        Get background - Foureground relationship.
  //
  // For all approaches it is firstly necessary only to regard the sgement
  // that has been activated by the preprocessing.
  //
  // Approach 1:  Generate Histogram (with low resolution) and take the local
  //                                  two biggest maxima.
  //              The histogram is generated on-the-fly in percecution method.
  //
  // Approach 2)  DOES NOT WORK
  //              a) Do threshold. Once with 200 and with 55.
  //              b) Sum of white, sum of black. Min_i = min(white_i, black_i)
  //                 for both thresholds
  //              c) Take argmin_i min_i
  //                 if i == approach 200 ->  fg black
  //                 else                     fg white
    
    
  //
  // Taks for now: find two biggest local maxima 
  int biggest = 0;
  Vec3b biggestIndex;
  for (int bindx = 0; bindx < HIST_SIZE; bindx++) {
    for (int gind = 0; gind < HIST_SIZE; gind++) {
      for (int rind = 0; rind < HIST_SIZE; rind++) {
        if (localHistogram[bindx][gind][rind] > biggest) {
          biggestIndex = Vec3b(bindx, gind, rind);
          biggest = localHistogram[bindx][gind][rind];
        }          
      }
    }
  }
    
  //TODO: may yield a seg fault
  //
  // find 2nd local maximum
  // rm the values that are not allowed as maxima
  for (int a = -1; a <= 1; a+= 2) {
      int v1u = biggestIndex[0];
      int v2u = biggestIndex[1];
      int v3u = biggestIndex[2];
      
      int v1 = biggestIndex[0] + a;
      int v2 = biggestIndex[1] + a;
      int v3 = biggestIndex[2] + a;
      
      if (v1 >= 0 && v1 < HIST_SIZE) {
        localHistogram[v1][v2u][v3u] *= -1;
      } 
      
      if (v2 >= 0 && v2 < HIST_SIZE) {
        localHistogram[v1u][v2][v3u] *= -1;
      }
      if (v3 >= 0 && v3 < HIST_SIZE) {
        localHistogram[v1u][v2u][v3] *= -1;
      }
    }
    int ScndBb = 0;
    Vec3b scndBbIndex;
    for (int bindx = 0; bindx < HIST_SIZE; bindx++) {
      for (int gind = 0; gind < HIST_SIZE; gind++) {
        for (int rind = 0; rind < HIST_SIZE; rind++) {

          if (localHistogram[bindx][gind][rind] > ScndBb) {
            scndBbIndex = Vec3b(bindx, gind, rind);
            ScndBb = localHistogram[bindx][gind][rind];
          }          
        }
      }
    }
    
    // find a value in between as threshold
    // val >= clrThresh -> background
    Vec3f clrThresh = Vec3f(
          biggestIndex[0] / 2.0 + scndBbIndex[0] / 2.0,
          biggestIndex[1] / 2.0 + scndBbIndex[1] / 2.0,
          biggestIndex[2] / 2.0 + scndBbIndex[2] / 2.0);
    bool geq1 = biggestIndex[0] <= scndBbIndex[0];  // == true in case text grater eq thresh
    bool geq2 = biggestIndex[1] <= scndBbIndex[1]; 
    bool geq3 = biggestIndex[2] <= scndBbIndex[2]; 
    
    
    vector<Mat> shapes;
    extractLetters(binResult, source, shapes, (this->smallSegmentRowDisplace), stretchFactor, clrThresh, geq1, geq2, geq3);
    
    Mat source2;
	  //threshold(source, source2, 220, 255, 0);;
	  threshold(source, source2, 100, 255, 0);;
  
  
  
  
  
  // for displaying
  bool display = true;
  if (display) {
  
    
    Mat result = Mat(source.rows, source.cols, CV_8UC3);
    
    //bge
    for(int i = 0; i < result.rows; i++) {
      for(int j = 0; j < result.cols; j++) {
        int iimg = (int) ((i - (this->smallSegmentRowDisplace)) * stretchFactor);
        int jimg = (int) ((j) * stretchFactor);
        int b = source2.at<uchar>(i, j);
        int g = source2.at<uchar>(i, j);
        int r = source2.at<uchar>(i, j);
        
        if (iimg >= 0 && jimg >= 0 && iimg < binResult.rows && jimg < binResult.cols) {
          
          int rV = (int)binResult.at<uchar>(iimg , jimg);
          if (rV == 255)  {
          
            r = min(r + 200, 255);
            g = max(g - 50, 0);
            b = max(b - 50, 0);
          }
        }
        result.at<Vec3b>(i, j) = Vec3b(b, g, r);
      }
    }
    
       // window
    std::string win1 = "orig";
    namedWindow( win1, CV_WINDOW_AUTOSIZE );
    imshow( win1, result);
    waitKey(0);
  }
}





















//
//
// @param binResult The resulting image (is written by this function)
//
// @param row       The location of the click event
// @param col
//
// @param orig      the original image
//
// @param shiftRow  The location of the patch of the image that is extracted
//                  for being able to display the extracted patch and for
//                  being able to link the extracted patch to the image.
//                  (is written by this function)
// @param stretch   The stretch factor. Dto.
bool Preprocessing::startPercecution(int row, int col) {


  /*
   * Task 1:    Extract the approximate size of the font.
   *            
   *            This is done by first going upwards and afterwards going
   *            downwards (2 loops).
   *
   *            The stripe's location is at (row - minus, row + plus) inside
   *            the image
   */
   
  // The length of the stripe over which the gray values are averaged.
  int stripelength = 6;
  
  // The threshold value. Does not depend on the stripelength because
  // the sum is averaged
  int sizeThresh = 5;
  
  // the sum 
  int sum = 0;
  int minus = 0;
  int plus = 0;
  do {
  
    sum = 0;
    for (int i = -stripelength / 2; i < stripelength / 2; i++) {
      int newcol = i + col;
      if (newcol >= 0 && newcol < orig.cols) {
        sum += (int) (this->cran).at<uchar>(row - minus, newcol);
      }
    }
    
    minus ++;
  } while(sum / stripelength > sizeThresh);
  
  do {
  
    sum = 0;
    for (int i = -stripelength / 2; i < stripelength / 2; i++) {
    
      int newcol = i + col;
      if (newcol >= 0 && newcol < (this->cran).cols) {
        sum += (int) (this->cran).at<uchar>(row + plus, newcol);
      }
    }
    plus ++;
  } while(sum / stripelength > sizeThresh);
  
  
  
  
  
  
  
  /*
   * Task2:   Initialize smallSegmentBin and SmallSegmentDir and preprocess
   */  
  this->smallSegmentDer = source(Range(row - minus, row + plus), Range::all()).clone();
  this->smallSegmentStretch = 14.0 / (this->smallSegmentDer.rows);
  this->smallSegmentRowDisplace = row - minus;
  Size dsize = Size( (int)((this->smallSegmentDer).cols * (this->smallSegmentStretch)), (int) ((this->smallSegmentDer).rows * (this->smallSegmentStretch))); // is swapped...
  resize((this->smallSegmentDer), (this->smallSegmentDer), dsize);
  this->smallSegmentBin = 2 * Mat::ones(smallSegmetDer.rows, smallSegmentDer.cols, CV_8UC1);
  derivative((this->smallSegmentDer), (this->smallSegmentDer));




  /*
   * Task3:   Fill values for smallSegmentBin
   */

  row = (int) round(1.0 * minus * (this->smallSegmentStretch));
  col = (int) round(1.0 * col * (this->smallSegmentStretch));

  int maxadd = 20;
  for(int i = 1; i <= maxadd; i++) {
    for(int j = 1; j <= maxadd; j++) {
    
      for (int sigmai = -1; sigmai <= 1; sigmai += 2) {
        for (int sigmaj = -1; sigmaj <= 1; sigmaj += 2) {
        
          int newRow = row + i * sigmai;
          int newCol = col + j * sigmaj;
          
        
          if (newRow >= 0 
              && newCol >= 0 
              && newRow < this->smallSegmentBin.rows 
              && newCol < this->smallSegmentBin.cols ) {
            int valuuu = (int) ((this->smallSegmentDer).at<uchar>(newRow, newCol));
            if (valuuu >= derivThreshold) {
          
              // found starting point.
              minR = (this->smallSegmentBin).rows;
              maxR = 0;
              minC = (this->smallSegmentBin).cols;
              maxC = 0;

              percecution(this->smallSegmentBin, newRow, newCol, this->smallSegmentDer, newRow, newCol);
              
              minR -= (this->smallSegmentRowDisplace);
              minR /= (this->smallSegmentStretch);
              maxR /= (this->smallSegmentStretch);
              minC /= (this->smallSegmentStretch);
              maxC /= (this->smallSegmentStretch);
              return true;
            }
          }
        }
      } 
    }
  }
  return false;
}




















// CV_8UC1 == binResult.type()
// CV_8UC1 = orig.type()
void Preprocessing::percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol) {


  // 8UC1
  int valuuu = (int) (orig.at<uchar>(row, col));
  int sub = pow(startRow - row, 2) / 6;
  if (valuuu - sub >= derivThreshold) {
    binResult.at<uchar>(row, col) = 255;
    
    // update the surrounding rectangle
    minR = min(minR, row);
    maxR = max(maxR, row);
    minC = min(minC, col);
    maxC = max(maxC, col);
    localHistogram[ (int)round(source.at<Vec3b>(row, col)[0] / HIST_STEP)][(int) round(source.at<Vec3b>(row, col)[1] / HIST_STEP)][ (int) round(source.at<Vec3b>(row, col)[2] / HIST_STEP)] ++;
  } else {
    binResult.at<uchar>(row, col) = 0;
    return; 
  }  
  
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
    
      int newRow = row + i;
      int newCol = col + j;
    
      if ((!(i == 0 && j == 0)) 
          && newRow >= 0 
          && newCol >= 0 
          && newRow < binResult.rows 
          && newCol < binResult.cols ) {
        int du = (int) binResult.at<uchar>(newRow,  newCol);
        if (du == 2) {
          percecution(binResult, newRow, newCol, orig, startRow, startCol);
        }
      } 
    }
  }
}



























void Preprocessing::extractLetters(Mat binResult, Mat xsource, vector<Mat>& shapes, int shiftRow, double stretchFactor, Vec3b thresh, bool geq1, bool geq2, bool geq3) {

  shapes = vector<Mat>();
  vector<Mat> locs = vector<Mat>();
  
  for(int i = 0; i < xsource.rows; i++) {
    for(int j = 0; j < xsource.cols; j++) {
      int iimg = (int) ((i - shiftRow) * stretchFactor);
      int jimg = (int) ((j) * stretchFactor);
      int b = xsource.at<Vec3b>(i, j)[0];
      int g = xsource.at<Vec3b>(i, j)[1];
      int r = xsource.at<Vec3b>(i, j)[2];
        
      if (iimg >= 0 && jimg >= 0 && iimg < binResult.rows && jimg < binResult.cols) {
          
        int rV = (int)binResult.at<uchar>(iimg , jimg);
        if ((geq1 == (b >= thresh[0]))
            && (geq2 == (g >= thresh[1]))
            && (geq3 == (r >= thresh[2]))
            && rV == 255)  {//TODO: <= oder >=?
          
          // this is selected area
          Mat result = Mat::zeros(3 * (maxR - minR), 3*(maxC - minC), CV_8UC1);
          Mat locate = Mat(4, 1, CV_32FC1);
          int minCShape = maxC - minC;
          int maxCShape = 0;
          int minRShape = maxR - minR;
          int maxRShape = 0;
    std:: cout << i << "," << j << "\t" << xsource.rows << " " << xsource.cols << " 5\n";
          percecuteTwo(result, xsource, thresh, i, j, minCShape, maxCShape, minRShape, maxRShape, geq1, geq2, geq3);
    std:: cout << i << "," << j << "\t" << xsource.rows << " " << xsource.cols << " 6\n";
          locate.at<float>(0, 0) = minRShape;
          locate.at<float>(1, 0) = maxRShape;
          locate.at<float>(2, 0) = minCShape;
          locate.at<float>(3, 0) = maxCShape;
          
          //TODO: Check if the order is correct.
          result = result(Range(minCShape, maxCShape), Range(minRShape, maxRShape));
          shapes.push_back(result);
          locs.push_back(result);
          
          
        }
      }
    }
  }
  
  for (int s = 0; s < shapes.size(); s++) {
    showImage(shapes[s], "hker", 0);
  }
  
  //TODO: Split letters or merge them according to their size.
  
  
  
  
  
}

// result is a lot bigger 3 * preferredsize.
void Preprocessing::percecuteTwo(Mat& result, Mat xsource, Vec3b thresh, int r, int c, int& minCShape, int& maxCShape, int& minRShape, int& maxRShape, bool geq1, bool geq2, bool geq3) {

  int aR = maxR - minR;
  int oR = minR - aR;
  int curr = r - oR;
  
  int aC = 0; // for now
  int oC = minC - aC;
  int curc = c - oC;

  std:: cout << minC << " "  << maxC << "\n";
  std:: cout << c<<"\n";
  exit(0);
  std:: cout << "hier0\n";
  //curr = r - (maxR - minR);
  //curc = c - (maxC - minC);
  std:: cout << curr << "," << curc<<" 1asdfasdf\n";
  std:: cout << result.rows << "," << result.cols<<" 2asdfasdf\n";

  
  if (curr < 0 || curc < 0 || curr >= result.rows || curc >= result.cols) {
    return;
  }
  minRShape = min(curr, minRShape);
  maxRShape = max(curr, maxRShape);
  minCShape = min(curc, minCShape);
  maxCShape = max(curc, maxCShape);
  std:: cout << "hier1\n";
  result.at<uchar>(curr, curc) = 255;
  std:: cout << "hier2\n";
  
  for(int i = -1; i <= 1; i++) {
    for(int j = 1; j <= 1; j++) {
      if (i == j == 0) {
        continue;
      } else if (i + r >= 0 && j + c >= 0 && i + r < xsource.rows && j + c < xsource.cols) {
        // check if in range
        
        int b = xsource.at<Vec3b>(i + r, j + c)[0];
        int g = xsource.at<Vec3b>(i + r, j + c)[1];
        int r = xsource.at<Vec3b>(i + r, j + c)[2];
        
        if ((geq1 == (b >= thresh[0]))
            && (geq2 == (g >= thresh[1]))
            && (geq3 == (r >= thresh[2]))) {
            percecuteTwo(result, xsource, thresh, r + i, c + j, minCShape, maxCShape, minRShape, maxRShape, geq1, geq2, geq3);
        }
      }
    }
  }
}


void Preprocessing::derivative(Mat src, Mat& dst) {

  int edgeThresh = 1;
  int lowThreshold = 100;
  int const max_lowThreshold = 100;
  int ratio = 1;
  int kernel_size = 3;


  /// Canny detector
  Canny( src, dst, lowThreshold, lowThreshold*ratio, kernel_size );
  blur( dst, dst, Size(5,1) ); // 5 15
}













































////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                   UTILITIES                                //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
        
/**
 * Convert coordinates of entire image to coordinates of small-segment img.
 * @see smallSegmentRowDisplace
 */
void Preprocessing::cvtEC2SC(int eR, int eC, int& sR, int& sC) {

  double str = this->smallSegmentStretch;
  int rd = this-smallSegmentRowDisplace;
  sR  = (r - rd) * str;
  sC = c * str;
}
    
/**
  * Convert coordinates of small-segment-image to coordinates of entire img.
  * @see smallSegmentRowDisplace
  */
void Preprocessing::cvtSC2EC(int sR, int sC, int& eR, int& eC) {

  double str = this->smallSegmentStretch;
  int rd = this-smallSegmentRowDisplace;
  eR  = sR / str + rd;
  eC = sC / str;
}








