#include "stdafx.h"
#include "ImgAlgo.h"

void ImgAlgo::init(int height, int width) {
	row = height;
	col = width;
	platform = 0;
}


void ImgAlgo::imgNorm(uchar** imgArr, uchar* pimg, int height, int width) {
	uchar pvalue;
	int min = 256, max = -1;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			pvalue = *(pimg + j + i * width);
			if (pvalue < min) min = pvalue;
			if (pvalue > max) max = pvalue;
		}

	if (max == min) max = min + 1;
	/* Norm+Binary	timecost: <2ms	size: 640*480 */
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			pvalue = uchar((imgArr[i][j] - min) * 255 / (max - min));
			*(pimg + j + i * width) = pvalue > thresh ? 0 : 255; // targets - 255
		}
}



int ImgAlgo::getRectangles(uchar** imgArr) {
	uchar pvalue;
	int min = 256, max = -1;
	uchar** origin = new uchar * [row];

	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++) {
			pvalue = imgArr[i][j];
			if (pvalue < min) min = pvalue;
			if (pvalue > max) max = pvalue;
		}

	/* Norm+Binary	timecost: <2ms	size: 640*480 */
	for (int i = 0; i < row; i++) {
		origin[i] = new uchar[col];
		for (int j = 0; j < col; j++) {
			pvalue = uchar((imgArr[i][j] - min) * 255 / (max - min));
			imgArr[i][j] = pvalue > thresh ? 0 : 255; // targets - 255
			origin[i][j] = imgArr[i][j];
			if (!imgArr[i][j]) platform++;
		}
	}
	/* Dilate */
	for (int i = 1; i < row - 1; i++)
		for (int j = 1; j < col - 1; j++)
			kernelDilate(imgArr, origin, i, j);
	for (int i = 0; i < row; i++)
		delete origin[i];
	delete origin;
	
	
	/* Find Connection */
	tarRect suspect;
	top = 0;
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
			if (imgArr[i][j] == 255) {
				suspect = findConnection(imgArr, i, j);
				if (suspect.S > INF && suspect.S < SUP) {
					suspect.segment = createArea(suspect);
					suspect.border = createBorder();
					platform += suspect.S;
					suspect.inv = rule_inv, suspect.borlen = bt;
					suspect.rectS = (suspect.xmax - suspect.xmin + 1) * (suspect.ymax - suspect.ymin + 1);
					targets[top++] = suspect;
				}
			}
	for (int i = 0; i < top; i++)
		targets[i].S /= platform;

	tarRect tmp;
	for (int i = 0; i < top; i++)
		for (int j = 0; j < top - 1 - i; j++) 
			if (targets[j].rectS > targets[j + 1].rectS) {
				tmp = targets[j];
				targets[j] = targets[j + 1];
				targets[j + 1] = tmp;
			}
	return top;
}


//创建segment并计算不变矩
uchar** ImgAlgo::createArea(tarRect object) {
	int index = 0, px, py;
	int dy = object.ymax - object.ymin + 1;
	int dx = object.xmax - object.xmin + 1;
	double Mc = 0, Mx = 0, My = 0, rule = 0;

	uchar** workpiece = new uchar * [dy];
	for (int i = 0; i < dy; i++) {
		workpiece[i] = new uchar[dx];
		for (int j = 0; j < dx; j++)
			workpiece[i][j] = 0;
	}

	while (area[index][0] > -1) {
		py = area[index][0] - object.ymin;
		px = area[index][1] - object.xmin;
		workpiece[py][px] = 255;
		Mc += 1, Mx += px, My += py;
		index++;
	}
	Mx /= Mc, My /= Mc;
	index = 0;
	while (area[index][0] > -1) {
		py = area[index][0] - object.ymin;
		px = area[index][1] - object.xmin;
		rule += pow(py - My, 2) + pow(px - Mx, 2);
		index++;
	}
	rule = rule / Mc / Mc;
	rule_inv = rule;

	return workpiece;
}


tarRect ImgAlgo::findConnection(uchar** img, int py, int px) {
	tarRect target = {};
	int stats[5] = { INFTY, -1, INFTY, -1, 0 }; // xmin, xmax, ymin, ymax, S
	fillConnected(img, py, px, stats);
	target.ymin = stats[0];
	target.ymax = stats[1];
	target.xmin = stats[2];
	target.xmax = stats[3];
	target.S = stats[4];
	return target;
}


void ImgAlgo::kernelDilate(uchar** imgArr, uchar** origin, int cy, int cx) {
	double sum = 0;
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
			if (origin[cy + i][cx + j]) sum++;
	if (abs(sum - 4.5) <= 2.5)
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				imgArr[cy + i][cx + j] = 0;
}


void ImgAlgo::fillConnected(uchar** img, int py, int px, int* stats) {
	int index = 0, cnt = 0;
	int ey, ex;
	int(*border)[2] = regA, (*regter)[2] = regB;
	areaClear(area);
	borderClear(border), borderClear(regter);
	border[0][0] = py, border[0][1] = px;
	while (border[0][0] > -1) {
		while (border[index][0] > -1) {
			ey = border[index][0], ex = border[index][1];
			isIn = false, atEdge(ey, ex);
			if (--ex > -INFTY && edge[0] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (++ey > -INFTY && edge[1] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (++ex > -INFTY && edge[2] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (++ex > -INFTY && edge[3] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (--ey > -INFTY && edge[4] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (--ey > -INFTY && edge[5] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (--ex > -INFTY && edge[6] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			if (--ex > -INFTY && edge[7] && isBorder(img[ey][ex], ey, ex) && img[ey][ex] > CHG_SIGN) regter[cnt][0] = ey, regter[cnt++][1] = ex, updStats(img, ey, ex, stats);
			index++;
		}
		borderClear(border);
		border = (border == regA) ? regB : regA;
		regter = (regter == regA) ? regB : regA;
		index = 0, cnt = 0;
	}
}


void ImgAlgo::borderClear(int(*border)[2]) {
	for (int i = 0; i < BD_SIZE; i++)
		border[i][0] = -1, border[i][1] = -1;
}


void ImgAlgo::areaClear(int(*area)[2]) {
	pt = 0, bt = 0;
	for (int i = 0; i < SUP; i++)
		area[i][0] = -1, area[i][1] = -1;
	for (int i = 0; i < BD_SIZE; i++)
		border[i].x = -1, border[i].y = -1;
}


void ImgAlgo::updStats(uchar** img, int ey, int ex, int* stats) {
	img[ey][ex] = CHG_SIGN;
	if (ey < stats[0]) stats[0] = ey;
	if (ey > stats[1]) stats[1] = ey;
	if (ex < stats[2]) stats[2] = ex;
	if (ex > stats[3]) stats[3] = ex;
	stats[4]++;

	area[pt][0] = ey, area[pt][1] = ex;
	pt++;

	if (pt == SUP) pt--;
}


void ImgAlgo::atEdge(int py, int px) {
	bool N = px - 1 > -1;
	bool E = py + 1 < row;
	bool S = px + 1 < col;
	bool W = py - 1 > -1;

	// N, NE, E, SE, S, SW, W, NW
	edge[0] = N, edge[1] = N && E, edge[2] = E, edge[3] = S && E;
	edge[4] = S, edge[5] = S && W, edge[6] = W, edge[7] = N && W;
}


bool ImgAlgo::isBorder(uchar val, int py, int px) {
	if (!isIn && !val) isIn = true, border[bt].y = py, border[bt++].x = px;
	return true;
}


CPoint* ImgAlgo::createBorder() {
	CPoint* wpBorder;
	wpBorder = new CPoint[bt];
	for (int i = 0; i < bt; i++) {
		wpBorder[i] = border[i];
		
	}
	return wpBorder;
}



void ImgAlgo::release() {
	int w, h;
	for (int i = 0; i < top; i++) {
		h = targets[i].ymax - targets[i].ymin + 1;
		w = targets[i].xmax - targets[i].xmin + 1;
		for (int j = 0; j < h; j++) delete targets[i].segment[j];
		delete targets[i].segment;
		delete targets[i].border;
	}
}
