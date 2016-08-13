#include "preprocessing.h"


Mat gray, clr;
void segmentHandler(int event, int x, int y, int flags, void* userdata) {
  if  ( event == EVENT_LBUTTONDOWN ) {
    Preprocessing p = Preprocessing(clr.clone(), y, x);
  } else if  ( event == EVENT_RBUTTONDOWN ) {
  
  } else if  ( event == EVENT_MBUTTONDOWN ) { 
  
  } else if ( event == EVENT_MOUSEMOVE ) { 
  
  }
}

int main(int argc, char** argv) {

  gray = imread("screenshot.png", 0);
  gray = imread("test2.png", 0);
  clr = imread("test2.png");
  if (!gray.data) {
    std:: cout << "err loading img\n";
    exit(0);
  }
  std:: string win0 = "oerig";
  namedWindow( win0 );
  setMouseCallback(win0, segmentHandler, NULL);
  imshow( win0, gray);
  waitKey(0);
  
  std:: cout << "Ende \n";
  return 0;
}



