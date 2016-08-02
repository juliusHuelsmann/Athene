#include "preprocessing.h"


/*


Step 1)     threshold
Step 2)     get the first 'active' point next to the click location
            get the corresponding shape (center, 









*/



Mat source;


// CV_8UC1 == binResult.type()
// CV_8UC1 = orig.type()
void percecution(Mat& binResult, int x, int y, Mat orig, int srcx, int srcy) {


  // 8UC1
  int threshold = 100;
  int valuuu = (int) (orig.at<uchar>(x, y));
  int sub = (abs(srcx - x)) / 2;
  if (valuuu - sub >= threshold) {
    binResult.at<uchar>(x, y) = 255;
  } else {
    binResult.at<uchar>(x, y) = 0;
    return; 
  }
  
  for(int i = -1; i <= 1; i++) {
    for(int j = -1; j <= 1; j++) {
    
      int newx = x + i;
      int newy = y + j;
    
      if ((!(i == 0 && j == 0)) 
          && newx >= 0 
          && newy >= 0 
          && newx < binResult.rows 
          && newy < binResult.cols ) {
        int du = (int) binResult.at<uchar>(newx,  newy);
        if (du == 2) {
          percecution(binResult, newx, newy, orig, srcx, srcy);
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
  //blur( src_gray, src_gray, Size(8,1) ); // 5
  blur( src_gray, src_gray, Size(5,1) ); // 5 15
  
  Mat binResult = 2 * Mat::ones(src_gray.rows, src_gray.cols, CV_8UC1);
  percecution(binResult, x, y, src_gray, x, y);
  
  
  int k = 5;
	dilate(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k, k)));
	erode(binResult, binResult, getStructuringElement(MORPH_RECT, Size(k, k)));
  
  
  // for displaying
  bool display = true;
  if (display) {
    
    Mat result = 2 * Mat::ones(binResult.rows, binResult.cols, CV_8UC3);
    
    //bge
    for(int i = 0; i < result.rows; i++) {
      for(int j = 0; j < result.cols; j++) {
        int b = source.at<Vec3b>(i, j)[0];
        int g = source.at<Vec3b>(i, j)[1];
        int r = (int)binResult.at<uchar>(i, j);
          int redd = source.at<Vec3b>(i, j)[2];
        if (r == 255)  {
        
          
          r = min(redd + 50, 255);
          g = max(g - 50, 0);
          b = max(b - 50, 0);
        } else {
          r = redd;
        }
        result.at<Vec3b>(i, j) = Vec3b(b, g, r);
      }
    }
    
    int d = 10;
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
  
  std:: string win0 = "orig";
  namedWindow( win0 );
  setMouseCallback(win0, segmentHandler, NULL);
  imshow( win0, source);
  waitKey(0);
  



  
  std:: cout << "Ende \n";
  return 0;
}

