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
	//���λ����Ҫ�������һ�£�
	//1. �������ͼ������ʱ����ú�����ͬʱӳ�䵽�ڼ���ͼ���У���
	//2. ����imageprocessedָ��ָ������ݡ�
	//3. ����ӳ�䵽�ļ���ͼ���ҵ�λ�á�
	//4. �����ؼ�����ʾ��Ϣ����ʱ�Զ���ʧ��
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
