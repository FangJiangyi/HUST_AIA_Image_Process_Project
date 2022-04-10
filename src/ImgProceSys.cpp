#include "stdafx.h"
#include "ImgProceSys.h"
MVImage* ImgProceSys::input(int width,int height,MV_PixelFormatEnums pixelformat)
{
	if (db.Isempty())
		db.newimage(width, height, pixelformat,0);
	return db.getImageHandle(0);
}

void ImgProceSys::algorithm()
{
	
	con.pImg2Arr(db.getImageData(0), db.getImageWidth(0), db.getImageHeight(0), db.getImageBpp(0), db.getImageBytes(0));
	con.getRectangles(db.getImageWidth(0), db.getImageHeight(0), 1);
	for (int i = 0; i < con.getRecArrsize(); i++)
	{
		reg.setProperty(con.getRecData(i), con.getRecWidth(i), con.getRecHeight(i));
		reg.classify();
	}
}

MVImage* ImgProceSys::show()
{
	//这个位置需要重新设计一下：
	//1. 在鼠标点击图形区域时进入该函数（同时映射到第几个图形中）。
	//2. 更改imageprocessed指针指向的内容。
	//3. 根据映射到的几个图形找到位置。
	//4. 创建控件，显示信息，计时自动消失。
	int size = reg.getarrsize();
	for (int i = 0; i < size; i++)
	{
		db.putManyPixel(con.getEdge(i), COLOR);
		reg.getResult(i);
	}
	return NULL;
}
void type2context(ToolType)
{

}
