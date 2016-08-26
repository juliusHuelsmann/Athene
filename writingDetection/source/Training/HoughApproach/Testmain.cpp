

#include "CharPreparationSpace.h"
#include <string>
#include "../../Utils/Utils.h"
#include "../../Utils/Constants.h"

CharPreparationSpace testCreation(int w, int h) {

  Vec2d size = Vec2d(w, h);
  std:: string name = "../../../probe1/chars/small/e.png";
  return CharPreparationSpace(size, name, 'e');  
  
}
void creationEverything(int w, int h, vector<CharPreparationSpace>& characters) {
  
  
  //TODO divide by the max val for the character
  std:: string letterEndings[] ={"a","b","c","d","e","f","g","h","i", "j","k","l"};

  for (int i = 0; i < 12; i++) {
    stringstream s;
    s << "../../../probe1/chars/small/" << letterEndings[i] << ".png";
    Vec2d size = Vec2d(w, h);
    characters.push_back(CharPreparationSpace(size, s.str(), letterEndings[i].at(0)));  
  }
  
  
}


CharPreparationSpace testCreation() {

  return testCreation(200, 200);  
  
}

void testDetection() {


  
  
  // Load img and perform DFT
  
  // a) load image
  // b) calc directional gradient
  // c) perform DFT
  std:: string detectionPath = "../../../testimg/detection.png";
  Mat testImg = imread(detectionPath, 0);
  
  // hier muss noch gradimg gemacht werden
  if (!testImg.data){
		std:: cerr << "Error loading img from \t" << detectionPath << "\n";
		std:: cerr << "The program exits now.";
	  std:: cin.get();
		exit(-1);
  }
  













  //
  // This matrix will contain the indices of the CharPreparationSpaceVector
  // and the size of the found maximum
  Mat result = Mat::zeros(testImg.rows, testImg.cols, CV_32FC3);
  vector <CharPreparationSpace> characters; // die haben jeweils ihren char.
  //result.at<Vec2f>(i, j) = Vec3f([strength of the recognition][index of CharPereparationSpaceVector],[size index])
  
















  // convert 8U to 32F
  testImg.convertTo(testImg, CV_32FC1);
  Mat gradImage = calcDirectionalGrad(testImg, SIGMA);
  dft(gradImage, gradImage, DFT_SCALE + DFT_COMPLEX_OUTPUT);//image

  creationEverything(gradImage.rows, gradImage.cols, characters);

  for (int chara = 0; chara < characters.size(); chara++) {
    
    std:: cout << 100 * chara / characters.size() << "procent  done\n";
    
    CharPreparationSpace A = characters[chara];
    //
    // Compare IMG
    // Compare image
    vector< vector<Mat> >  houghSpace = A.getHoughResult(gradImage);


    // find maxima in hough space
    vector<Scalar> objList;
    A.findHoughMaxima(houghSpace, objList);
    //A.plotHoughDetectionResult(testImg, objList);
    
    int i = 0;
	  for(vector<Scalar>::iterator it = objList.begin(); it != objList.end(); it++, i++){
	  
	  	double scale = A.getScales()[it->val[0]];
		 // double angle = A.getScales()[it->val[1]];
			double x = it->val[2];
			double y = it->val[3];
			double currstrength = it->val[1];
    
      Vec2f objSize =  A.getExpansion(scale);
      for (int r = -objSize[0] / 2; r < objSize[0] / 2; r ++) {
        for (int c = -objSize[1] / 2; c < objSize[1] / 2; c ++) {
          int row = r + y;
          int col = c + x;
          
          if (row < 0 || col < 0 || row >= result.rows || col >= result.cols) {
            continue;
          } else {
            float prevstrength =  result.at<Vec3f>(row, col)[0];
            if (prevstrength < currstrength) {
              result.at<Vec3f>(row, col) = Vec3f(currstrength, i, it->val[0]);
            }
          }
        }
      }
    }
  }
  
  float th = 0.0;
  Mat viz = Mat::zeros(result.rows, result.cols, CV_8UC3);
  for (int r = 0; r < viz.rows; r ++) {
    for (int c = 0; c < viz.cols; c ++) {
      if (result.at<Vec3f>(r, c)[0] > th) {
      
        int re = 255 / 12 * result.at<Vec3f>(r, c)[1];
        //int re = 0;
        int gr = 255;
        int bl = gr;
        viz.at<Vec3b>(r, c) = Vec3b(re, gr, bl);
      }
    }
  }
  showImage(testImg, "viz", 0);
  showImage(viz, "viz", 0);
  
  
  
  // print found objects on screen
  //cout << "Number of objects: " << objList.size() << endl;
  //int i=0;
	//for(vector<Scalar>::iterator it = objList.begin(); it != objList.end(); it++, i++){
	//
		//	double scale = A.getScales()[it->val[0]];
			//double angle = A.getScales()[it->val[1]];
	//		double x = it->val[2];
	//		double y = it->val[3];

   // std:: cout << "Scale: " << scale << "\tAngle: " 
   //            << angle << "\t(x, y): (" << x << "," << y << ")\n";	
  //}



}


int main(int argc, char** argv) {


  //CharPreparationSpace A = testCreation();  
  testDetection();  


  std:: cout << "Terminated successfully\n";
}


