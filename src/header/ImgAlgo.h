#pragma once

#define INFTY 0x3fffffff
#define BD_SIZE 3000
#define CHG_SIGN 100
#define INF 10
#define SUP 20000

typedef unsigned char uchar;

struct Rectangle {
	int xmin, xmax, ymin, ymax;		// 矩形范围
	double S;						// 工件面积
	int borlen;						// 边缘长度
	int rectS;						// 方框面积
	double inv;
	uchar** segment;				// 区域分割
	CPoint* border;					// 工件边缘
}typedef tarRect;

class ImgAlgo
{
private:
	/* 变量 */
	int row, col;
	double platform;			//平台面积
	//1~40

	int regA[BD_SIZE][2];	// 边缘寄存器A
	int regB[BD_SIZE][2];	// 边缘寄存器B
	bool edge[8];
	int area[SUP][2];		// 区域寄存器
	int pt;
	CPoint border[BD_SIZE];	// 边界寄存器
	int bt;
	bool isIn;
	double rule_inv;
	/* 函数 */
	tarRect findConnection(uchar**, int, int);
	uchar** createArea(tarRect);
	void kernelDilate(uchar**, uchar**, int, int);
	void fillConnected(uchar**, int, int, int*);
	void borderClear(int(*)[2]);
	void areaClear(int(*)[2]);
	void updStats(uchar**, int, int, int*);
	void atEdge(int, int);
	bool isBorder(uchar, int, int);
	CPoint* createBorder();

public:
	tarRect targets[50];
	int thresh = 100;	//0~255
	int top;
	void init(int height, int width);
	void imgNorm(uchar** imgArr, uchar* pimg, int height, int width);
	int getRectangles(uchar**);
	void release();
};

