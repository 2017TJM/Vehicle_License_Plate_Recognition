#include "Head.h"

IplImage* SecondRec(IplImage* image_in)//��Ƭ������һ����ȡ
{
	IplImage* image1 = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 3);
	IplImage* image2 = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 3);
	IplImage* HSV = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 3);
	cvSmooth(image_in, image1, CV_GAUSSIAN, 3, 3);
	cvSmooth(image1, image2, CV_MEDIAN, 3, 3);
	cvCvtColor(image2, HSV, CV_BGR2HSV);
	IplImage* H = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	IplImage* S = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	IplImage* V = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	//�ָ�ΪHSV��ͨ��
	cvSplit(HSV, H, S, V, 0);
	//�ж�Hͨ����ÿһ��ֵ��������100-130��ɫ�ĵ��Ϊ��
	int step = H->widthStep;
	for (int i = 0; i < H->height; i++){
		for (int j = 0; j < H->width; j++)
		{
			if (((*(H->imageData + step*i + j) < 100) || *(H->imageData + step*i + j) >130)){ *(H->imageData + step*i + j) = 0; }
		}
	}
	//�ٴ�������ȡ��ȥ����Ϊ�������
	IplImage* Thres = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	cvThreshold(H, Thres, 0, 255, CV_THRESH_OTSU);
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* contour = 0;
	RectList* list = (RectList*)malloc(sizeof(RectList));
	InitRectList(list);
	int contour_num = cvFindContours(Thres, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	for (; contour != NULL; contour = contour->h_next)
	{
		CvRect rect = cvBoundingRect(contour, 0);
		if ((rect.width)*(rect.height) > 1000)
		{
			AddRectToList(list, rect);
		}
	}

	IplImage* imagecopy = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 3);
	cvCopy(image_in, imagecopy);
	cvSetImageROI(imagecopy, list->rect[0]);
	IplImage* ROI = cvCreateImage(cvSize(list->rect[0].width, list->rect[0].height), IPL_DEPTH_8U, 3);
	cvCopy(imagecopy, ROI);
	return ROI;
}