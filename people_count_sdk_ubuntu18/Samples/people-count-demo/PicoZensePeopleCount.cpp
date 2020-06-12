#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "PicoZense_api2.h"
#include "PicoZense_enums.h"
#include <thread>
#include "ALG_PeopleCount.h"
#include "zenselog.h"
#include <chrono>
#include <bits/stdc++.h>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

using namespace std;
using namespace cv;

int LOG_LEVEL = LOG_INFO;
int DBG_DISPLAY = 0;
int LOG_TO_FILE = 1;

//HM: For logging purposes.
static char time_str[100];

static char *get_local_and_gm_time_str() {
        time_t tt;
        struct tm *local_ti;
        struct tm *utc_ti;

        time(&tt);
        local_ti = localtime(&tt);
        utc_ti = gmtime(&tt);

        char *local_time = asctime(local_ti);
        char *utc_time = asctime(utc_ti);
        int local_time_len = strlen(local_time);
        int utc_time_len = strlen(utc_time);

        strncpy(time_str, local_time, local_time_len);
        strncpy(&time_str[local_time_len - 1], " | ", 3);
        strncpy(&time_str[local_time_len + 2], utc_time, strlen(utc_time) - 1);

        return time_str;
}

static char *get_local_time_str() {
        time_t tt;
        struct tm *local_ti;

        time(&tt);
        local_ti = localtime(&tt);

        char *local_time = asctime(local_ti);
        int local_time_len = strlen(local_time);
        local_time[local_time_len - 1] = '\0';
        strncpy(time_str, local_time, local_time_len - 1);
        return time_str;
}


// HM: Logging changes.
// Log evens to file.
// change is an integer which specifies the type of event to be logged.
// Since the log file should be append only, adding App started/App stopped events.
// 0 - People count changed
// 1 - Residence count changed
// 2 - Application started.
// 3 - Application stopped.
static void log_to_file(int change, int people_cnt, int residence_cnt) {
		ofstream logfile;

		logfile.open("tof-log.txt", ios::out | ios::app);

		if (change == 0) {
				logfile << get_local_time_str() 
						<< " | People    Count Changed " 
						<< " | People Count "
						<< people_cnt
						<< " | Residence Count "
						<< residence_cnt
						<< endl;
		}
		else if (change == 1) {
				logfile << get_local_time_str() 
						<< " | Residence Count Changed " 
						<< " | People Count "
						<< people_cnt
						<< " | Residence Count "
						<< residence_cnt
						<< endl;
		}
		else if (change == 2) {
				logfile << get_local_time_str() 
						<< " | Application started.    " 
						<< " | People Count "
						<< people_cnt
						<< " | Residence Count "
						<< residence_cnt
						<< endl;
		}
		else if (change == 3) {
				logfile << get_local_time_str() 
						<< " | Application stopped.    " 
						<< " | People Count "
						<< people_cnt
						<< " | Residence Count "
						<< residence_cnt
						<< endl;
		}

		logfile.close();
}


static void exit_log_fn(void) {
		log_to_file(3, 0, 0);
}

static void sigint_hdlr(int sig) {
		printf("Exiting program.\n");
		exit(0);
}

static void sigterm_hdlr(int sig) {
		printf("Exiting program.\n");
		exit(0);
}


uint32_t slope = 1450;			//For near range, slope is 1450; For far range, slope is 4400
PsDepthRange depthRange = PsXFarRange;
PsDataMode dataMode = PsDepthAndRGB_30;
PsCameraParameters cameraParameters_tof;
PsCameraParameters cameraParameters_rgb;
PsMeasuringRange measuringrange = { 0 };

static int init_device(PsDeviceHandle deviceHandle, uint32_t sessionIndex)
{
	PsReturnStatus status;

	status = Ps2_SetDataMode(deviceHandle, sessionIndex, dataMode);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_SetDataMode failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "Ps2_SetDataMode succeed: %d\n", dataMode);

	printf("depthRange: %d\n", depthRange);
	status = Ps2_SetDepthRange(deviceHandle, sessionIndex, depthRange);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_SetDepthRange failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "Ps2_SetDepthRange succeed: %d\n", depthRange);

	status = Ps2_SetThreshold(deviceHandle, sessionIndex, 20);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_SetThreshold failed: %d\n", status);
	}

	//Enable the Depth and RGB synchronize feature
	Ps2_SetSynchronizeEnabled(deviceHandle, sessionIndex, false);

	//Set PixelFormat as PsPixelFormatBGR888 for opencv display
	Ps2_SetColorPixelFormat(deviceHandle, sessionIndex, PsPixelFormatBGR888);

	status = Ps2_GetCameraParameters(deviceHandle, sessionIndex, PsDepthSensor, &cameraParameters_tof);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraParameters about TOF failed: %d\n", status);
		return -1;
	}
	else
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraParameters about TOF succeed.\n");
		cout << "Depth Camera Intinsic: " << endl;
		cout << "Fx: " << cameraParameters_tof.fx << endl;
		cout << "Cx: " << cameraParameters_tof.cx << endl;
		cout << "Fy: " << cameraParameters_tof.fy << endl;
		cout << "Cy: " << cameraParameters_tof.cy << endl;
		cout << "Depth Distortion Coefficient: " << endl;
		cout << "K1: " << cameraParameters_tof.k1 << endl;
		cout << "K2: " << cameraParameters_tof.k2 << endl;
		cout << "P1: " << cameraParameters_tof.p1 << endl;
		cout << "P2: " << cameraParameters_tof.p2 << endl;
		cout << "K3: " << cameraParameters_tof.k3 << endl;
		cout << "K4: " << cameraParameters_tof.k4 << endl;
		cout << "K5: " << cameraParameters_tof.k5 << endl;
		cout << "K6: " << cameraParameters_tof.k6 << endl;
	}

	status = Ps2_GetCameraParameters(deviceHandle, sessionIndex, PsRgbSensor, &cameraParameters_rgb);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraParameters about RGB failed: %d\n", status);
	}
	else
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraParameters about RGB succeed.\n");
		cout << "RGB Camera Intinsic: " << endl;
		cout << "Fx: " << cameraParameters_rgb.fx << endl;
		cout << "Cx: " << cameraParameters_rgb.cx << endl;
		cout << "Fy: " << cameraParameters_rgb.fy << endl;
		cout << "Cy: " << cameraParameters_rgb.cy << endl;
		cout << "RGB Distortion Coefficient: " << endl;
		cout << "K1: " << cameraParameters_rgb.k1 << endl;
		cout << "K2: " << cameraParameters_rgb.k2 << endl;
		cout << "K3: " << cameraParameters_rgb.k3 << endl;
		cout << "P1: " << cameraParameters_rgb.p1 << endl;
		cout << "P2: " << cameraParameters_rgb.p2 << endl;
	}

	PsCameraExtrinsicParameters CameraExtrinsicParameters;
	status = Ps2_GetCameraExtrinsicParameters(deviceHandle, sessionIndex, &CameraExtrinsicParameters);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraExtrinsicParameters failed: %d\n", status);
		return -1;
	}
	else
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetCameraExtrinsicParameters succeed\n");
		cout << "Camera rotation: " << endl;
		cout << CameraExtrinsicParameters.rotation[0] << " "
			<< CameraExtrinsicParameters.rotation[1] << " "
			<< CameraExtrinsicParameters.rotation[2] << " "
			<< CameraExtrinsicParameters.rotation[3] << " "
			<< CameraExtrinsicParameters.rotation[4] << " "
			<< CameraExtrinsicParameters.rotation[5] << " "
			<< CameraExtrinsicParameters.rotation[6] << " "
			<< CameraExtrinsicParameters.rotation[7] << " "
			<< CameraExtrinsicParameters.rotation[8] << " "
			<< endl;

		cout << "Camera transfer: " << endl;
		cout << CameraExtrinsicParameters.translation[0] << " "
			<< CameraExtrinsicParameters.translation[1] << " "
			<< CameraExtrinsicParameters.translation[2] << " " << endl;
	}

	//Get MeasuringRange
	status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, depthRange, &measuringrange);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetMeasuringRange failed: %d\n", status);
		return -1;
	}
	else
	{
		switch (depthRange)
		{
		case PsNearRange:
		case PsXNearRange:
		case PsXXNearRange:
			slope = measuringrange.effectDepthMaxNear;
			break;

		case PsMidRange:
		case PsXMidRange:
		case PsXXMidRange:
			slope = measuringrange.effectDepthMaxMid;
			break;

		case PsFarRange:
		case PsXFarRange:
		case PsXXFarRange:
			slope = measuringrange.effectDepthMaxFar;
			break;
		default:
			break;
		}
		ZENSE_LOG(LOG_INFO, "Ps2_GetMeasuringRange succeed: range:%d, slope: %d\n",
			depthRange, slope);
	}

	return 0;
}

/*
 * 1. PsInitialize()
 * 2. Ps2_GetDeviceCount
 * 3. Ps2_GetDeviceListInfo
 * 4. Ps2_OpenDevice
 * 5. Ps2_StartStream
 * 6. init_device
 * 7. process data and display people-count algorithm result
 */

int main(int argc, char *argv[])
{
	PsReturnStatus status;
	int ret = -1;
	uint32_t deviceIndex = 0;
	uint32_t deviceCount = 0;
	uint32_t slope_max = 8275;

	status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "PsInitialize failed: %d\n", status);
		return -1;
	}

GET:
	status = Ps2_GetDeviceCount(&deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "PsGetDeviceCount failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "Get device count: %d\n", deviceCount);
	if (0 == deviceCount)
	{
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}

	PsDeviceInfo* pDeviceListInfo = new PsDeviceInfo[deviceCount];
	status = Ps2_GetDeviceListInfo(pDeviceListInfo, deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetDeviceListInfo failed: %d\n", status);
		return -1;
	}

	PsDeviceHandle deviceHandle = 0;
	status = Ps2_OpenDevice(pDeviceListInfo->uri, &deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "OpenDevice failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "OpenDevice succeed\n");

	uint32_t sessionIndex = 0;
	status = Ps2_StartStream(deviceHandle, sessionIndex);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_StartStream failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "startStream: %d\n", status);

	ret = init_device(deviceHandle, sessionIndex);
	if(ret < 0)
	{
		ZENSE_LOG(LOG_INFO, "init_device failed: %d\n", ret);
		return -1;		
	}

	cv::Mat imageMat;
	const string irImageWindow = "IR Image";
	const string depthImageWindow = "Depth Image";

	PsDepthVector3 DepthVector = { 0, 0, 0 };
	PsVector3f WorldVector = { 0.0f };

	bool f_bDistortionCorrection = false;
	bool f_bFilter = false;
	bool f_bInvalidDepth2Zero = false;
	bool f_bSync = false;

	status = Ps2_GetDataMode(deviceHandle, sessionIndex, &dataMode);
	if (status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_GetDataMode failed: %d\n", status);
		return -1;
	}
	ZENSE_LOG(LOG_INFO, "Ps2_GetDataMode succeed: %d\n", dataMode);

	cout << "\n--------------------------------------------------------------------" << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "Press following key to set corresponding feature:" << endl;
	cout << "M/m: Change data mode: input corresponding index in terminal:" << endl;
	cout << "                    0: Output Depth and RGB in 30 fps" << endl;
	cout << "                    1: Output IR and RGB in 30 fps" << endl;
	cout << "                    2: Output Depth and IR in 30 fps" << endl;
	cout << "0/1/2...: Change depth range Near/Middle/Far..." << endl;
	cout << "P/p: Save point cloud data into PointCloud.txt in current directory" << endl;
	cout << "U/u: Enable or disable the distortion correction feature" << endl;
	cout << "Esc: Program quit " << endl;
	cout << "--------------------------------------------------------------------" << endl;
	cout << "--------------------------------------------------------------------\n" << endl;


	// HM: Logging changes. Log entry message
	log_to_file(2, 0, 0);

	// HM: Logging changes. Register exit function.
	atexit(exit_log_fn);
	signal(SIGTERM, sigterm_hdlr);
	signal(SIGINT, sigint_hdlr);

	ALG_PeopleCount* pPeopleCount = new ALG_PeopleCount();

	float downsampleratio = 2.0;
	PsCameraIntrinsic campara;
	campara.cx = cameraParameters_tof.cx/ downsampleratio;
	campara.cy = cameraParameters_tof.cy/ downsampleratio;
	campara.fx = cameraParameters_tof.fx/ downsampleratio;
	campara.fy = cameraParameters_tof.fy/ downsampleratio;

	pPeopleCount->Init(campara);


	cout << "Entering main loop" << endl;
	if (DBG_DISPLAY == 0) {
		printf("Debug Display is not enabled.\n");
	}
	else {
		printf("Debug Display is enabled\n");
	}

	printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	int loop_counter = 1;
	auto start = std::chrono::system_clock::now();

	// HM: Logging change.
	int prev_people_cnt = 0;
	int prev_residence_cnt = 0;

	for (;;)
	{

		//printf("... Loop counter: %d\n", loop_counter);
		if (loop_counter % 10 == 0) {
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			std::time_t end_time = std::chrono::system_clock::to_time_t(end);
			//std::cout << loop_counter << ". " <<  elapsed_seconds.count() << "s.\n";

			start = std::chrono::system_clock::now();
		}

		PsFrame depthFrame = { 0 };
		PsFrame irFrame = { 0 };

		// Read one frame before call PsGetFrame
		PsFrameReady frameReady = { 0 };
		status = Ps2_ReadNextFrame(deviceHandle, sessionIndex, &frameReady);
		// printf("... Frame buffer return status: %d, OK: %d\n", (int)status, (int)PsRetOK);
		if (status != PsRetOK)
		{
			goto KEY;
		}

		if (1 == frameReady.depth)
		{
			//printf("... ... frameReady.depth: %d", frameReady.depth);
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);

			if (depthFrame.pFrameData != NULL)
			{
				//printf("... Read frame buffer");

				Mat RawDepth = cv::Mat(depthFrame.height, depthFrame.width, CV_16UC1, depthFrame.pFrameData);
				resize(RawDepth, RawDepth, Size(), 0.5, 0.5);

				if (loop_counter % 10 == 0) {
					// ZENSE_LOG(LOG_INFO, "before Run. width:%d, height:%d\n", RawDepth.cols, RawDepth.rows);
				}

				if(false == pPeopleCount->Run(RawDepth)) {
					ZENSE_LOG(LOG_INFO, "Run failed.\n");
					goto IrFramePart;
				}

				//Display the Depth Image
				imageMat = cv::Mat(depthFrame.height, depthFrame.width, CV_16UC1, depthFrame.pFrameData);
				imageMat.convertTo(imageMat, CV_8U, 255.0 / slope_max);
				cv::applyColorMap(imageMat, imageMat, cv::COLORMAP_RAINBOW);

				char temp[255];

				for (int i = 0; i < pPeopleCount->total_number; i++)
				{
					if(pPeopleCount->peopleInfo[i].isValid)
					{
        		        cv::Point headpoint = cv::Point((pPeopleCount->peopleInfo[i].HighPoint[0]*2),
						 								(pPeopleCount->peopleInfo[i].HighPoint[1]*2));
						// //printf("*******label=%d, headpoint=%d, %d*****\n", i, headpoint.x,headpoint.y);
						int tmpx = headpoint.x - 25;
						int tmpy = headpoint.y - 25;
						tmpx = (tmpx < 0) ? 0 : tmpx;
						tmpy = (tmpy < 0) ? 0 : tmpy;
						cv::Point LeftPoint = cv::Point(tmpx, tmpy);
						
						tmpx = headpoint.x + 25;
						tmpy = headpoint.y + 25;
						tmpx = (tmpx > 639) ? (639) : tmpx;
						tmpy = (tmpy > 479) ? (479) : tmpy;
						cv::Point RightPoint = cv::Point(tmpx, tmpy);
						
				        cv::rectangle(imageMat, LeftPoint, RightPoint, cv::Scalar(0, 0, 0), 2);
				        cv::circle(imageMat, headpoint, 11, cv::Scalar(0, 0, 255), -1, 8);

						cv::Point string_show_point1;
						cv::Point string_show_point2;

						string_show_point1 = cv::Point(headpoint.x + 25, headpoint.y - 25);
						string_show_point2 = cv::Point(headpoint.x + 25, headpoint.y - 5);
						
						float distance = pPeopleCount->peopleInfo[i].distance;
                        snprintf(temp, 50, "D: %.0f ", distance);
				        cv::putText(imageMat, temp,
									string_show_point2,
									cv::FONT_HERSHEY_SIMPLEX,
									0.6,
									cv::Scalar(0, 0, 0),
									2,
									9);

						float duration_time = pPeopleCount->peopleInfo[i].duration_time;
                        snprintf(temp, 50, "R: %.1f", duration_time);
				        cv::putText(imageMat, temp,
									string_show_point1,
									cv::FONT_HERSHEY_SIMPLEX,
									0.6,
									cv::Scalar(0, 0, 0),
									2,
									9);

			            pPeopleCount->MyDrawContours(imageMat, pPeopleCount->m_contours[i]);
					}
				}

				cv::putText(imageMat, "People:" + std::to_string(pPeopleCount->people_number_statistics),
							cv::Point(100, 30),
							cv::FONT_HERSHEY_PLAIN,
							2,
							cv::Scalar(0, 0, 0),
							3);
				cv::putText(imageMat, "AD Count:" + std::to_string(pPeopleCount->residence_count),
							cv::Point(300, 30),
							cv::FONT_HERSHEY_PLAIN,
							2,
							cv::Scalar(0, 0, 0),
							3);

				if (DBG_DISPLAY == 1) {
						namedWindow(depthImageWindow, WINDOW_AUTOSIZE);
						cv::imshow(depthImageWindow, imageMat);
				}
				
				// HM: Add this code if we want to print out individual frames.
				//char fileName [20];
				//snprintf(fileName, 20, "%d.jpg", loop_counter);
				//cv::imwrite(fileName, imageMat);
				loop_counter++;

				// HM: This code prints statistics to a log file.
				if (pPeopleCount->people_number_statistics != prev_people_cnt) {
						printf("%s | People Count Changed    | People Count %d | AD Count %d\n", get_local_time_str(), pPeopleCount->people_number_statistics, pPeopleCount->residence_count);
						prev_people_cnt = pPeopleCount->people_number_statistics;

						if (LOG_TO_FILE == 1) {
								log_to_file(0, pPeopleCount->people_number_statistics, pPeopleCount->residence_count);
						}
				}

				if (pPeopleCount->residence_count != prev_residence_cnt) {
						printf("%s | Residence Count Changed | People Count %d | AD Count %d\n", get_local_time_str(), pPeopleCount->people_number_statistics, pPeopleCount->residence_count);
						prev_residence_cnt = pPeopleCount->residence_count;

						if (LOG_TO_FILE == 1) {
								log_to_file(1, pPeopleCount->people_number_statistics, pPeopleCount->residence_count);
						}
				}
				
			}
			else
			{
				ZENSE_LOG(LOG_INFO, "Ps2_GetFrame PsDepthFrame failed, pFrameData is NULL\n");
			}
		}

		IrFramePart:
		if (1 == frameReady.ir)
		{
			status = Ps2_GetFrame(deviceHandle, sessionIndex, PsIRFrame, &irFrame);

			if (irFrame.pFrameData != NULL)
			{
				//Display the IR Image
				imageMat = cv::Mat(irFrame.height, irFrame.width, CV_16UC1, irFrame.pFrameData);
				// Convert 16bit IR pixel (max pixel value is 3840) to 8bit for display
				imageMat.convertTo(imageMat, CV_8U, 255.0 / 3840);
				cvtColor(imageMat, imageMat, CV_GRAY2BGR);

				char temp[255];

				//namedWindow(irImageWindow, WINDOW_AUTOSIZE);
				//resize(imageMat, imageMat,Size(),3.0,3.0);
				//cv::imshow(irImageWindow, imageMat);
			}
			else
			{
				ZENSE_LOG(LOG_INFO, "Ps2_GetFrame PsIrFrame failed, pFrameData is NULL\n");
			}
		}
	KEY:
		unsigned char key = waitKey(1);
		//imageMat.release();

		if (key == ' ')
		{
			pPeopleCount->ClearCountResult();
		}
		if (key == 'M' || key == 'm')
		{
			cout << "Selection: 0:DepthAndRgb_30; 1:IrAndRgb_30; 2:DepthAndIR_30;" << endl;
			int index = -1;
			cin >> index;
			//clear buffer and cin flag. if not, cin will not get input anymore;
			if (cin.fail())
			{
				std::cout << "Unexpected input\n";
				cin.clear();
				cin.ignore(1024, '\n');
				continue;
			}
			else
			{
				cin.clear();
				cin.ignore(1024, '\n');
			}

			if((index < 0) ||(index > 2))
			{
				ZENSE_LOG(LOG_INFO, "Unsupported data mode: %d\n", index);
				continue;
			}
			status = Ps2_SetDataMode(deviceHandle, sessionIndex, (PsDataMode)index);
			if (status != PsRetOK)
			{
				cout << "Ps2_SetDataMode  status" << status << endl;
				continue;
			}
			dataMode = (PsDataMode)index;
		}
		else if( (key <= '8') && (key >= '0'))
		{
			status = Ps2_SetDepthRange(deviceHandle, sessionIndex, (PsDepthRange)(key - '0'));
			if (status != PsRetOK)
			{
				ZENSE_LOG(LOG_INFO, "Set depth range failed: %d\n", status);
			}
			else
			{
				depthRange = (PsDepthRange)(key - '0');
				ZENSE_LOG(LOG_INFO, "Set depth range succeed: %d\n", depthRange);
			}

			status = Ps2_GetMeasuringRange(deviceHandle, sessionIndex, depthRange, &measuringrange);
			if (status != PsReturnStatus::PsRetOK)
			{
				ZENSE_LOG(LOG_INFO, "Ps2_GetMeasuringRange failed: %d\n", status);
			}	
			else
			{
				switch (depthRange)
				{
				case PsNearRange:
				case PsXNearRange:
				case PsXXNearRange:
					slope = measuringrange.effectDepthMaxNear;
					break;

				case PsMidRange:
				case PsXMidRange:
				case PsXXMidRange:
					slope = measuringrange.effectDepthMaxMid;
					break;

				case PsFarRange:
				case PsXFarRange:
				case PsXXFarRange:
					slope = measuringrange.effectDepthMaxFar;
					break;
				default:
					break;
				}
				ZENSE_LOG(LOG_INFO, "Ps2_GetMeasuringRange succeed: %d-%d\n", depthRange, slope);
			}
		}
		else if (key == 'P' || key == 'p')
		{
			if (depthFrame.pFrameData != NULL)
			{
				pPeopleCount->savePointCloud();
			}
		}
		else if (key == 'T' || key == 't')
		{
			//Set background filter threshold
			static uint16_t threshold = 0;
			threshold += 10;
			if (threshold > 100)
				threshold = 0;
			status = Ps2_SetThreshold(deviceHandle, sessionIndex, threshold);
			ZENSE_LOG(LOG_INFO, "set threhold value: %d, status:%d\n", threshold, status);
		}
		else if (key == 'U' || key == 'u')
		{
			Ps2_SetDepthDistortionCorrectionEnabled(deviceHandle, sessionIndex, f_bDistortionCorrection);
			Ps2_SetIrDistortionCorrectionEnabled(deviceHandle, sessionIndex, f_bDistortionCorrection);
			Ps2_SetRGBDistortionCorrectionEnabled(deviceHandle, sessionIndex, f_bDistortionCorrection);
			cout << "Set DistortionCorrection " << (f_bDistortionCorrection ? "Enabled." : "Disabled.") << endl;
			f_bDistortionCorrection = !f_bDistortionCorrection;
		}
		else if (key == 27)	//ESC Pressed
		{
			break;
		}
	}

	cv::destroyAllWindows();

	status = Ps2_CloseDevice(deviceHandle);
	if(status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_CloseDevice failed: %d\n", status);
	}

	status = Ps2_Shutdown();
	if(status != PsReturnStatus::PsRetOK)
	{
		ZENSE_LOG(LOG_INFO, "Ps2_Shutdown failed: %d\n", status);
	}

	return 0;
}
