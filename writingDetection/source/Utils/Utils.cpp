#include "Utils.h"

float getNormPowMax(Mat input) {

  float maxVal = 0.0;

  for (int r = 0; r < input.rows; r++) {
    for (int c = 0; c < input.cols; c++) {
      float normedVal = pow(input.at<Vec2f>(r, c)[0], 2) + pow(input.at<Vec2f>(r, c)[1], 2);
      maxVal = max(maxVal, normedVal);
    }
  }
  
  
  return maxVal;
}














/*



Simple function for writing matrix into file. Is good for testing randomness 
of matrix output ;)


*/
void writeToFile(char* name, Mat text) {

  std::fstream fs;
  fs.open (name, std::fstream::in | std::fstream::out | std::fstream::app);
  fs << text;
  fs.close();
}








//
//
//
Mat calcDirectionalGrad(Mat& image, double sigma){

  // compute kernel size
  int ksize = max(sigma*3,3.);
  if (ksize % 2 == 0)  ksize++;
  double mu = ksize/2.0;

  // generate kernels for x- and y-direction
  double val, sum=0;
  Mat kernel(ksize, ksize, CV_32FC1);
  //Mat kernel_y(ksize, ksize, CV_32FC1);
  for(int i=0; i<ksize; i++){
      for(int j=0; j<ksize; j++){
		val  = pow((i+0.5-mu)/sigma,2);
		val += pow((j+0.5-mu)/sigma,2);
		val = exp(-0.5*val);
		sum += val;
		kernel.at<float>(i, j) = -(j+0.5-mu)*val;
     }
  }
  kernel /= sum;
  // use those kernels to compute gradient in x- and y-direction independently
  vector<Mat> grad(2);
  filter2D(image, grad[0], -1, kernel);
  filter2D(image, grad[1], -1, kernel.t());
  // combine both real-valued gradient images to a single complex-valued image
  Mat output;
  merge(grad, output);
  
  return output; 
}















//
//
// Rotate and scale matrix.
Mat rotateAndScale(Mat& image, double angle, double scale){
    
    // create transformation matrices
    // translation to origin
    Mat T = Mat::eye(3, 3, CV_32FC1);    
    T.at<float>(0, 2) = -image.cols/2.0;
    T.at<float>(1, 2) = -image.rows/2.0;
    // rotation
    Mat R = Mat::eye(3, 3, CV_32FC1);
    R.at<float>(0, 0) =  cos(angle);
    R.at<float>(0, 1) = -sin(angle);
    R.at<float>(1, 0) =  sin(angle);
    R.at<float>(1, 1) =  cos(angle);
    // scale
    Mat S = Mat::eye(3, 3, CV_32FC1);    
    S.at<float>(0, 0) = scale;
    S.at<float>(1, 1) = scale;
    // combine
    Mat H = R*S*T;

    // compute corners of warped image
    Mat corners(1, 4, CV_32FC2);
    corners.at<Vec2f>(0, 0) = Vec2f(0,0);
    corners.at<Vec2f>(0, 1) = Vec2f(0,image.rows);
    corners.at<Vec2f>(0, 2) = Vec2f(image.cols,0);
    corners.at<Vec2f>(0, 3) = Vec2f(image.cols,image.rows);
    perspectiveTransform(corners, corners, H);
    
    // compute size of resulting image and allocate memory
    float x_start = min( min( corners.at<Vec2f>(0, 0)[0], corners.at<Vec2f>(0, 1)[0]), min( corners.at<Vec2f>(0, 2)[0], corners.at<Vec2f>(0, 3)[0]) );
    float x_end   = max( max( corners.at<Vec2f>(0, 0)[0], corners.at<Vec2f>(0, 1)[0]), max( corners.at<Vec2f>(0, 2)[0], corners.at<Vec2f>(0, 3)[0]) );
    float y_start = min( min( corners.at<Vec2f>(0, 0)[1], corners.at<Vec2f>(0, 1)[1]), min( corners.at<Vec2f>(0, 2)[1], corners.at<Vec2f>(0, 3)[1]) );
    float y_end   = max( max( corners.at<Vec2f>(0, 0)[1], corners.at<Vec2f>(0, 1)[1]), max( corners.at<Vec2f>(0, 2)[1], corners.at<Vec2f>(0, 3)[1]) );
       
    // create translation matrix in order to copy new object to image center
    T.at<float>(0, 0) = 1;
    T.at<float>(1, 1) = 1;
    T.at<float>(2, 2) = 1;
    T.at<float>(0, 2) = (x_end - x_start + 1)/2.0;
    T.at<float>(1, 2) = (y_end - y_start + 1)/2.0;
    
    // change homography to take necessary translation into account
    H = T * H;
    // warp image and copy it to output image
    Mat output;
    warpPerspective(image, output, H, Size(x_end - x_start + 1, y_end - y_start + 1), CV_INTER_LINEAR);

    return output;
  
}





//
//
// General utility
void showImage(Mat& img, string win, double dur){
  
  // use copy for normalization
  Mat tempDisplay;
  if (img.channels() == 1) {
    normalize(img, tempDisplay, 0, 255, CV_MINMAX);
  } else {
	  tempDisplay = img.clone();
	}
    
  tempDisplay.convertTo(tempDisplay, CV_8UC1);
    
  // create window and display omage
  namedWindow( win.c_str(), 0 );
  imshow( win.c_str(), tempDisplay );
  // wait
  if (dur>=0) {
    cvWaitKey(dur);
  }
  destroyWindow(win.c_str());
    
}


//
//
// General utility
void showBin(Mat& img, string win, double dur){
  
  
  Mat imgClone;
  cvtColor(img, imgClone, CV_GRAY2RGB);
  showImage(imgClone, win, dur);
    
}

//
//
// specialized utility
void circShift(Mat& in, Mat& out, int dx, int dy){
  
  Mat tmp = Mat::zeros(in.rows, in.cols, in.type());
	int x, y, new_x, new_y;
	
	for(y = 0; y < in.rows; y++) {
    // calulate new y-coordinate
	  new_y = y + dy;
	  if (new_y < 0) {
		  new_y = new_y + in.rows;
    }
	  if (new_y >= in.rows) {
		  new_y = new_y - in.rows;
    }
    	      
	  for(x=0; x < in.cols; x++) {
		  new_x = x + dx;
		  if (new_x < 0) {
			  new_x = new_x + in.cols;
			}
		  if (new_x >= in.cols) {
			  new_x = new_x - in.cols;
			}
		  tmp.at<Vec2f>(new_y, new_x) = in.at<Vec2f>(y, x);
    }
  }
	out = tmp;
}

std::string toString(int i) {
  stringstream s;
  s << i;
  return s.str();

}






