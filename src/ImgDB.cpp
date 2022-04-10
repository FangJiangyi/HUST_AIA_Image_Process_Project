#include "stdafx.h"
#include "ImgDB.h"
//图片数据库初始化
ImgDB::ImgDB(MVImage m, MVImage i)
{
	m_image = m;
	imageprocessed = i;
}
//新建一张图片
void ImgDB::newimage(int height, int width, MV_PixelFormatEnums pm,int flag)
{
	if(!flag)
		m_image.CreateByPixelFormat(width, height, pm);
	else
		imageprocessed.CreateByPixelFormat(width, height, pm);
}
//新建一张图片
void ImgDB::newimage(int flag)
{
	MVImage newimg;
	if (!flag)
		m_image = newimg;
	else
		imageprocessed = newimg;
}
//获取图片句柄
MVImage* ImgDB::getImageHandle(int flag)
{
	if (!flag)
		return &m_image;
	else
		return &imageprocessed;
}
//删除图片
void ImgDB::deleteimage(int flag)
{
		if(!flag)
			m_image.Destroy();
		else
			imageprocessed.Destroy();
}
//判断图片数据库是否为空
bool ImgDB::Isempty(int flag)
{
	if (!flag)
		return m_image.IsNull();
	else
		return imageprocessed.IsNull();
}

unsigned char* ImgDB::getImageData(int flag)
{
	if(!flag)
		return (uchar*)m_image.GetBits();
	else
		return (uchar*)imageprocessed.GetBits();
}
//获取图片宽度
int ImgDB::getImageWidth(int flag)
{
	if (!flag)
		return m_image.GetWidth();
	else
		return imageprocessed.GetWidth();
}
//获取图片高度
int ImgDB::getImageHeight(int flag)
{
	if (!flag)
		return m_image.GetHeight();
	else
		return imageprocessed.GetHeight();
}
//获取图片每像素占位数
int ImgDB::getImageBpp(int flag)
{
	if (!flag)
		return m_image.GetBPP();
	else
		return imageprocessed.GetBPP();
}
//获取图片每行字节数
int ImgDB::getImagePitch(int flag)
{
	if (!flag)
		return m_image.GetPitch();
	else
		return imageprocessed.GetPitch();
}
//绘制图像边缘
void ImgDB::drawEdge(CPoint*pp, int size, uchar* color)//涂色还不太会
{
	uchar* p = getImageData(0);
	for (int i = 0; i < size; i++)
	{
		*(p + 0 + pp[i].x * getImageBpp(0) / 8 + (pp[i].y) * getImagePitch(0)) = (uchar)color[0];
		*(p + 1 + pp[i].x * getImageBpp(0) / 8 + (pp[i].y) * getImagePitch(0)) = (uchar)color[1];
		*(p + 2 + pp[i].x * getImageBpp(0) / 8 + (pp[i].y) * getImagePitch(0)) = (uchar)color[2];
	}
}
//新建二维数组并返回指针
uchar** ImgDB::newarray(int height, int width)
{
	uchar** p = new uchar * [height];
	for (int i = 0; i < height; i++)
	{
		p[i] = new uchar[width];
	}
	return p;
}
//删除二维数组
void ImgDB::deletearray(uchar**&p, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		delete[] p[i];
	}
	delete[] p;
	p = NULL;
}
//将三通道图像转化为单通道图像
void ImgDB::threec2onec()
{
	uchar* p1 = getImageData(1);
	uchar* p0 = getImageData(0);
	int h = getImageHeight(1);
	int w = getImageWidth(1);
	int bytespp = getImageBpp(0) / 8;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w ; j++)
		{
			*(p1 + j + i * w) = *(p0 + 2 + j * bytespp + i * w * bytespp);
		}
	}
}

