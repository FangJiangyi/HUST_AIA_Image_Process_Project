#pragma once

#define INFTY 0x3fffffff
#define BD_SIZE 3000
#define CHG_SIGN 100
#define INF 10
#define SUP 20000

typedef unsigned char uchar;

struct Rectangle {
	int xmin, xmax, ymin, ymax;		// ���η�Χ
	double S;						// �������
	int borlen;						// ��Ե����
	int rectS;						// �������
	double inv;
	uchar** segment;				// ����ָ�
	CPoint* border;					// ������Ե
}typedef tarRect;

class ImgAlgo
{
private:
	/* ���� */
	int row, col;
	double platform;			//ƽ̨���
	//1~40

	int regA[BD_SIZE][2];	// ��Ե�Ĵ���A
	int regB[BD_SIZE][2];	// ��Ե�Ĵ���B
	bool edge[8];
	int area[SUP][2];		// ����Ĵ���
	int pt;
	CPoint border[BD_SIZE];	// �߽�Ĵ���
	int bt;
	bool isIn;
	double rule_inv;
	/* ���� */
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

