
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
}

Preprocessing::~Preprocessing() {
}



void Preprocessing::edges(Mat src, Mat& dst) {

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


  // 8UC1
  int valuuu = (int) (orig.at<uchar>(row, col));
  int sub = pow(startRow - row, 2) / 6;
  if (valuuu - sub >= thresh1) {
    binResult.at<uchar>(row, col) = 255;
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
void Preprocessing::startPercecution(Mat& binResult, int row, int col, Mat orig, int& shiftRow, double& stretch) {


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
  
  edges(pathOrig, pathOrig);



  row = (int) round(1.0 * minus * ratio);
  col = (int) round(1.0 * col * ratio);
  bool doubledebug = false;
  if (doubledebug) {
  for (int q = -5; q <= 5; q++) {
    pathOrig.at<uchar>(row+ q, col * ratio + q) = 255;
  }
  std:: string win0 = "jk ";
  namedWindow( win0 );
  imshow( win0, pathOrig);
  }

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
              return;
            }
          }
        }
      } 
    }
  }
}


void Preprocessing::extractSegment(Mat& src_gray, int y, int x) {

  edges(src_gray, src_gray);
  
  Mat binResult;
  int shiftRow;
  double stretchFactor;
  startPercecution(binResult, x, y, src_gray, shiftRow, stretchFactor);
  
  // dislike long structures. therefore: erode with more col (2nd entry)
  int morec = 1;
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
  int k = 5;
  int addidil = 8;
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k + addidil, k + addidil)));
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k, k)));
  
  
  
  
  
  
  std:: cout << binResult.rows << ",  " << binResult.cols << "\n";
  
  
  
  // for displaying
  bool display = true;
  if (display) {
  
    Mat source2;
	  threshold(source, source2, 130, 255, 0);;
    
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



