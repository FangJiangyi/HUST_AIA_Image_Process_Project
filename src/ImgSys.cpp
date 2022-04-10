#include "stdafx.h"
#include "ImgSys.h"
#define REDCHAN 2
ImgSys::ImgSys(ImgDB*pdb, ImgAlgo*palgo)
{
    imgdatabase = pdb;
    algo = palgo;
    imgArr = NULL;
}
void ImgSys::preprocess(HANDLE cam, MV_IMAGE_INFO* pInfo, int zoomrate)
{
    int w, h;
    MV_PixelFormatEnums m_PixelFormat;
    MVImage tmpimage;
    MVGetWidth(cam, &w);
    MVGetHeight(cam, &h);
    MVGetPixelFormat(cam, &m_PixelFormat);
    tmpimage.CreateByPixelFormat(w, h, m_PixelFormat);//创建临时图片存储区
    if (!imgdatabase->Isempty(0))
    {
        imgdatabase->deleteimage(0);
        imgdatabase->newimage(0);
    }
    if(imgArr==NULL)
        imgArr = imgdatabase->newarray(h/zoomrate, w/zoomrate);//创建缩放后内存空间
    imgdatabase->newimage(h/zoomrate,w/zoomrate,m_PixelFormat,0);
    imgdatabase->newimage(h/zoomrate, w/zoomrate, PixelFormat_Mono8, 1);
    MVInfo2Image(cam, pInfo, &tmpimage);
    MVGetPixelFormat(cam, &m_PixelFormat);
	MVZoomImageBGR(cam, (uchar*)tmpimage.GetBits(),
        w, h, (uchar*)imgdatabase->getImageData(0), 
        1 / (double)zoomrate, 1 / (double)zoomrate);//利用自带缩放函数
    imgdatabase->threec2onec();
    tmpimage.Destroy();
}
void ZoomInImg(uchar*srcp,uchar*dstp,int w,int h,int channels,int zoomrate)
{
    int wdst = w / zoomrate;
    int hdst = h / zoomrate;
    
    for (int i = 0, k = 0; i < h && k < hdst; k++)
    {
        for (int j = 0, l = 0; j < w && l < wdst; l++)
        {
            for(int t=0;t<channels;t++)
            {
                *(dstp + t +l * channels + k * wdst * channels) = *(srcp + t + j * channels + i * w * channels);
            }
            j = j + zoomrate;
        }
        i = i + zoomrate;
    }
}
void ImgSys::preprocess(CString strFilePath, int zoomrate)
{
    MV_PixelFormatEnums pm;    MVImage tmpimage;
    if (!imgdatabase->Isempty(0)){
        imgdatabase->deleteimage(0);
        imgdatabase->newimage(0);
    }
    if (!imgdatabase->Isempty(1)){
        imgdatabase->deleteimage(1);
        imgdatabase->newimage(1);
    }
    tmpimage.Load(strFilePath);
    const int h = tmpimage.GetHeight();const int w = tmpimage.GetWidth();
    const int channels = tmpimage.GetBPP()/8;
    if (channels == 3)//进行预处理前图片像素格式判断
        pm = PixelFormat_BayerBG8;
    else
        pm = PixelFormat_Mono8;
    if (imgArr == NULL)
        imgArr = imgdatabase->newarray(h / zoomrate, w / zoomrate);
    imgdatabase->newimage(h / zoomrate, w / zoomrate, PixelFormat_Mono8, 1);
    imgdatabase->newimage(h/ zoomrate, w/ zoomrate, pm, 0);
    uchar* srcp = (uchar*)tmpimage.GetBits();uchar* dstp = (uchar*)imgdatabase->getImageData(0);
    ZoomInImg(srcp, dstp, w, h, channels, zoomrate);//自行编写的缩放函数
    imgdatabase->threec2onec();tmpimage.Destroy();
}


uchar** ImgSys::img2arr()
{
    int h = imgdatabase->getImageHeight(0);
    int w = imgdatabase->getImageWidth(0);
    int bpp = imgdatabase->getImageBpp(0);
    int oneline = imgdatabase->getImagePitch(0);
    uchar* p = (uchar*)imgdatabase->getImageData(0);
    if(oneline/w==3)
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                imgArr[i][j] = *(p + j * bpp / 8 + REDCHAN + i * oneline);
            }
        }
    }
    else
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                imgArr[i][j] = *(p + j * bpp / 8  + i * oneline);
            }
        }
    }
    return imgArr;
}

void ImgSys::runAlgo()
{
    //运行处理算法
    algo->init(imgdatabase->getImageHeight(0), imgdatabase->getImageWidth(0));
    algo->getRectangles(img2arr());
}

void ImgSys::showpre(CPoint point)
{
    CString stateinfo;
    uchar color[3] = {0,255,0};
    uchar color_[3] = {255,255};
    int flag = 0;
    for (int i = 0; i < 10; i++)
    {
        numsofeachtype[i] = 0;
    }
    if(!imgdatabase->Isempty(0))
        for (int i = 0; i < algo->top; i++)
        {
            // 识别分类后直接进行计数：有什么类，然后添加什么内容
            CString toolstate;
            imgdatabase->drawEdge(algo->targets[i].border, algo->targets[i].borlen, color);
            recognize(algo->targets[i].S*PS, algo->targets[i].inv, toolstate);
            if (point.x <= algo->targets[i].xmax && point.y <= algo->targets[i].ymax && point.x >= algo->targets[i].xmin && point.y >= algo->targets[i].ymin && flag==0)
            {
                imgdatabase->drawEdge(algo->targets[i].border, algo->targets[i].borlen, color_);
                stateinfo.Format("%s的实际面积：%.3lf平方厘米", toolstate,algo->targets[i].S*PS);
                flag = 1;
            }
        }
    
    imgdatabase->statebar = stateinfo;
    algo->release(); 
}


bool ImgSys::IsNull()
{
    if (!imgdatabase || !algo)
        return false;
    else
        return true;
}
// 对两个类进行初始化
void ImgSys::setDBandAlgo(ImgDB*pdb, ImgAlgo*palgo)
{
    imgdatabase = pdb;
    algo = palgo;
}
// 删除整个对象
void ImgSys::destroy()
{
    if (!imgdatabase->Isempty(0))
    {
        imgdatabase->deleteimage(0);
    }
    if (!imgdatabase->Isempty(1))
    {
        imgdatabase->deleteimage(1);
    }
    if (imgArr != NULL)
    {
        imgdatabase->deletearray(imgArr, imgdatabase->getImageHeight(1), imgdatabase
            ->getImageWidth(1));
    }
    delete imgdatabase;
    delete algo;
    imgdatabase = NULL;
    algo = NULL;
}
// 对工件进行识别
int ImgSys::recognize(double s,double m,CString &tooltype)
{
    int minindex = 9;
    double mindis = INFTY;
    const double limit = 300;
    for (int i = 0; i < 9; i++)
    {
        double tmp = 10000 * (m - invarr[i]) * (m - invarr[i]) + 100 * (s - sarr[i]) * (s - sarr[i]);
        if ( tmp < mindis)
        {
            mindis = tmp;
            minindex = i;
        }
    }
    if(mindis>limit)    //相似距离阈值
        minindex = 9;
    numsofeachtype[minindex]++;     //编号映射
    tooltype = typearr[minindex];
    return minindex;
}

