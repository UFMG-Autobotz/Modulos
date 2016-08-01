#include <opencv2/imgproc/imgproc.hpp>	
#include <opencv2/highgui/highgui.hpp>	
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  
#include <string.h> 
#include <iostream> 
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cv.h> 
#include <highgui.h>
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "opencv/cxcore.h"
#include "opencv2/core/core.hpp"
#include <sstream>
#include <string> 
#include <iostream> 
#include "detectshare.cpp"
#include "detectshare.hpp"

#define PI 3.14159265

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	Mat dst;
	dst = imread( argv[1], 1);
	vector<int> share;
	share.clear();
	
	Detectshare(dst, share);
	
	//imprime (dst, share);

	// Exibe a imagem processada
	namedWindow("Final Image"); 
	imshow("Final Image", dst);
	cv::waitKey(0);
}
