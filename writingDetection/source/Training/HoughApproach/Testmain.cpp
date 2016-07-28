

#include "CharPreparationSpace.h"
#include <string>
#include "../../Utils/Utils.h"
#include "../../Utils/Constants.h"

CharPreparationSpace testCreation(int w, int h) {

  Vec2d size = Vec2d(w, h);
  std:: string name = "../../../probe1/chars/small/e.png";
  return CharPreparationSpace(size, name, 'e');  
  
}


CharPreparationSpace testCreation() {

  return testCreation(200, 200);  
  
}

void testDetection() {

  
  // Load img and perform DFT
  
  // a) load image
  // b) calc directional gradient
  // c) perform DFT
  std:: string detectionPath = "../../../testimg/detection2.png";
  Mat testImg = imread(detectionPath, 0);
  
  // hier muss noch gradimg gemacht werden
  if (!testImg.data){
		std:: cerr << "Error loading img from \t" << detectionPath << "\n";
		std:: cerr << "The program exits now.";
	  std:: cin.get();
		exit(-1);
  }
  
  // convert 8U to 32F
  testImg.convertTo(testImg, CV_32FC1);
  Mat gradImage = calcDirectionalGrad(testImg, SIGMA);
  dft(gradImage, gradImage, DFT_SCALE + DFT_COMPLEX_OUTPUT);//image
  
  CharPreparationSpace A = testCreation(gradImage.rows, gradImage.cols);

  //
  // Compare IMG
  // Compare image
  vector< vector<Mat> >  houghSpace = A.getHoughResult(gradImage);


  // find maxima in hough space
  vector<Scalar> objList;
  A.findHoughMaxima(houghSpace, objList);

  // print found objects on screen
  cout << "Number of objects: " << objList.size() << endl;
  int i=0;
	for(vector<Scalar>::iterator it = objList.begin(); it != objList.end(); it++, i++){
	
			double scale = A.getScales()[it->val[0]];
			double angle = A.getScales()[it->val[1]];
			double x = it->val[2];
			double y = it->val[3];

    std:: cout << "Scale: " << scale << "\tAngle: " 
               << angle << "\t(x, y): (" << x << "," << y << ")\n";	
  }

    // show final detection result
  A.plotHoughDetectionResult(testImg, objList);


}


int main(int argc, char** argv) {


  //CharPreparationSpace A = testCreation();  
  testDetection();  


  std:: cout << "Terminated successfully\n";
}


