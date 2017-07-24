#include "Head.h"

NumList* NumRec(IplImage* image_in)//��Ƭ�ڸ�����ĸ���ֵķָ�
{
	IplImage* Grey = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	Grey = SmoothBinary(image_in);
	IplImage* Thres = cvCreateImage(cvGetSize(image_in), IPL_DEPTH_8U, 1);
	cvThreshold(Grey, Thres, 0, 255, CV_THRESH_OTSU);


	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* contour = 0;
	RectList* list = (RectList*)malloc(sizeof(RectList));
	InitRectList(list);
	int contour_num = cvFindContours(Thres, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	for (; contour != NULL; contour = contour->h_next)
	{
		CvRect rect = cvBoundingRect(contour, 0);
		if (((rect.width)*(rect.height) < 1200) && ((rect.width)*(rect.height) >200) && (rect.width<100))//�˴�����ȱ�� ��������1���ΰ�Χ�����̫Сֱ�ӱ�����
		{
			AddRectToList(list, rect);
		}

	}


	//ð�ݷ�:x���������������
	for (int i = 0; i < list->length; i++)
	{
		for (int j = i + 1; j<list->length; j++)
		{
			if (list->rect[i].x > list->rect[j].x)
			{
				CvRect rect;
				rect = list->rect[i];
				list->rect[i] = list->rect[j];
				list->rect[j] = rect;
			}
		}
	}


	//���í��������í�������־��α�Ȼ���������ֵľ���Ҫ�����Դ���Ϊ�ж�����
	for (int i = 0; i < list->length; i++)
	{
		float rate = 1.100000;
		float rate2 = 0.100000;
		if ((float)list->rect[i].height >(rate*(float)list->rect[3].height))//�м�������ǿ϶�û��í���ģ����ĳ�����γ�����Ҫ��0.1�������ܺ���í��
		{
			//í����������,��ôy�������������ҪС
			if ((list->rect[3].y - list->rect[i].y) > (rate2*(float)list->rect[3].height))
			{
				CvRect rect = cvRect(list->rect[i].x, list->rect[i].y + (list->rect[i].height - list->rect[3].height), list->rect[i].width, list->rect[3].height);
				list->rect[i] = rect;
			}

			//í����������
			else{ CvRect rect = cvRect(list->rect[i].x, list->rect[i].y, list->rect[i].width, list->rect[3].height); }
		}

		rate = 0.900000;
		if ((float)list->rect[0].height < ((rate)*(float)list->rect[3].height))//�����һ�����α��м��ҪС�ܶ࣬������ʡ�ݵĺ��ֱ��ض�
		{
			CvRect rect = cvRect(list->rect[0].x, list->rect[0].y - list->rect[3].height + list->rect[0].height, list->rect[0].width, list->rect[3].height);
			list->rect[0] = rect;
		}
	}

	//�����8�����Σ����һ�������Ǹ����ɾȥ
	if (list->length == 8)
	{
		list->length--;
	}


	//��©���������7��������Ȼ��ǰ�沽�������1����©��
	if (list->length < 7)
	{
		for (int i = 0; i < list->length-1; i++)
		{
			//���ǰ���������־���λ���������������ο�ȣ��м�϶�©����һ��1
			if ((list->rect[i + 1].x - list->rect[i].x) >= (2 * list->rect[i+1].width))
			{
				CvRect rect1 = cvRect(list->rect[i].x + list->rect[i].width, list->rect[i].y, 15, 20);//��1�ľ�����Ϊ15*20��С�ģ�15*20�ں��������Ϊ�б�1�����ݣ�
				for (int j = list->length; j > i + 1; j--)
				{
					list->rect[j] = list->rect[j - 1];//������λ
				}
				list->rect[i + 1] = rect1;//����1
				list->length++;
			}
		}
	}

	//�ٴθĽ�����������1�����һλ�����Ծ�����һ��֮����Ȼ����7������ô���һλ��1(ѭ��5�Σ����ܳ��ƺ���11111)
	for (int i = 0; i <= 4; i++)
	{
		if (list->length < 7)
		{
			CvRect rect1 = cvRect(list->rect[list->length - 1].x + list->rect[list->length - 1].width, list->rect[list->length - 1].y, 15, 20);
			list->rect[list->length] = rect1;
			list->length++;
		}
	}


	//�����α߿�(û�б�Ҫ)
	//for (int i = 0; i < list->length; i++)
	//{
	//CvPoint point1 = cvPoint(list->rect[i].x, list->rect[i].y );
	//CvPoint point2 = cvPoint(list->rect[i].x + list->rect[i].width, list->rect[i].y + list->rect[i].height);
	//cvRectangle(image_in, point1, point2, CV_RGB(0, 255, 0), 1, 8, 0);
	//}
	//cvNamedWindow("numberrec");
	//cvShowImage("numberrec", image_in);


	//����������ROI���Ƶ��µ�ͼƬ���������Ա�
	NumList* numlist = (NumList*)malloc(sizeof(NumList));
	InitNumList(numlist);
	for (int i = 0; i < list->length; i++)
	{
		cvSetImageROI(image_in, list->rect[i]);
		IplImage* ROI = cvCreateImage(cvSize(list->rect[i].width, list->rect[i].height), IPL_DEPTH_8U, 3);
		cvCopy(image_in, ROI);
		AddNumToList(numlist, ROI);
		cvResetImageROI(image_in);
	}
	return numlist;
}