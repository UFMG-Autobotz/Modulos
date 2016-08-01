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
#define PI 3.14159265

using namespace cv;
using namespace std;

// Método que auxilia na disposição do texto no centro do contorno
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);

// Método que retorna o cosseno do ângulo entre dois vetores: pt0->pt1 e pt0->pt2, sendo pt (ponto)
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

//Correspondência dos números no vetor forma e a forma aproximada
//0 = círculo
//1 = trapezio
//2 = paralelogramo
//3 = triângulo
//4 = quadrado
//5 = pentágono
//6 = hexágono
//7 = heptágono
//8 = octógono
//9 = eneágono
//10 = retângulo
 
void Detectshare(Mat &dst, std::vector<int>& forma);

//void imprime (cv::Mat& dst, vector<int> share);
