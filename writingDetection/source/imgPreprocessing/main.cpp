#include "preprocessing.h"


Mat matemat;
void segmentHandler(int event, int x, int y, int flags, void* userdata) {
  if  ( event == EVENT_LBUTTONDOWN ) {
    Preprocessing p = Preprocessing(matemat);
    p.extractSegment(x, y);
  } else if  ( event == EVENT_RBUTTONDOWN ) {
  
  } else if  ( event == EVENT_MBUTTONDOWN ) { 
  
  } else if ( event == EVENT_MOUSEMOVE ) { 
  
  }
}

int main(int argc, char** argv) {

  matemat = imread("screenshot.png", 0);
  matemat = imread("test2.png", 0);
  if (!matemat.data) {
    std:: cout << "err loading img\n";
    exit(0);
  }
  
  std:: string win0 = "orig";
  namedWindow( win0 );
  setMouseCallback(win0, segmentHandler, NULL);
  imshow( win0, matemat);
  waitKey(0);
  
  std:: cout << "Ende \n";
  return 0;
}



