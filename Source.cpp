#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cmath>
#include <iostream>


using namespace cv;
using namespace std;


void setLabel(Mat& im, const string label, vector<Point>& contour)
{
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}





int main(int argc, const char** argv)
{
	//Read image
	String imageName("Shape.jpg"); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}

	Mat img;
	img = imread(imageName, IMREAD_COLOR);

	//Convert to gray scale
	Mat img_gray;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);

	//Use filter
	medianBlur(img_gray, img_gray, 3);

	//Find contours
	Mat img_edges;
	Canny(img_gray, img_edges, 50, 150, 3, true);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(img_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_KCOS);

	vector<vector<Point>> shapes(contours.size());

	int j(0);
	//Separate external contours
	for (int i = 0; i >= 0; i = hierarchy[i][0])
	{
		double area = contourArea(contours[i]);
		double perimeter = arcLength(contours[i], true);

		// Approximate contours with polygon
		approxPolyDP(contours[i], shapes[j], 0.04*perimeter, true);

		if ((abs(area) > 100) && (isContourConvex(shapes[j])))
		{

			

			//Count the number of vertices
			switch (shapes[j].size())
			{
			case 3:
			{
				Scalar color(rand() & 255, rand() & 255, rand() & 255);
				drawContours(img, shapes, j, color, 5);
				setLabel(img, "Tri", shapes[j]);
				break;
			}
			case 4:
			{
				Scalar color(rand() & 255, rand() & 255, rand() & 255);
				drawContours(img, shapes, j, color, 5);
				setLabel(img, "Rect", shapes[j]);
				break;
			}
			case 5:
			{
				Scalar color(rand() & 255, rand() & 255, rand() & 255);
				drawContours(img, shapes, j, color, 5);
				setLabel(img, "Pente", shapes[j]);
				break;
			}
			}

			//Split circles and polygons
			if (shapes[j].size() > 6)
			{
				Point2f center;
				float radius;
				minEnclosingCircle((Mat)contours[i], center, radius);
				double alpha = abs(1 - area / (CV_PI*pow(radius, 2)));
				if (alpha < 0.2)
				{

					Scalar color(rand() & 255, rand() & 255, rand() & 255);
					circle(img, center, (int)radius, color, 3, 8, 0);
					setLabel(img, "Circle", contours[i]);
				}
				else
				{
					Scalar color(rand() & 255, rand() & 255, rand() & 255);
					drawContours(img, shapes, j, color, 5);
					setLabel(img, "Poly", shapes[j]);
				}
			}
			j++;
		}

	}




	namedWindow("Image", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
	namedWindow("Image Edges", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
	imshow("Image", img);
	imshow("Image Edges", img_edges);
	waitKey(0);
	return 0;
}




