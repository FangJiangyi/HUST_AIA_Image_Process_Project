// CrossDlg.cpp : implementation file
// 可以继续改进的地方，现在使能menu item和逻辑值的处理有些重复，可以回去画一个状态转换机进行算一下。
// 增加识别算法类，然后继续对我的图像数据类做一定的维护修改，对源代码进一步简练，考虑增加状态栏
// 可以考虑换成收集灰度图
#include "stdafx.h"
#include "toolrecogn.h"
#include "toolrecognDlg.h"
#include <ctime>
#include <cstdio>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define C 3
#define A 6
#define B 7

// CtoolrecognDlg dialog

time_t t1=0, t2=0;
//CWinThread* pThread = NULL;
CtoolrecognDlg::CtoolrecognDlg(CWnd* pParent /*=NULL*/)
: CDialog(CtoolrecognDlg::IDD, pParent)
, m_hCam(NULL)
,imgsys(NULL,NULL)
,count(0)
, etrs(0)
,state(0)
,m_Point(-1,-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitializeCriticalSection(&m_csInvalidate);
}

void CtoolrecognDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_StateText, statebar);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Text(pDX, IDC_EDITTHRESH, etrs);
	DDX_Control(pDX, IDC_THRESH, m_thresh);
	DDX_Control(pDX, IDC_FPS, editfps);
	DDX_Control(pDX, IDC_STATICFPS, m_fps);
}

BEGIN_MESSAGE_MAP(CtoolrecognDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_COMMAND(ID_OpenImage, &CtoolrecognDlg::OnOpenimage)
	ON_COMMAND(ID_OpenExCam, &CtoolrecognDlg::OnOpenexcam)
	ON_COMMAND(ID_StartGrab, &CtoolrecognDlg::OnStartgrab)
	ON_COMMAND(ID_CloseCam, &CtoolrecognDlg::OnClosecam)
	ON_COMMAND(ID_ClearImage, &CtoolrecognDlg::OnClearimage)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CtoolrecognDlg message handlers

BOOL CtoolrecognDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	m_Menu.LoadMenu(IDR_MENU1);  //  IDR_MENU1

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//设置相机
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("函数库初始化失败"), _T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("查找连接计算机的相机失败！"), _T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	//设置菜单与快捷键
	SetMenu(&m_Menu);
	hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU1));
	//初始化系统
	MVImage img1;
	MVImage img2;
	imgsys.setDBandAlgo(new ImgDB(img1,img2), new ImgAlgo);
	//初始化控件
	m_thresh.SetRange(80, 255);
	m_thresh.SetTicFreq(1);
	//初始化表格
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);
	CImageList m_imageList;
	m_imageList.Create(1, 40, ILC_COLOR24, 1, 1);
	m_List.SetImageList(&m_imageList, LVSIL_SMALL);
	m_List.ModifyStyle(0, LVS_REPORT);               // 报表模式  
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	const int nColumnWidth = 200;
	const int FontW = 30;
	const int FontH = 30;
	const int FontWeight = 2;
	CFont font;
	font.CreateFontA(FontH, FontW, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("宋体"));
	m_List.SetFont(&font,1);
	m_List.InsertColumn(0, _T("种类"), LVCFMT_CENTER, nColumnWidth);
	m_List.InsertColumn(1, _T("个数"), LVCFMT_CENTER, nColumnWidth);
	for (int i = 0; i < 10; i++) {
		m_List.InsertItem(i, NULL); 
	}
	m_List.SetItemText(0/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("圆形工件"));
	m_List.SetItemText(1/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("乐高矩形"));
	m_List.SetItemText(2/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("L型杆"));
	m_List.SetItemText(3/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("直圆钉"));
	m_List.SetItemText(4/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("方形"));
	m_List.SetItemText(5/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("六角螺母"));
	m_List.SetItemText(6/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("乐高锤子"));
	m_List.SetItemText(7/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("乐高条"));
	m_List.SetItemText(8/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("电池"));
	m_List.SetItemText(9/*行从0开始，0为标题*/, 0/*列从0开始，0为第一列*/, _T("未知"));
	GetMenu()->EnableMenuItem(ID_ClearImage, MF_BYCOMMAND | MF_DISABLED|MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartProcess, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_PauseProcess, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//初始化某些字体
	CFont font_;
	bool success=font_.CreateFontA(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("Arial"));
	m_fps.SetFont(&font_,1);
	m_fps.SetWindowText(_T("FPS"));
	editfps.SetFont(&font_);
	statebar.SetFont(&font_);
	font_.DeleteObject();
	font.DeleteObject();
	
	
	InvalidateRect(NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
	
}
BOOL CtoolrecognDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN && GetDlgItem(pic)->GetSafeHwnd() == pMsg->hwnd)
		OnLButtonDown(MK_LBUTTON, pMsg->pt);
	if (::TranslateAccelerator(GetSafeHwnd(), hAccel, pMsg))
		return   true;
	return CDialog::PreTranslateMessage(pMsg);
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtoolrecognDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		if(!imgsys.IsNull())
		{
			DrawImage(NULL);
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtoolrecognDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtoolrecognDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	if( m_hCam != NULL )
	{
		MVCloseCam(m_hCam);
	}
	if(!imgsys.IsNull())
		imgsys.destroy();
	MVTerminateLib();
}


int __stdcall StreamCB(MV_IMAGE_INFO *pInfo,ULONG_PTR nUserVal)
{
	CtoolrecognDlg *pDlg = (CtoolrecognDlg *)nUserVal;
	return ( pDlg->OnStreamCB(pInfo) );
}

int CtoolrecognDlg::OnStreamCB( MV_IMAGE_INFO *pInfo)
{
	MVImage*pshowimg=NULL;
	t2 = t1;
	t1 = clock();
	int gap = t1 - t2;
	if (gap == 0) gap = 100;
	int fps = int(double(1) / (double)gap * 1000);
	CString tmp;
	tmp.Format("%d", fps);
	editfps.SetWindowTextA(tmp);

	//////////////////////////////////////////////////////////////////////////
	/**************************************************************/
	//进行图像预处理（类似于大小倍数上的转换）
	imgsys.preprocess(m_hCam,pInfo,ZOOMRATIO);
	//运行抽象信息提取算法
	imgsys.runAlgo();
	//为最终显示到设备的图像做准备 新图片 状态栏 帧率队列 浮动文本 
	imgsys.showpre(m_Point);
	for (int i = 0; i < 10; i++)
	{
		CString tmps;
		tmps.Format("%d", imgsys.numsofeachtype[i]);
		m_List.SetItemText(i/*行从0开始，0为标题*/, 1/*列从0开始，0为第一列*/, (LPCTSTR)tmps);
	}
	/**************************************************************/
	DrawImage(m_Point);
	return 0;
}

void CtoolrecognDlg::DrawImage(CPoint p)
{
	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);
	int dstW = rct.Width();
	int dstH = rct.Height();
	
	CDC* pDC = GetDlgItem(pic)->GetDC();
	if (pDC == NULL)
		return;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(165, 130, 255));
	if (!imgsys.imgdatabase->Isempty(0))
		imgsys.imgdatabase->getImageHandle(0)->Draw(pDC->GetSafeHdc(), 0, 0, dstW, dstH);
	statebar.SetWindowTextA(imgsys.imgdatabase->statebar);
	

}

void CtoolrecognDlg::OnOpenimage()
{
	// TODO: 在此添加命令处理程序代码
	TCHAR szFilter[] = _T("文本文件(*.BMP)|*.BMP|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("BMP"), NULL, 0, szFilter, this);
	CString strFilePath;
	MVImage* simg=NULL;
	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFilePath = fileDlg.GetPathName();
	}
	t1 = time(NULL);
	if (!strFilePath.IsEmpty())
	{
		imgsys.preprocess(strFilePath, 2);
		imgsys.runAlgo();
		imgsys.showpre(m_Point);
		DrawImage(m_Point);
	}
	GetMenu()->EnableMenuItem(ID_OpenImage, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_ClearImage, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_StartProcess, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	state = 1;
}


void CtoolrecognDlg::OnOpenexcam()
{
	// TODO: 在此添加命令处理程序代码
	int nCams = 0;
	MVGetNumOfCameras(&nCams);
	if (nCams == 0)
	{
		MessageBox(_T("没有找到相机，请确认连接和相机IP设置"), _T("提示"), MB_ICONWARNING);
		return;
	}
	MVSTATUS_CODES r = MVOpenCamByIndex(0, &m_hCam);
	if (m_hCam == NULL)
	{
		if (r == MVST_ACCESS_DENIED)
			MessageBox(_T("无法打开相机，可能正在被别的软件控制"), _T("提示"), MB_ICONWARNING);
		else
			MessageBox(_T("无法打开相机"), _T("提示"), MB_ICONWARNING);
		return;
	}
	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	state = 2;
}


void CtoolrecognDlg::OnStartgrab()
{
	// TODO: 在此添加命令处理程序代码
	TriggerModeEnums enumMode;
	MVGetTriggerMode(m_hCam, &enumMode);
	if (enumMode != TriggerMode_Off)
	{
		MVSetTriggerMode(m_hCam, TriggerMode_Off);
		Sleep(100);
	}
	MVStartGrab(m_hCam, (MVStreamCB)StreamCB, (ULONG_PTR)this);

	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_StartProcess, MF_BYCOMMAND | MF_ENABLED);
	state = C;
}


void CtoolrecognDlg::OnClosecam()
{
	// TODO: 在此添加命令处理程序代码
	MVStopGrab(m_hCam);
	MVCloseCam(m_hCam);
	this->RedrawWindow();
	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	state = 0;
}
void CtoolrecognDlg::OnClearimage()
{
	// TODO: 在此添加命令处理程序代码
	this->RedrawWindow();
	if (!imgsys.imgdatabase->Isempty(0))
	{
		imgsys.imgdatabase->deleteimage(0);
	}
	if (!imgsys.imgdatabase->Isempty(1))
	{
		imgsys.imgdatabase->deleteimage(1);
	}
	if (imgsys.imgArr != NULL)
	{
		imgsys.imgdatabase->deletearray(imgsys.imgArr, imgsys.imgdatabase->getImageHeight(1), imgsys.imgdatabase
			->getImageWidth(1));
	}
	GetMenu()->EnableMenuItem(ID_ClearImage, MF_BYCOMMAND | MF_DISABLED|MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_OpenImage, MF_BYCOMMAND | MF_ENABLED);
	state = 0;
}

void CtoolrecognDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect IRect;
	CRect tmpRect;
	
	state = A;
	GetDlgItem(pic)->GetWindowRect(&IRect);
	if (((point.x >= IRect.left) && (point.x <= IRect.right)) 
		&& (point.y >= IRect.top && point.y <= IRect.bottom))//判断鼠标点击坐标是否在图像区
	{
		m_Point.x = point.x - IRect.left;
		m_Point.y = point.y - IRect.top;
	}
	m_Point.x = m_Point.x * imgsys.imgdatabase->getImageWidth(0)  / (IRect.Width());//将鼠标点击坐标转化为像素坐标
	m_Point.y = m_Point.y * imgsys.imgdatabase->getImageHeight(0) / (IRect.Height());
	if (count >= 10000)
		count = 0;
	q.push(count);
	SetTimer(count, 2000, NULL);
	count++;
	CDialog::OnLButtonDown(nFlags, point);
}



void CtoolrecognDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	state = C;
	if(q.back()==nIDEvent)	//队列中最后一个定时器到时
		m_Point = { -1,-1 };//焦点复位
	KillTimer(nIDEvent);	//删除定时器
	q.pop();				//弹出队列
	CDialog::OnTimer(nIDEvent);
}



void CtoolrecognDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	CWnd* pSlidCtrl = GetDlgItem(IDC_THRESH);
	if (pScrollBar == pSlidCtrl)
	{
		etrs = ((CSliderCtrl*)pSlidCtrl)->GetPos();
		imgsys.algo->thresh= ((CSliderCtrl*)pSlidCtrl)->GetPos();
		if (state == 1)
		{
			imgsys.img2arr();
			state = 5;
		}
	}
	UpdateData(false);
}

