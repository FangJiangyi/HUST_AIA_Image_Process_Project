#pragma once
#include "connect.h"
#include "ImgInput.h"
#include "ImgDB.h"
#include "toolrecognDlg.h"
#define FROMFILE 1
#define FROMCAMERA 0
#define COLOR 100

class ImgProceSys
{
private:
	Recognition reg;
	Connect con;
	ImgDB db;
public:
	MVImage* input(int width, int height, MV_PixelFormatEnums pixelformat);
	void algorithm();
	MVImage* show(); 
};

