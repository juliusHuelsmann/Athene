//
// This is NOT performed at the client side, is just done for generating
// a lot of training data nearly automatically.
//

#include <sstream>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include "../Utils/Constants.h"
#include "../Utils/Utils.h"
using namespace std;
using namespace cv;





void seedGrowing(int row, int col, Mat&img) {

  if (row >= 0 && col >= 0 && row < img.rows && col < img.cols) {
  

    if (img.at<uchar>(row, col) == 1) {
      img.at<uchar>(row, col) = 0;
      for (int k = -1; k <= 1; k+= 2) {
        seedGrowing(k + row, col, img);
        seedGrowing(row, col + k, img);
      }
    }
  }
}

/*

Calculates the contour line of all objects in an image

img				the input image
objList		vector of contours, each represented by a two-channel matrix
thresh		threshold used to binarize the image
k					number of applications of the erosion operator


*/
vector<Mat> getContourLine(Mat& img) {

  int k = 2;
  int thresh = 200;
  vector<Mat> result = vector<Mat>();
  vector<Mat> objList = vector<Mat>();

	//
	// reserve space for the matrices used for storing information 
	Mat processedImage;
	
	// 
	// preprocessing and find contour
	threshold(img, processedImage, thresh, 255, THRESH_BINARY_INV);
	if (k > 0) {
	
		erode(processedImage, processedImage, getStructuringElement(MORPH_RECT, Size(k, k)));
	}
	findContours(processedImage, objList, RETR_EXTERNAL, CHAIN_APPROX_NONE);


	//
	// Convert the resulting matrices to 2channel floating point matrice.
	for (int i = 0; i < objList.size(); i ++) {
	  
	  Mat filledMatrix = Mat::ones(img.rows, img.cols, img.type());  
	  
		objList[i].convertTo(objList[i], CV_32FC2);
	  dft(objList[i], objList[i], 0);
	  objList[i].at<Vec2f>(0, 0)[0] = objList[i].at<Vec2f>(0, 0)[0] + 1;
	  dft(objList[i], objList[i], DFT_INVERSE);
	  
	  
	  Vec2f lastPoint = objList[i].at<Vec2f>(objList[i].rows - 1, 0);
	  for (int px = 0; px < objList[i].rows; px++) {
	    
	    Vec2f cPoint = objList[i].at<Vec2f>(px, 0);
	    
	    double unterteilung = 100;
	    for (int g = 0; g <= unterteilung; g++) {
	    
	      int v1 = (int) round(lastPoint[0] + g / 100.0 * (cPoint[0] - lastPoint[0]));
	      int v2 = (int) round(lastPoint[1] + g / 100.0 * (cPoint[1] - lastPoint[1]));
	      filledMatrix.at<uchar>(v1, v2) = 2;
	    }
	  }
	  
	  //TODO: Go through all objList[i] elements and write filledMatrix(..) = 2;
	  
	  if (filledMatrix.at<uchar>(0, 0) == 2) {
	    std:: cout << "fataler fehler. hier war ich zu bequem, kann man aber recht"
	               << "schnell beheben. Muss px finden am Rand von bild, das != 2.\n";
	  }
	  
	  //TODO: filledMatrix = 2 -> filledMatrix = 1;
	  seedGrowing(0, 0, filledMatrix);
	  
	  // seeed growing....:) hope that the border is not reached xDDD
	  
	  for (int r = 0; r < filledMatrix.rows; r++) {
	    for (int c = 0; c < filledMatrix.cols; c++) {
	      if (filledMatrix.at<uchar>(r, c) == 2) {
          filledMatrix.at<uchar>(r, c) = 1;     
	      }
	    }
	  }
	  
    result.push_back(filledMatrix);
    showImage(filledMatrix, "fm1", 0);
  
	}
	
	
	return result;
	
}













int main(int argc, char** argv) {

  // the amount of images. Names: 0... amount-1
  int amount = 57;
  
  // margins at the left and right respectively at the top and the bottom.
  double rowStart = 1.0 / 9.0;
  double colStart = 1.0 / 9.0;

  int amountDifferentLetters = 69;
  vector<string> names = vector<string>(amountDifferentLetters);
  names[0] = "A";
  names[1] = "B";
  names[2] = "C";
  names[3] = "D";
  names[4] = "E";
  names[5] = "F";
  names[6] = "G";
  names[7] = "H";
  names[8] = "I";
  names[9] = "J";
  names[10] = "K";
  names[11] = "L";
  names[12] = "M";
  names[13] = "N";
  names[14] = "O";
  names[15] = "P";
  names[16] = "Q";
  names[17] = "R";
  names[18] = "S";
  names[19] = "T";
  names[20] = "U";
  names[21] = "V";
  names[22] = "W";
  names[23] = "X";
  names[24] = "Y";
  names[25] = "Z";
  names[26] = "AE";
  names[27] = "OE";
  names[28] = "UE";
  names[29] = "a";
  names[30] = "b";
  names[31] = "c";
  names[32] = "d";
  names[33] = "e";
  names[34] = "f";
  names[35] = "g";
  names[36] = "h";
  names[37] = "i";
  names[38] = "j";
  names[39] = "k";
  names[40] = "l";
  names[41] = "m";
  names[42] = "n";
  names[43] = "o";
  names[44] = "p";
  names[45] = "q";
  names[46] = "r";
  names[47] = "s";
  names[48] = "t";
  names[49] = "u";
  names[50] = "v";
  names[51] = "w";
  names[52] = "x";
  names[53] = "y";
  names[54] = "z";
  names[55] = "ae";
  names[56] = "oe";
  names[57] = "ue";
  names[58] = "sz";
  names[59] = "0";
  names[60] = "1";
  names[61] = "2";
  names[62] = "3";
  names[63] = "4";
  names[64] = "5";
  names[65] = "6";
  names[66] = "7";
  names[67] = "8";
  names[68] = "9";

  /*
   *
   * LOAD IMAGES
   *
   */ 
  vector<Mat> sources = vector<Mat>(amount);

  for (int i = 0; i < amount; i++) {
  
    ostringstream convert;
    convert << "scripts/" << i << ".png";
    std:: string name = convert.str();    
    //
    // load image
    sources[i] = imread(name, 0);
    if (!sources[i].data){
      std:: cerr << "Error loading img from \t" << name << "\n";
      std:: cerr << "The program exits now.";
      std:: cin.get();
      exit(-1);
    }
  }

  /*
   *
   * PROCESS IMAGES
   *
   */ 
  int currentLetter = 0;
  int threshold = 0;
  int threshold2 = 0;
   
  for (int i = 0; i < amount; i++) {
  
    sources[i] = sources[i](
        Range(rowStart * sources[i].rows, sources[i].rows * (1 - rowStart)), 
        Range(colStart * sources[i].cols, sources[i].cols * (1 - colStart))); 
  
    //
    // 
    // Extract the text-lines
    vector<Vec2f> textRowsPage = vector<Vec2f>();
    Vec2f current = Vec2f(0, 0);
    int oldsum = 0;
    for (int r = 0; r < sources[i].rows; r++) {
      int sum = 0;
      for (int c = 0; c < sources[i].cols; c++) {
        sum += (sources[i].at<uchar>(r, c) <= 100);
      }
      
      //
      // from black to white
      if(sum <= threshold && oldsum > threshold && (r - current[0]) > 20) {
        current[1] = r;
        std:: cout << current[0] << ", " << current[1] << " of " << sources[i].rows << "\n";
        textRowsPage.push_back(current);
        std:: cout << current << "\n";
        current = Vec2f(r, 0);
      }
      
      //
      // from white to black
      if(sum != 0 && oldsum == 0) {
        current[0] = r;
      }
      oldsum = sum;
    }
  
  
    std:: cout << textRowsPage.size() << "\n";
  
    //
    //
    // Extract the letters in line for each line
    for (int line = 0; line < textRowsPage.size(); line++) {
      Mat d = sources[i](Range(textRowsPage[line][0], textRowsPage[line][1]),
                Range::all());
                
      //showImage(d, "row", 0);
         
      vector<Vec2f> lettersRow = vector<Vec2f>();
      
      
      
      // line detektieren
      
      
      Vec2f current = Vec2f(0, 0);
      int oldsum = 0;
      for (int c = 0; c < d.cols; c++) {
        int sum = 0;
        for (int r1 = 0; r1 < d.rows; r1++) {
        
          sum += (d.at<uchar>(r1, c) <= 200);
          
        }
          
        //
        // from black to white
        if((sum <= threshold2) && (oldsum > threshold2)) {
          current[1] = c;
          std:: cout << current[0] << ", " << current[1] << " of " << d.rows << "\n";
          lettersRow.push_back(current);
            
          current = Vec2f(0, 0);
        }
          
        //
        // from white to black
        if(sum != 0 && oldsum == 0) {
          current[0] = c;
        }
        oldsum = sum;
        
      }
      if (current[1] != 0) {
        current[2] = d.cols - 1;
        lettersRow.push_back(current);
      }
        
        
        
        
      for(int letterIndex = 0; letterIndex < lettersRow.size(); letterIndex++) {
          
        Mat letterImg = d(Range::all(), Range(lettersRow[letterIndex][0], lettersRow[letterIndex][1]));
        
        int letterName = currentLetter % amountDifferentLetters; //a, b, c, ...
        int letter = floor(currentLetter / amountDifferentLetters); //1.png, 2.png, ..
        std:: cout << "letter " << letter << "\n";
        if (letter == 24){// letterName == 0) {
          showImage(letterImg, names[letterName], 0);
        }
          
        currentLetter++;
      }
    }
  }

  return 0;
}
