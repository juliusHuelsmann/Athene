#include "preprocessing.h"

int main(int argc, char** argv) {

  Mat d = imread("screenshot.png");
  preprocessing(d);
}
