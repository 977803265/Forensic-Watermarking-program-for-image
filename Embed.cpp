#pragma once
#include "stdafx.h"
#include "Headers.h"

////////////////////////////////////////////////////////////////////////////////////////
// ����
////////////////////////////////////////////////////////////////////////////////////////
Mat EmbedWatermark(Mat& HostImg, Mat& QrImg)
{
	return Embed(HostImg, QrImg);
}

Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat WT_result;
	Mat IWT_result;
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode �� �ȼ� ���� 255, 0���� ������ ��� ���� ����

	// QR�� �����͸� 0�� 255�� ����
	for (int y = 0; y < QrImg.rows; y++)
		for (int x = 0; x < QrImg.cols; x++)
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr); // ä�� �и�

	WT(yuv_arr[0], WT_result, 1); // Yä-���� ������� 1�ܰ� DWT ����

	// �δ뿪�� ����� ������ ��� ����    
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
    HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));

	// DCT�� ������ 8x8 ũ���� ����
	Size blockSize(8, 8);
	vector<Mat> LH_blocks;
	vector<Mat> HH_blocks;
	vector<Mat> HL_blocks;
	int value[1024]; // QR�� ���� ���� ������ �迭
	int i = 0;
	// 256x256 ũ���� �δ뿪�� 1024���� 8x8 �� ������� ����
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			LH_blocks.push_back(Mat(LH_subband, rect));
			HH_blocks.push_back(Mat(HH_subband, rect));
			HL_blocks.push_back(Mat(HL_subband, rect));
			// ���� ���� ���� �Ķ���� ����
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}
	// 1024���� 8*8 ��Ͽ� dct ����� ���� �� ��dct ����
	for (int i = 0; i < 1024; i++)
	{
		dct(LH_blocks[i], LH_blocks[i]);
		dct(HH_blocks[i], HH_blocks[i]);
		dct(HL_blocks[i], HL_blocks[i]);

		LH_blocks[i].at<float>(0, 0) += value[i] * 25;	// ������ ������ ���� �Ķ���͸� �̿��Ͽ� ���� �̹����� DC ���� �߰�
		HH_blocks[i].at<float>(0, 0) += value[i] * 25;	// ������ ������ ���� �Ķ���͸� �̿��Ͽ� ���� �̹����� DC ���� �߰�
		HL_blocks[i].at<float>(0, 0) += value[i] * 25;	// ������ ������ ���� �Ķ���͸� �̿��Ͽ� ���� �̹����� DC ���� �߰�
		
		dct(LH_blocks[i], LH_blocks[i], DCT_INVERSE);
		dct(HH_blocks[i], HH_blocks[i], DCT_INVERSE);
		dct(HL_blocks[i], HL_blocks[i], DCT_INVERSE);
	}

	// IWT ����
	IWT(WT_result, IWT_result, 1);
	IWT_result.convertTo(yuv_arr[0], CV_8U);
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB

	// ����� ������ ���� �κ�
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;		// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;	// default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;
	param95[1] = 95;

	imwrite("Marked_image.png", yuv);
	imwrite("Marked_image_75.jpg", yuv, param75);
	imwrite("Marked_image_95.jpg", yuv, param95);

	////////////////////// ���� ���� �̿��� ���� ///////////////////////////
/*	Mat attack = imread("[blackgreygradient]DWT_DCT_CRT_LH.png");
	Mat GblurredImg1, GblurredImg2, Ablur, Mblur;
	Mat SPimg;

	GaussianBlur(attack, GblurredImg1, Size(3, 3), 0);
	GaussianBlur(attack, GblurredImg2, Size(5, 5), 0);
	blur(attack, Ablur, Size(3, 3));                  //Average blurring
	medianBlur(attack, Mblur, 3);

	SPimg = attack.clone();
	SaltandPepper(SPimg, 0.03);

	imwrite("2Gaussian Blurring 3x3.png", GblurredImg1);
	imwrite("2Gaussian Blurring 5x5.png", GblurredImg2);
	imwrite("2Average Blurring 3x3.png", Ablur);
	imwrite("2Median Blurring 3x3.png", Mblur);
	imwrite("2Salt and Pepper 3%.png", SPimg);
*/
	return yuv;
}