#ifndef _ALG_PEOPLE_COUNT_H_
#define _ALG_PEOPLE_COUNT_H_

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define Pi 3.1415926
#define Radian2Degree (180.0/Pi)
#define Degree2Radian (Pi/180.0)

#define PEOC_ROWS  240
#define PEOC_COLS  320

typedef struct
{
	float	fx;						// Focal length x (pixel)
	float	fy;						// Focal length y (pixel)
	float	cx;						// Principal point x (pixel)
	float	cy;						// Principal point y (pixel)
}PsCameraIntrinsic;					//Camera Intrinsic 

typedef struct
{
	int HighPoint[2];				//头部坐标
	float distance;					//人物距离相机的距离,单位 mm
	float duration_time;			//停留时间
	bool isValid;					//标识识别的人物是否稳定有效
}PeopleInfoStruct;

class ALG_PeopleCount
{
public:
	ALG_PeopleCount();
	~ALG_PeopleCount(){}
	bool Init(PsCameraIntrinsic cam);
	bool Run(Mat &DepthData);
	
	bool ClearCountResult();		//当需要清零 统计信息时候调用该接口
	void MyDrawContours(cv::Mat &drawimg, std::vector<cv::Point> &contour);//显示轮廓
	int savePointCloud();//保存点云
public:
	int residence_count;							//累计 停留超过预设时间的人物数量
	PeopleInfoStruct peopleInfo[5000];				//保存人头部的坐标，人的停留时间等信息
	std::vector<std::vector<cv::Point>> m_contours;	//当前视野中所有人的轮廓信息
	int total_number;								//视野中人物数量（包含临态，不稳定人物）
	int people_number_statistics;					//视野中有效的人物数量
};

#endif
