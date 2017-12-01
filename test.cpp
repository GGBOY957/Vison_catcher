#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

 
void thresh_callback(int, void*);
double get_distance(Point2f pointO, Point2f pointA);

int main()
{
	//读写视频
	VideoCapture cap("1.mov");
	if (!cap.isOpened()){
		cout << "open fail"<<endl;
		return -1;
	}
	
	while (1){
		Mat frame; 
		Rect rightRect(640, 0, 640, 480);
		cap >> frame;
		if (frame.empty()) break;
		src = frame(rightRect);
		cvtColor(src, src_gray, CV_BGR2GRAY);
		thresh_callback(0, 0);
		imshow("frame", src);
		//writer << src;
		if (waitKey(30) >= 0) break;
	}
	waitKey(60);
	return(0);
}


void thresh_callback(int, void*)
{
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Point2f temp[2];
	double distance = 0;
	double distance2 = 0;
	int temp_num = 0;
	int temp_num2 = 0;
	int flag1=0;
	int flag2=0;
	Point2f rect_points[4];
	Point2f rect_points_temp[4];
	Point2f points_final[4];

	/// 二值化  
	threshold(src_gray, threshold_output, 250, 255, THRESH_BINARY);
	/// 寻找轮廓  
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// 对每个找到的轮廓创建可倾斜的边界框  
	vector<RotatedRect> minRect(contours.size());
	vector<RotatedRect> minEllipse(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		//获取轮廓四个点坐标
		minRect[i] = minAreaRect(Mat(contours[i]));

		double height = minRect[i].size.height;
		
	}
	//根据矩形高度冒泡排序
	for (int j = 0; j < contours.size(); j++){
		for (int i = contours.size()-1; i > j; i--){
			if (minRect[i].size.height > minRect[i - 1].size.height){
				RotatedRect roa_temp = minRect[i - 1];
				minRect[i - 1] = minRect[i];
				minRect[i] = roa_temp;
			}
		}
	}

	
	
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	minRect[0].points(rect_points);
	minRect[1].points(rect_points_temp);
	//区两个矩形内侧四点重新绘制矩形
	if (rect_points[0].x < rect_points_temp[0].x){
		points_final[0] = rect_points[3];
		points_final[1] = rect_points[2];
		points_final[2] = rect_points_temp[1];
		points_final[3] = rect_points_temp[0];
	}
	else{
		points_final[0] = rect_points_temp[3];
		points_final[1] = rect_points_temp[2];
		points_final[2] = rect_points[1];
		points_final[3] = rect_points[0];
	}
	for (int j = 0; j < 4; j++)
		line(src, points_final[j], points_final[(j + 1) % 4], color, 5, 16);
	cout <<"中心坐标  "<< (points_final[0] + points_final[1] + points_final[2] + points_final[3]) / 4 << endl;
	
}

