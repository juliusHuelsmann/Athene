
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



Preprocessing::Preprocessing(Mat xsource) {
  thresh1 = 70;
  source = xsource;
  
  
    // Taks:        Get background - Foureground relationship.
    //
    // For all approaches it is firstly necessary only to regard the sgement
    // that has been activated by the preprocessing.
    //
    // Approach 1:  Generate Histogram (with low resolution) and take the local
    //                                  two biggest maxima.
    //              The histogram is generated on-the-fly in percecution method.
    //
    //Accodring to 255 = 5 * 51 = 5 * 3 * 17, the possible values are
    // 3, 5, 15, 17, 51, 85. We could suggest that 15, 16 work best
    for (int bind = 0; bind < histSize; bind++) {
      for (int gind = 0; gind < histSize; gind++) {
        for (int rind = 0; rind < histSize; rind++) {
          localHistogram[bind][gind][rind] = 0;
        }
      }
    }
}

Preprocessing::~Preprocessing() {
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
          Mat result = Mat::zeros(maxR - minR, maxC - minC, CV_8UC1);
          Mat locate = Mat(4, 1, CV_32FC1);
          int minCShape = maxC - minC;
          int maxCShape = 0;
          int minRShape = maxR - minR;
          int maxRShape = 0;
          percecuteTwo(result, xsource, thresh, i, j, minCShape, maxCShape, minRShape, maxRShape, geq1, geq2, geq3);
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

void Preprocessing::percecuteTwo(Mat& result, Mat xsource, Vec3b thresh, int r, int c, int& minCShape, int& maxCShape, int& minRShape, int& maxRShape, bool geq1, bool geq2, bool geq3) {
  
  
  int curr = r - minR;
  int curc = c - minC;
  minRShape = min(curr, minRShape);
  maxRShape = max(curr, maxRShape);
  minCShape = min(curc, minCShape);
  maxCShape = max(curc, maxCShape);
  result.at<uchar>(curr, curc) = 255;
  
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


// CV_8UC1 == binResult.type()
// CV_8UC1 = orig.type()
void Preprocessing::percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol) {


  std:: cout << "start perc\n";
  minR = binResult.rows;
  maxR = 0;
  minC = binResult.cols;
  maxC = 0;
  exit(1);

  // 8UC1
  int valuuu = (int) (orig.at<uchar>(row, col));
  int sub = pow(startRow - row, 2) / 6;
  if (valuuu - sub >= thresh1) {
    binResult.at<uchar>(row, col) = 255;
    
    // update the surrounding rectangle
    minR = min(minR, row);
    maxR = max(maxR, row);
    minC = min(minC, col);
    maxC = max(maxC, col);
    localHistogram[ source.at<Vec3b>(row, col)[0]][ source.at<Vec3b>(row, col)[1]][ source.at<Vec3b>(row, col)[2]] ++;
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
bool Preprocessing::startPercecution(Mat& binResult, int row, int col, Mat orig, int& shiftRow, double& stretch) {


  //
  // Extract the approximate size of the font
  int stripelength = 6;
  int stripeStartCol = col;
  
  int sizeThresh = 5;
  int amountPartial = 0;
  int sum = 0;
  int minus = 0;
  int plus = 0;
  do {
  
    sum = 0;
    amountPartial = 0;
    for (int i = -stripelength / 2; i < stripelength / 2; i++) {
    
      int newcol = i + col;
      if (newcol >= 0 && newcol < orig.cols) {
        amountPartial ++;
        sum += (int) orig.at<uchar>(row - minus, newcol);
      }
    }
    
    minus ++;
  } while(sum / stripelength > sizeThresh);
  
  do {
  
    sum = 0;
    amountPartial = 0;
    for (int i = -stripelength / 2; i < stripelength / 2; i++) {
    
      int newcol = i + col;
      if (newcol >= 0 && newcol < orig.cols) {
        amountPartial ++;
        sum += (int) orig.at<uchar>(row + plus, newcol);
      }
    }
    plus ++;
  } while(sum / stripelength > sizeThresh);
  
  
  // stripe row - minus, row + plus
    
  

  //preprocessing. 
  binResult = 2 * Mat::ones(minus + plus, orig.cols, CV_8UC1);
  Mat pathOrig = source(Range(row - minus, row + plus), Range::all()).clone();
  // resize
  double ratio = 
                  14.0
                 //pathOrig.rows 
                 / pathOrig.rows;
  stretch = ratio;
  shiftRow = row - minus;
  Size dsize = Size( (int)(pathOrig.cols * ratio), (int) (pathOrig.rows * ratio)); // is swapped...
  resize(pathOrig, pathOrig, dsize);
  
  derivative(pathOrig, pathOrig);



  row = (int) round(1.0 * minus * ratio);
  col = (int) round(1.0 * col * ratio);

  int maxadd = 20;
  for(int i = 1; i <= maxadd; i++) {
    for(int j = 1; j <= maxadd; j++) {
    
      for (int sigmai = -1; sigmai <= 1; sigmai += 2) {
        for (int sigmaj = -1; sigmaj <= 1; sigmaj += 2) {
        
          int newRow = row + i * sigmai;
          int newCol = col + j * sigmaj;
          
        
          if (newRow >= 0 
              && newCol >= 0 
              && newRow < binResult.rows 
              && newCol < binResult.cols ) {
            int valuuu = (int) (pathOrig.at<uchar>(newRow, newCol));
            if (valuuu >= thresh1) {
          
              // found starting point.
              percecution(binResult, newRow, newCol, pathOrig, newRow, newCol);
              return true;
            }
          }
        }
      } 
    }
  }
  return false;
}


void Preprocessing::extractSegment(int y, int x) {



  //
  // Extract edges
  Mat binResult;
  derivative(source, binResult);
  
  //
  // Start the percecution
  int shiftRow = 0;
  double stretchFactor = 0;
  bool found = startPercecution(binResult, x, y, binResult, shiftRow, stretchFactor);
  
  if (!found) {
    std:: cout << "nothing found\n";
    return;
  }
  
  
  
  // dislike long structures. therefore: erode with more col (2nd entry)
  int morec = 1;
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
  int k = 5;
  int addidil = 8;
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k + addidil, k + addidil)));
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k, k)));
  
  
  
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
    for (int bindx = 0; bindx < histSize; bindx++) {
      for (int gind = 0; gind < histSize; gind++) {
        for (int rind = 0; rind < histSize; rind++) {

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
    localHistogram[biggestIndex[0]][biggestIndex[1]][biggestIndex[2]] *= -1;
    localHistogram[biggestIndex[0]][biggestIndex[1] + 1][biggestIndex[2]] *= -1;
    localHistogram[biggestIndex[0]][biggestIndex[1] - 1][biggestIndex[2]] *= -1;
    localHistogram[biggestIndex[0]][biggestIndex[1]][biggestIndex[2] + 1] *= -1;
    localHistogram[biggestIndex[0]][biggestIndex[1]][biggestIndex[2] - 1] *= -1;
    localHistogram[biggestIndex[0] + 1][biggestIndex[1]][biggestIndex[2]] *= -1;
    localHistogram[biggestIndex[0] - 1][biggestIndex[1]][biggestIndex[2]] *= -1;
    int ScndBb = 0;
    Vec3b scndBbIndex;
    for (int bindx = 0; bindx < histSize; bindx++) {
      for (int gind = 0; gind < histSize; gind++) {
        for (int rind = 0; rind < histSize; rind++) {

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
    extractLetters(binResult, source, shapes, shiftRow, stretchFactor, clrThresh, geq1, geq2, geq3);
    
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
        int iimg = (int) ((i - shiftRow) * stretchFactor);
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



