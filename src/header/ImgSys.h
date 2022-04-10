#pragma once
#include "..\..\include\MVGigE.h"
#include "..\..\include\MVImage.h"
#include <atlimage.h>
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"
#include "ImgDB.h"
#include "ImgAlgo.h"
#include <algorithm>
#define PS 112.36
typedef unsigned char uchar;
extern class ImgDB;
extern class ImgAlgo;
const double sarr[9] = { 6.85,  7.4,   2.2,   0.545,   2.77,   0.09,   4.38,   7.84,   6.45};
const double invarr[9]{ 0.159,  0.299,   2.2,   1.13,   0.167,  0.88,   0.2,  0.721,   0.348};
const CString typearr[10]{ "圆形工件","乐高矩形","L型杆","直圆钉","方形","六角螺母","乐高锤子","乐高条","电池","未知"};
class ImgSys
{
private:
    
public:
    ImgSys(ImgDB*, ImgAlgo*);
    void preprocess(HANDLE, MV_IMAGE_INFO* pInfo,int);
    void preprocess(CString strFilePath, int);
    uchar** img2arr();
    void runAlgo();
    void showpre(CPoint);
    bool IsNull();
    void setDBandAlgo(ImgDB*, ImgAlgo*);
    void destroy();
    int recognize(double s, double m, CString& tooltype);
    ImgDB* imgdatabase;
    ImgAlgo* algo;
    uchar** imgArr;
    int numsofeachtype[10];
};

