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
	/*Size s(640, 480);
	VideoWriter writer = VideoWriter("mov2.avi",CV_FOURCC('M','J','P','G'),30,s);
	if (!writer.isOpened()){
		cout << "Creat Fail"<<endl;
		return -1;
	}*/
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
		//Size2f size = minRect[i].size;
		double height = minRect[i].size.height;
		//roa_temp = minRect[i];
		/*Point2f vertices[4];
		minRect[i].points(vertices);
		if ((vertices[0].y - vertices[1].y) < (vertices[1].x - vertices[2].x))
			minRect[i] = minAreaRect(Mat(contours[i+1]));
		/*if (contours[i].size() > 5)
		{
			minEllipse[i] = fitEllipse(Mat(contours[i]));
		}*/
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

	/*for (int j = 0; j < 4; j++){
		Point2f para1[4];
		minRect[j].points(para1);
		for (int i = 3; i >j ; i--){
			Point2f para2[4];
			minRect[i].points(para2);
			if (((para1[0].y - para1[1].y) / (para1[0].x - para1[1].x)) == ((para2[0].y - para2[1].y) / (para2[0].y - para2[1].y))){
				RotatedRect roa_temp = minRect[0];
				minRect[0] = minRect[i];
				minRect[i] = roa_temp;
				roa_temp = minRect[1];
				minRect[1] = minRect[j];
				minRect[j] = roa_temp;
				flag1 = j;
				flag2 = i;
			}
		}
	}*/
	//Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	//
	/*for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		minRect[i].points(rect_points_temp);
		minRect[i].points(rect_points);
		if (minRect[i].size.height> distance){
			distance = minRect[i].size.height;
			temp_num = i;
		}
	}*/
	/*for (int i = 0; i< contours.size(); i++)
	{
		if (i == temp_num) continue;
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		minRect[i].points(rect_points_temp);
		minRect[i].points(rect_points);
		double a, b;
		a = get_distance(rect_points[0], rect_points[1]);
		b = get_distance(rect_points[1], rect_points[2]);
		if (get_distance(rect_points_temp[0], rect_points_temp[1]) > distance){
			distance = get_distance(rect_points_temp[0], rect_points_temp[1]);
			temp_num = i;
		}
	}*/
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
	/*for (int i = 0; i < contours.size(); i++){
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		minRect[i].points(rect_points_temp);
		minRect[i].points(rect_points);
		if (i == temp_num){
			for (int j = 0; j < 4; j++)
				line(src, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
		}
	}*/
}
//计算两点距离，没用上
double get_distance(Point2f pointO, Point2f pointA)
{
	double distance;
	distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);
	distance = sqrtf(distance);

	return distance;
}
