#include "detectshare.hpp"

#define PI 3.14159265

using namespace cv;
using namespace std;

// Método que auxilia na disposição do texto no centro do contorno
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{  // Parâmetros: Imagem, Texto, Contorno
	int fontface = cv::FONT_HERSHEY_SIMPLEX;  // Definição na fonte utilizada
	double scale = 0.4; // Tamanho da letra
	int thickness = 1;  // Espessura da letra
	int baseline = 0;  // Parâmetetro de saída - coordenada y do "baseline" relativo ao ponto do texto mais baixo

	// void cvGetTextSize(const char* text_string, const CvFont* font, CvSize* text_size, int* baseline)
	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline); // Obtém o tamanho do texto
	cv::Rect r = cv::boundingRect(contour);  // Declaração de um retângulo

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	
	// void cvPutText(CvArr* img, const char* text, CvPoint org, const CvFont* font, CvScalar color)
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

// Método que retorna o cosseno do ângulo entre dois vetores: pt0->pt1 e pt0->pt2, sendo pt (ponto)
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){  // Recebe 3 Pontos como parâmetro
	double dx1 = pt1.x - pt0.x;  // x1 - x0
	double dy1 = pt1.y - pt0.y;  // y1 - y0
	double dx2 = pt2.x - pt0.x;  // x2 - x0
	double dy2 = pt2.y - pt0.y;  // y2 - y0
	double dx3 = pt2.x - pt1.x;  // x3 - x0
	double dy3 = pt2.y - pt1.y;  // y3 - y0
	return abs(((dx3*dx3 + dy3*dy3) - (dx2*dx2 + dy2*dy2) - (dx1*dx1 + dy1*dy1))/(2*sqrt(dx2*dx2 + dy2*dy2)*sqrt(dx1*dx1 + dy1*dy1)));
}

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
 
void Detectshare(Mat& dst, std::vector<int>& forma){

	// Converte a imagem original para "grayscale"
	cv::Mat gray;
	cv::cvtColor(dst, gray, CV_BGR2GRAY);

	// Converte para uma imagem binária utilizando Canny
	cv::Mat bw;
	cv::Canny(gray, bw, 1000, 3000, 5);  // OBS: Esses parâmetros podem ser modificados

	// Encontra contornos
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// O array para guardar a curva aproximada
	std::vector<cv::Point> approx;

	int i = 0;  // Contador para ser utilizado no loop

	for (i = 0; i < contours.size(); i++){

		// Approximate contour with accuracy proportional
    		// to the contour perimeter
    		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

    		// Skip small or non-convex objects 
    		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
        		continue;

		if (approx.size() == 3)
		{
			forma.push_back(3);   // Triangles
			setLabel(dst, "TRI", contours[i]);
		}
		else 
		{
			if (approx.size() >= 4 /*&& approx.size() <= 9*/){
		
				// Number of vertices of polygonal curve
				int vtc = approx.size();

	 		       	// Get the degree (in cosines) of all corners
	 		       	std::vector<double> cos;
	 		       	for (int j = 2; j < vtc+1; j++)
	 		       		cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

	 	    		// Sort ascending the corner degree values
	 	      		std::sort(cos.begin(), cos.end());

	 		       	// Get the lowest and the highest degree
	 		       	double mincos = cos.front();
	 		       	double maxcos = cos.back();

	 		       	// Use the degrees obtained above and the number of vertices
	 		       	// to determine the shape of the contour
	 		       	if (vtc == 4){
					if (mincos >= -0.1 && maxcos <= 0.3)
					{
		 	           		// Detect rectangle or square
				    		cv::Rect r = cv::boundingRect(contours[i]);
				    		double ratio = std::abs(1 - (double)r.width / r.height);
						if (ratio <= 0.1)
						{
							forma.push_back(4);
							setLabel(dst, "QUA", contours[i]);  
						}
						else
						{
							forma.push_back(10);  
							setLabel(dst, "RET", contours[i]);
						}
					}
					else
					{
						double lado[4];
						int cont = 0, cont2 = 0, max = 0;
			
						for (int j = 1; j < cos.size()+1; j++)
						{
							lado[j-1] = sqrt(((approx[j-1].x-approx[j].x)*(approx[j-1].x-approx[j].x))+((approx[j-1].y-approx[j].y)*(approx[j-1].y-approx[j].y)));
						}
						for (int j = 0; j < cos.size()+1; j++)
						{
							for (int k = j; k < cos.size()+1; k++)
							{
								if ( lado[j] > lado[k])
								{
									max = lado[j];
								}
								else 
								{
									max = lado[k];
								}	
					
				
								if ( (j != k) && (lado[j]-lado[k]) <= (0.1*max))
								{
									cont++;
									break;
								}	
				
							}
						}
						if (cont == 1)
							{
								forma.push_back(1);  
								setLabel(dst, "TRA", contours[i]);
							}
							else
							{
								forma.push_back(2);  
								setLabel(dst, "PAR", contours[i]);
							}
	
					}
				}

				else
				{
					if (vtc == 5 && maxcos <= 0.38 && mincos >= 0.22)
					{
						forma.push_back(5); 
						setLabel(dst, "PENT", contours[i]); 
					}
					else
					{
						if (vtc == 6 && maxcos <= 0.59 && mincos >= 0.41)
				    		{
							forma.push_back(6);  
							setLabel(dst, "HEX", contours[i]);
						}
						else
						{
							if (vtc == 7 && maxcos <= 0.69 && mincos >= 0.53)
					    		{
								forma.push_back(7); 
								setLabel(dst, "HEP", contours[i]); 
							}
							else
							{
								if (vtc == 8 && maxcos <= 0.72 && mincos >= 0.68)
						    		{
									forma.push_back(8);  
									setLabel(dst, "OCT", contours[i]);
								}
								else
								{
									if (vtc == 9 && maxcos >= 0.69 && mincos <= 0.84)
							    		{
										forma.push_back(9); 
										setLabel(dst, "ENE", contours[i]); 
									}
									else
									{
										// Detect and label circles
										double area = cv::contourArea(contours[i]);
										cv::Rect r = cv::boundingRect(contours[i]);
										int radius = r.width / 2;
	
										if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 && std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2){
											forma.push_back(0);  
											setLabel(dst, "CIR", contours[i]);
										}
							    		}
								}
							}
						}
					}
				}
	    		}
		}
	}
}

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
/*
void imprime (cv::Mat& dst, vector<int> share)
{	
	// Converte a imagem original para "grayscale"
	cv::Mat gray;
	cv::cvtColor(dst, gray, CV_BGR2GRAY);

	// Converte para uma imagem binária utilizando Canny
	cv::Mat bw;
	cv::Canny(gray, bw, 1000, 3000, 5);  // OBS: Esses parâmetros podem ser modificados

	// Encontra contornos
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	for (int i = 0; i < contours.size(); i++)
	{
		//if (share.size() != contours.size())
			//break; 
		switch(share[i])
		{	case 0: 
				setLabel(dst, "CIR", contours[i]);
				break;
			case 1:
				setLabel(dst, "TRA", contours[i]);
				break;
			case 2:
				setLabel(dst, "PAR", contours[i]);
				break;
			case 3:
				setLabel(dst, "TRI", contours[i]);
				break;
			case 4:
				setLabel(dst, "QUA", contours[i]);
				break;
			case 5:
				setLabel(dst, "PEN", contours[i]);
				break;
			case 6:
				setLabel(dst, "HEX", contours[i]);
				break;
			case 7:
				setLabel(dst, "HEP", contours[i]);
				break;
			case 8:
				setLabel(dst, "OCT", contours[i]);
				break;
			case 9:
				setLabel(dst, "ENE", contours[i]);
				break;
			case 10:
				setLabel(dst, "REC", contours[i]);
				break;
		}
	}
}*/
