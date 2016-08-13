
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
  //delete(source); //TODO: Test if that is okay by deleting the inst of Preproc.
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
  //printHistogram();
  
  
  // dislike long structures. therefore: erode with more col (2nd entry)
  
  //TODO: Das hier wieder einkommentieren und ueberpruefen ob bei min max richtig gehandelt wird.
  int morec = 1;
	erode(this->smallSegmentBin, this->smallSegmentBin, getStructuringElement(MORPH_RECT, Size(1, morec)));
	dilate(this->smallSegmentBin, this->smallSegmentBin, getStructuringElement(MORPH_RECT, Size(1, morec)));
  int k = 8;
  int addidil = 8;
	dilate(this->smallSegmentBin, this->smallSegmentBin, getStructuringElement(MORPH_RECT, Size(k + addidil, k + addidil)));
	erode(this->smallSegmentBin, this->smallSegmentBin, getStructuringElement(MORPH_RECT, Size(k, k)));
  minR -= addidil;
  maxR += addidil;
  minC -= addidil;
  maxC += addidil;
  
  
  // Taks:        Get background - Foureground relationship.
  //
  // For all approaches it is firstly necessary only to regard the sgement
  // that has been activated by the preprocessing.
  //
  //
  // Taks for now: find global maximum and global minimum 
  int biggest = 0;
  Vec3b biggestIndex;
  int smallest = 0;
  Vec3b smallestIndex;
  for (int bindx = 0; bindx < HIST_SIZE; bindx++) {
    for (int gind = 0; gind < HIST_SIZE; gind++) {
      for (int rind = 0; rind < HIST_SIZE; rind++) {
        if (this->localHistogram[bindx][gind][rind] > biggest) {
          biggestIndex = Vec3b(bindx, gind, rind);
          biggest = this->localHistogram[bindx][gind][rind];
        }  
        if (this->localHistogram[bindx][gind][rind] < smallest) {
          smallestIndex = Vec3b(bindx, gind, rind);
          smallest = this->localHistogram[bindx][gind][rind];
        }         
      }
    }
  }
    
  Vec3b scndBbIndex;
    
  // find a value in between as threshold
  // val >= clrThresh -> background
  float weight = 1.0 / 2.0;
  Vec3f clrThresh = Vec3f(
        (biggestIndex[0] * weight + smallestIndex[0] * (1 - weight)) * HIST_STEP,
        (biggestIndex[1] * weight + smallestIndex[1] * (1 - weight)) * HIST_STEP,
        (biggestIndex[2] * weight + smallestIndex[2] * (1 - weight)) * HIST_STEP);
  
 
 
  //
  //
  // Compute the current propability histogram.
  Mat meanFG = (Mat_<float>(3, 1) << biggestIndex[0], biggestIndex[1], biggestIndex[2]);
  Mat meanBG = (Mat_<float>(3, 1) << smallestIndex[0], smallestIndex[1], smallestIndex[2]);
  Mat invCovFG = Mat::zeros(3, 3, CV_32FC1);
  Mat invCovBG = Mat::eye(3, 3, CV_32FC1);
  float eigenvalues = 0.822;
  invCovFG.at<float>(0, 0) = 1.0 / eigenvalues; 
  invCovFG.at<float>(1, 1) = 1.0 / eigenvalues; 
  invCovFG.at<float>(2, 2) = 1.0 / eigenvalues;
  float sqrtdetFG = sqrt(eigenvalues * eigenvalues * eigenvalues);
  float sqrtdetBG = 1.0;
  
  for (int bindx = 0; bindx < HIST_SIZE; bindx++) {
    for (int gind = 0; gind < HIST_SIZE; gind++) {
      for (int rind = 0; rind < HIST_SIZE; rind++) {
      
      
        Mat x = (Mat_<float>(3, 1) << bindx, gind, rind);
        Mat xt;
        transpose(x-meanFG, xt);
        Mat d = x - meanFG;
        Mat expM = (-1.0/2.0 *  xt * invCovFG * d);
        float vFG = 1.0/sqrtdetFG * exp(expM.at<float>(0,0));
        
        
        Mat x2t;
        transpose(x-meanBG, x2t);
        Mat d2 = x - meanBG;
        Mat expM2 = (-1.0/2.0 *  x2t * invCovBG * d2);
        float vBG = 1.0/sqrtdetBG * exp(expM2.at<float>(0,0));
        probHistogram[bindx][gind][rind] = vFG / (vFG + vBG);
        std:: cout << ((int) round(probHistogram[bindx][gind][rind] * 10))/10.0 << "\t";
      }
        std:: cout << "\n";
    }
        std:: cout << "\n";
        std:: cout << "\n";
  }
  
  
  
  
        
  // debug
  
  /*
  std:: cout << "histogram" <<"\n";
  int sum = 0;
  for (int bindx = 0; bindx < HIST_SIZE; bindx++) {
    std:: cout << "\n" << bindx <<"\n";
    for (int gind = 0; gind < HIST_SIZE; gind++) {
      
      for (int rind = 0; rind < HIST_SIZE; rind++) {
        if (localHistogram[bindx][gind][rind]== 0) {
        std:: cout << " "  << "\t";   
        } else {
        
          std:: cout << localHistogram[bindx][gind][rind] << "\t";   
        }  
        sum +=  localHistogram[bindx][gind][rind] ;
      }
      std:: cout <<"\n";
    }
  }
  std:: cout << "sum=" << sum << "\n";
  exit(1);
  */
  Vec3b one = Vec3b(1, 1, 1);
  Vec3b border = Vec3b(0, 0, 255);
  Mat fgbg = Mat(2, 7, CV_8UC3);
  fgbg.at<Vec3b>(0, 0) = biggestIndex * HIST_STEP;
  fgbg.at<Vec3b>(1, 0) = biggestIndex * HIST_STEP;
  fgbg.at<Vec3b>(0, 1) = (biggestIndex + one) * HIST_STEP;
  fgbg.at<Vec3b>(1, 1) = (biggestIndex + one) * HIST_STEP;
  
  fgbg.at<Vec3b>(0, 2) = border;
  fgbg.at<Vec3b>(1, 2) = border;
  
  fgbg.at<Vec3b>(0, 3) = clrThresh;
  fgbg.at<Vec3b>(1, 3) = clrThresh;
  
  
  fgbg.at<Vec3b>(0, fgbg.cols - 3) = border;
  fgbg.at<Vec3b>(1, fgbg.cols - 3) = border;
  
  
  fgbg.at<Vec3b>(0, fgbg.cols - 2) = smallestIndex * HIST_STEP;
  fgbg.at<Vec3b>(1, fgbg.cols - 2) = smallestIndex * HIST_STEP;
  fgbg.at<Vec3b>(0, fgbg.cols - 1) = (smallestIndex + one) * HIST_STEP;
  fgbg.at<Vec3b>(1, fgbg.cols - 1) = (smallestIndex + one) * HIST_STEP;
 // showImage(fgbg, "fg links, bg rechts (ranges); Center:thresh", 0);
   
   
  
    
  Mat source2 = source.clone();
	//threshold(source, source2, 220, 255, 0);;
  cvtColor(source2, source2, CV_BGR2GRAY);
	threshold(source2, source2, 100, 255, 0);
//	showImage(source2, "orig", 0);
  
  
  
  
  // for displaying
  bool display = true;
  if (display) {
  
    
    Mat result = Mat(source.rows, source.cols, CV_8UC3);
    
    //bge
    for(int i = 0; i < result.rows; i++) {
      for(int j = 0; j < result.cols; j++) {
        int iimg = 0;
        int jimg = 0;
        cvtEC2SC(i, j, iimg, jimg);
        int b = source2.at<uchar>(i, j);
        int g = source2.at<uchar>(i, j);
        int r = source2.at<uchar>(i, j);
        
        if (iimg >= 0 && jimg >= 0 && iimg < this->smallSegmentBin.rows && jimg < this->smallSegmentBin.cols) {
          
          int rV = (int) this->smallSegmentBin.at<uchar>(iimg , jimg);
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
    std::string win1 = "oerig";
    namedWindow( win1, CV_WINDOW_AUTOSIZE );
    imshow( win1, result);
    waitKey(0);
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  vector<Mat> shapes;
 // extractLetters(this->smallSegmentBin, source, shapes);
  //showImage(this->source, "source detected", 0);
 
 
  //TODO: bis hier hin korrigiert.
 // showImage(this->source, "source", 0);
 // showImage(this->cran, "cran", 0);
 // showImage(this->smallSegmentBin, "smallSegmentBin", 0);
 // showImage(this->smallSegmentDer, "smallSegmentDer", 0);
  
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
      if (newcol >= 0 && newcol < this->cran.cols) {
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
  this->smallSegmentBin = 2 * Mat::ones(this->smallSegmentDer.rows, this->smallSegmentDer.cols, CV_8UC1);
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
              
              
              /*
               * Task 4:     Fill the histogram with background values
               */
              for(int sr = 0; sr < smallSegmentBin.rows; sr++) {
                for(int sc = 0; sc < smallSegmentBin.cols; sc++) {
                  // if background
                  int eR, eC;
                  cvtSC2EC(sr, sc, eR, eC);
                  if (smallSegmentBin.at<uchar>(sr, sc) == 0) {
                    int vB = (int) floor(1.0 * source.at<Vec3b>(eR, eC)[0] / HIST_STEP);
                    int vG = (int) floor(1.0 * source.at<Vec3b>(eR, eC)[1] / HIST_STEP);
                    int vR = (int) floor(1.0 * source.at<Vec3b>(eR, eC)[2] / HIST_STEP);
                  localHistogram
                        [min(vB, HIST_STEP)]
                        [min(vG, HIST_STEP)]
                        [min(vR, HIST_STEP)] --;
                  }
                }
              }
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
void Preprocessing::percecution(Mat& binResult, int row, int col, Mat orig,
    int startRow, int startCol) {


  //
  //
  // Check if the gray value is large enough. Write the result into the bin mask.
  // Als update the surroundin rectangle and the local histogram.
  int valuuu = (int) (orig.at<uchar>(row, col));
  int sub = pow(startRow - row, 2) / 6;
  if (valuuu - sub >= derivThreshold) {
    binResult.at<uchar>(row, col) = 255;
    
    // update the surrounding rectangle
    int rowEntire, colEntire;
    cvtSC2EC(row, col, rowEntire, colEntire);
    
    minR = min(minR, rowEntire);
    maxR = max(maxR, rowEntire);
    minC = min(minC, colEntire);
    maxC = max(maxC, colEntire);
    localHistogram
        [(int) floor(1.0 * source.at<Vec3b>(row, col)[0] / HIST_STEP)]
        [(int) floor(1.0 * source.at<Vec3b>(row, col)[1] / HIST_STEP)]
        [(int) floor(1.0 * source.at<Vec3b>(row, col)[2] / HIST_STEP)] ++;
  } else {
    binResult.at<uchar>(row, col) = 0;
    return; 
  }  
  
  
  //
  //
  // Recursive call.
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



























void Preprocessing::extractLetters(Mat binResult, Mat xsource, vector<Mat>& shapes) {

  shapes = vector<Mat>();
  vector<Mat> locs = vector<Mat>();   
  
  Mat tempImageRange = xsource(Range(this->smallSegmentRowDisplace, this->smallSegmentRowDisplace + 50), Range::all());
  showImage(tempImageRange, "segment", 0);
  
  for(int i = this->smallSegmentRowDisplace; i < xsource.rows; i++) {
    for(int j = 0; j < xsource.cols; j++) {

      int iimg; //(int) ((i - shiftRow) * stretchFactor);
      int jimg; //(int) (j * stretchFactor);
      cvtEC2SC(i, j, iimg, jimg);
      int b = xsource.at<Vec3b>(i, j)[0];
      int g = xsource.at<Vec3b>(i, j)[1];
      int r = xsource.at<Vec3b>(i, j)[2];
      
      
      int vvB = (int) floor(1.0 * b / HIST_STEP);
      int vvG = (int) floor(1.0 * g / HIST_STEP);
      int vvR = (int) floor(1.0 * r / HIST_STEP);
        
      if (iimg >= 0 && jimg >= 0 && iimg < binResult.rows && jimg < binResult.cols) {
          
        int rV = (int) binResult.at<uchar>(iimg , jimg);
        if (probHistogram[vvB][vvG][vvR] >= 0.5 && rV == 255)  {
          
          // this is selected area
          Mat result = 155 *  Mat::ones(3 * (maxR - minR), 3*(maxC - minC), CV_8UC1);
          Mat locate = Mat(4, 1, CV_32FC1);
          int minCShape = result.cols; //maxC - minC;
          int maxCShape = 0;
          int minRShape = result.rows; //(maxR - minR);
          int maxRShape = 0;


          percecuteTwo(result, xsource, i, j, minCShape, 
              maxCShape, minRShape, maxRShape);

          locate.at<float>(0, 0) = minRShape;
          locate.at<float>(1, 0) = maxRShape;
          locate.at<float>(2, 0) = minCShape;
          locate.at<float>(3, 0) = maxCShape;
          //std:: cout << "locate vals\n";
          //std:: cout << "(" << result.rows << "," << result.cols << "\n";
          //std:: cout << "(" << minRShape << "-" << maxRShape<< "," << minCShape  << "-" << maxCShape << "\n";
          
          //showImage(result, "resultLetter", 0);
          //TODO: Check if the order is correct.
          result = result(Range(minRShape, maxRShape + 1), Range(minCShape, maxCShape + 1));
          //std:: cout << "result rerange\t";
          //std:: cout << "(" << result.rows << "," << result.cols << "\n";
          //showImage(result, "resultLetter", 0);
          shapes.push_back(result);
          locs.push_back(result);
          
          
        }
      }
    }
  }
  
  for (int s = 0; s < shapes.size(); s++) {
    showBin(shapes[s], "the extracted shape :)", 0);
  }
     //     showImage(source, "changedsource", 0);
  
  //TODO: Split letters or merge them according to their size.
}





//
// @param result      Pixel in third coorindate system
//                    Entire Image coord (r,c) -> cvtEC2SC(r,c) + (h, w) 
//                    with  h := maxR - minR
//                          w := maxC - minC
//
// @param xsource     clone of source; entire image coordinates.
//
// @param thresh      The threshold that is used for percecution together with
// @param geq1,       true if foreground is to be >= thresh coord.
//         geq2, geq3
//
// @param r, c        The coordinates in entire image
//
// @param m***Shape   The describing rectangle of the percecuteTwo.
//
void Preprocessing::percecuteTwo(Mat& result, Mat xsource, 
    int r, int c, int& minCShape, int& maxCShape, 
    int& minRShape, int& maxRShape) {

  int curr = 0;
  int curc = 0;
  cvtEC2Letter(r, c, curr, curc);

  if (curr < 0 || curc < 0 || curr >= result.rows || curc >= result.cols) {
    return;
  }
  
  minRShape = min(curr, minRShape);
  maxRShape = max(curr, maxRShape);
  minCShape = min(curc, minCShape);
  maxCShape = max(curc, maxCShape);
  
  result.at<uchar>(curr, curc) = 255;
  
  //
  // Recursive call is triggered in case the pixel is cathegorized as 
  // foreground.
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) {
        continue;
      } else {
        if (i + r >= 0 && j + c >= 0 
            && i + r < xsource.rows && j + c < xsource.cols) {
        
          int newrL, newcL;
          cvtEC2Letter(i + r, j + c, newrL, newcL);
        
          // check if in range
          int vvB = (int) floor(1.0 *  xsource.at<Vec3b>(i + r, j + c)[0] / HIST_STEP);
          int vvG = (int) floor(1.0 *  xsource.at<Vec3b>(i + r, j + c)[1] / HIST_STEP);
          int vvR = (int) floor(1.0 *  xsource.at<Vec3b>(i + r, j + c)[2] / HIST_STEP);
         
          source.at<Vec3b>(i + r, j + c) = Vec3b(255, 0, 0);
          
          if (newrL >= 0 && newcL >= 0 && newrL < result.rows && newcL < result.cols && result.at<uchar>(newrL, newcL) == 155) {
            if (probHistogram[vvB][vvG][vvR] >= 0.5) {
              percecuteTwo(result, xsource, r + i, c + j, 
                  minCShape, maxCShape, minRShape, maxRShape);
            } else {
              
              result.at<uchar>(newrL, newcL) = 0;
            }
          }
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


















void Preprocessing::printHistogram() {

  int factor = 100;
  int maxval = 20;
  int spaceadditional = 200;
  int gap = 5;
  
  Mat glob = Mat::zeros(HIST_SIZE * factor * 2 + spaceadditional, 2 * gap + HIST_SIZE * factor * 3 + spaceadditional, CV_8UC3);
  
  for (int i = 0; i < 3; i++) {
    //put first text
    
    for (int as = 0; as < HIST_SIZE; as++) {
    
      putText(glob, toString(as), Point(0, spaceadditional / 2 + factor / 2 + as * factor), 0, 2, Vec3b(255, 255, 255));
      putText(glob, toString(as), Point(i * gap + HIST_SIZE * factor * i +  as * factor, spaceadditional/2), 0, 2, Vec3b(255, 255, 255));
    }
  
    Vec3b txtClr = Vec3b(200, 200, 200);
    for (int r = 0; r < HIST_SIZE; r++) {
      for (int c = 0; c < HIST_SIZE; c++) {
        int anz = 0;
        for (int s = 0; s < HIST_SIZE; s++) {
          anz += localHistogram [r * (i == 0) + r * (i == 1) + s * (i == 2)]
                                [c * (i == 0) + s * (i == 1) + r * (i == 2)]
                                [s * (i == 0) + c * (i == 1) + c * (i == 2)];
        }
        
        int cr, cg, cb;
        cg = min(200, abs(anz) * 200 / maxval);
        if (anz < 0) {
          cr = cg;
          cb = cg + 55;
        } else if (anz > 0) {
          cb = cg;
          cr = cg + 55;
        } else {
          cg = 0;
          cb = 0;
          cr = 0;
        }
        txtClr = Vec3b(255 - cb, 255 - cg, 255 - cr);
        Vec3b vec = Vec3b(cb, cg, cr);
        for (int k = 0; k < factor; k++) {
          for (int l = 0; l < factor; l++) {
            
            int row = k + r * factor;
            int col = l + c * factor;
            glob.at<Vec3b>(spaceadditional / 2 + k + r * factor, spaceadditional / 2  + i * gap + HIST_SIZE * factor * i +  l + c * factor) =  vec;       
          }
        }
        std:: string pos = toString(abs(anz));
        putText(glob, pos, Point(spaceadditional / 2  + i * gap + HIST_SIZE * factor * i +  c * factor, spaceadditional / 2 + factor / 2 + r * factor), 0, 2, txtClr);
      }
    }
  
 // if (i == 1) 
   // exit(1);
  
    // bg, br, gr
    std:: string one = "bg";
    std:: string two = "br";
    std:: string three = "gr";
    
    std:: string t;
    switch(i) {
    case 0:
      t = one;
      break;
    case 1:
      t = two;
      break;
    default:
      t = three;
      break;
    }
    putText(glob, t, Point(spaceadditional / 2 + HIST_SIZE * factor * (i + 1.0/2) + i * gap, spaceadditional / 2 + HIST_SIZE * factor + 30), 1, 10, Vec3b(255, 255, 255));
  }
    showImage(glob, "segment histogram", 0);


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
  int rd = this->smallSegmentRowDisplace;
  sR  = (eR - rd) * str;
  sC = eC * str;
}
    
/**
  * Convert coordinates of small-segment-image to coordinates of entire img.
  * @see smallSegmentRowDisplace
  */
void Preprocessing::cvtSC2EC(int sR, int sC, int& eR, int& eC) {

  double str = this->smallSegmentStretch;
  int rd = this->smallSegmentRowDisplace;
  eR  = sR / str + rd;
  eC = sC / str;
}




/**
 * (r,c) -> (cvtEc2SC(r,c)) + (h_r, w_c) 
 */
 /*
void Preprocessing::cvtEC2Letter(int eR, int eC, int& lR, int& lC) {
  
  int h_r = this->maxR - this->minR;
  int h_c = this->maxC - this->minC;

  cvtEC2SC(eR, eC, lR, lC);
  std:: cout << "\thalfCoor:  (" << lR << ",  \t"  << lC << ")\n";
  lR = lR + h_r;
  double str = this->smallSegmentStretch;
  lC = lC + h_c - this->minC * str;
}


*/
/**
 * (r,c) -> (cvtEc2SC(r,c)) + (h_r, w_c) 
 */
void Preprocessing::cvtEC2Letter(int eR, int eC, int& lR, int& lC) {
  
  
  int rd = this->smallSegmentRowDisplace;
  lR  = (eR - rd);
  lC = eC ;
  
  int h_r = this->maxR - this->minR;
  int h_c = this->maxC - this->minC;
//TODO: hier kommentare falsch
  lR = lR + h_r;
  lC = lC + h_c - this->minC;
}



