#pragma once
#include "..\..\include\MVGigE.h"
#include "..\..\include\MVImage.h"
#include "ImgSys.h"
#include <deque>
typedef unsigned char uchar;
class ImgDB
{
private:
		MVImage m_image;
		MVImage imageprocessed;

public:
		ImgDB(MVImage, MVImage);
		void newimage(int, int, MV_PixelFormatEnums,int);
		void newimage(int flag);
		MVImage* getImageHandle(int flag);
		void deleteimage(int);
		bool Isempty(int);
		uchar* getImageData(int);
		int getImageWidth(int);
		int getImageHeight(int);
		int getImageBpp(int);
		int getImagePitch(int);
		void drawEdge(CPoint* pp, int size, uchar* color);
		uchar** newarray(int, int);
		void deletearray(uchar**& p, int height, int width);
		void threec2onec();
		void setstring(int);
		void show();
		CString statebar;
};

