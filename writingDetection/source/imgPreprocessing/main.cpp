#include "preprocessing.h"


/*


Step 1)     threshold
Step 2)     get the first 'active' point next to the click location
            get the corresponding shape (center, 









*/


int thresh1 = 70;
Mat source;




// CV_8UC1 == binResult.type()
// CV_8UC1 = orig.type()
void percecution(Mat& binResult, int row, int col, Mat orig, int startRow, int startCol) {


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
void startPercecution(Mat& binResult, int row, int col, Mat orig, int& shiftRow, double& stretch) {


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
  Mat pathOrig = orig(Range(row - minus, row + plus), Range::all());
  // resize
  double ratio = 
                  16.0
                 //pathOrig.rows 
                 / pathOrig.rows;
  stretch = ratio;
  shiftRow = row - minus;
  Size dsize = Size( (int)(pathOrig.cols * ratio), (int) (pathOrig.rows * ratio)); // is swapped...
  resize(pathOrig, pathOrig, dsize);



  row = (int) round(1.0 * minus * ratio);
  
  for (int q = -5; q <= 5; q++) {
    pathOrig.at<Vec3b>(row+ q, col + q) = Vec3b(0, 0, 255);
  }
  std:: string win0 = "hool";
  namedWindow( win0 );
  imshow( win0, pathOrig);
  waitKey(0);


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


void extractSegment(Mat& src_gray, int y, int x)
{
  int edgeThresh = 1;
  int lowThreshold = 100;
  int const max_lowThreshold = 100;
  int ratio = 1;
  int kernel_size = 3;
  std::string win1 = "orig";


  /// Canny detector
  Canny( src_gray, src_gray, lowThreshold, lowThreshold*ratio, kernel_size );
  blur( src_gray, src_gray, Size(8,1) ); // 5 15
  
  Mat binResult;
  int shiftRow;
  double stretchFactor;
  startPercecution(binResult, x, y, src_gray, shiftRow, stretchFactor);
  
  // dislike long structures. therefore: erode with more col (2nd entry)
  int morec = 3;
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(1, morec)));
  int k = 5;
  int addidil = 5;
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k + addidil, k + addidil)));
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k, k)));
  
  // for displaying
  bool display = true;
  if (display) {
    
    Mat result = source.clone();
    
    //bge
    for(int i = 0; i < result.rows; i++) {
      for(int j = 0; j < result.cols; j++) {
        int iimg = (int) ((i - shiftRow) * stretchFactor);
        int jimg = (int) ((j) * stretchFactor);
        int b = source.at<Vec3b>(i, j)[0];
        int g = source.at<Vec3b>(i, j)[1];
        int r = source.at<Vec3b>(i, j)[2];
        
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
    namedWindow( win1, CV_WINDOW_AUTOSIZE );
    imshow( win1, result);
    waitKey(0);
  }
  
  
  
 }

void segmentHandler(int event, int x, int y, int flags, void* userdata)
{


  Mat src_gray;
  cvtColor(source, src_gray, CV_BGR2GRAY );
  
     if  ( event == EVENT_LBUTTONDOWN )
     {
          extractSegment(src_gray, x, y);
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
     }
     else if ( event == EVENT_MOUSEMOVE )
     {

     }
}





int main(int argc, char** argv) {

  source = imread("screenshot.png");
  source = imread("test2.png");
  
  std:: string win0 = "orig";
  namedWindow( win0 );
  setMouseCallback(win0, segmentHandler, NULL);
  imshow( win0, source);
  waitKey(0);
  



  
  std:: cout << "Ende \n";
  return 0;
}

