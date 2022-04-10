// CrossDlg.cpp : implementation file
// ���Լ����Ľ��ĵط�������ʹ��menu item���߼�ֵ�Ĵ�����Щ�ظ������Ի�ȥ��һ��״̬ת����������һ�¡�
// ����ʶ���㷨�࣬Ȼ��������ҵ�ͼ����������һ����ά���޸ģ���Դ�����һ����������������״̬��
// ���Կ��ǻ����ռ��Ҷ�ͼ
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

	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	m_Menu.LoadMenu(IDR_MENU1);  //  IDR_MENU1

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//�������
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("�������ʼ��ʧ��"), _T("��ʾ"), MB_ICONWARNING);
		return TRUE;
	}
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("�������Ӽ���������ʧ�ܣ�"), _T("��ʾ"), MB_ICONWARNING);
		return TRUE;
	}
	//���ò˵����ݼ�
	SetMenu(&m_Menu);
	hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU1));
	//��ʼ��ϵͳ
	MVImage img1;
	MVImage img2;
	imgsys.setDBandAlgo(new ImgDB(img1,img2), new ImgAlgo);
	//��ʼ���ؼ�
	m_thresh.SetRange(80, 255);
	m_thresh.SetTicFreq(1);
	//��ʼ�����
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);
	CImageList m_imageList;
	m_imageList.Create(1, 40, ILC_COLOR24, 1, 1);
	m_List.SetImageList(&m_imageList, LVSIL_SMALL);
	m_List.ModifyStyle(0, LVS_REPORT);               // ����ģʽ  
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	const int nColumnWidth = 200;
	const int FontW = 30;
	const int FontH = 30;
	const int FontWeight = 2;
	CFont font;
	font.CreateFontA(FontH, FontW, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("����"));
	m_List.SetFont(&font,1);
	m_List.InsertColumn(0, _T("����"), LVCFMT_CENTER, nColumnWidth);
	m_List.InsertColumn(1, _T("����"), LVCFMT_CENTER, nColumnWidth);
	for (int i = 0; i < 10; i++) {
		m_List.InsertItem(i, NULL); 
	}
	m_List.SetItemText(0/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("Բ�ι���"));
	m_List.SetItemText(1/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("�ָ߾���"));
	m_List.SetItemText(2/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("L�͸�"));
	m_List.SetItemText(3/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("ֱԲ��"));
	m_List.SetItemText(4/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("����"));
	m_List.SetItemText(5/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("������ĸ"));
	m_List.SetItemText(6/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("�ָߴ���"));
	m_List.SetItemText(7/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("�ָ���"));
	m_List.SetItemText(8/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("���"));
	m_List.SetItemText(9/*�д�0��ʼ��0Ϊ����*/, 0/*�д�0��ʼ��0Ϊ��һ��*/, _T("δ֪"));
	GetMenu()->EnableMenuItem(ID_ClearImage, MF_BYCOMMAND | MF_DISABLED|MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartProcess, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_PauseProcess, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//��ʼ��ĳЩ����
	CFont font_;
	bool success=font_.CreateFontA(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("Arial"));
	m_fps.SetFont(&font_,1);
	m_fps.SetWindowText(_T("FPS"));
	editfps.SetFont(&font_);
	statebar.SetFont(&font_);
	font_.DeleteObject();
	font.DeleteObject();
	
	
	InvalidateRect(NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
	
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
	//����ͼ��Ԥ���������ڴ�С�����ϵ�ת����
	imgsys.preprocess(m_hCam,pInfo,ZOOMRATIO);
	//���г�����Ϣ��ȡ�㷨
	imgsys.runAlgo();
	//Ϊ������ʾ���豸��ͼ����׼�� ��ͼƬ ״̬�� ֡�ʶ��� �����ı� 
	imgsys.showpre(m_Point);
	for (int i = 0; i < 10; i++)
	{
		CString tmps;
		tmps.Format("%d", imgsys.numsofeachtype[i]);
		m_List.SetItemText(i/*�д�0��ʼ��0Ϊ����*/, 1/*�д�0��ʼ��0Ϊ��һ��*/, (LPCTSTR)tmps);
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
	// TODO: �ڴ���������������
	TCHAR szFilter[] = _T("�ı��ļ�(*.BMP)|*.BMP|�����ļ�(*.*)|*.*||");
	// ������ļ��Ի���   
	CFileDialog fileDlg(TRUE, _T("BMP"), NULL, 0, szFilter, this);
	CString strFilePath;
	MVImage* simg=NULL;
	// ��ʾ���ļ��Ի���   
	if (IDOK == fileDlg.DoModal())
	{
		// ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
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
	// TODO: �ڴ���������������
	int nCams = 0;
	MVGetNumOfCameras(&nCams);
	if (nCams == 0)
	{
		MessageBox(_T("û���ҵ��������ȷ�����Ӻ����IP����"), _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	MVSTATUS_CODES r = MVOpenCamByIndex(0, &m_hCam);
	if (m_hCam == NULL)
	{
		if (r == MVST_ACCESS_DENIED)
			MessageBox(_T("�޷���������������ڱ�����������"), _T("��ʾ"), MB_ICONWARNING);
		else
			MessageBox(_T("�޷������"), _T("��ʾ"), MB_ICONWARNING);
		return;
	}
	GetMenu()->EnableMenuItem(ID_OpenExCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_StartGrab, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_CloseCam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	state = 2;
}


void CtoolrecognDlg::OnStartgrab()
{
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect IRect;
	CRect tmpRect;
	
	state = A;
	GetDlgItem(pic)->GetWindowRect(&IRect);
	if (((point.x >= IRect.left) && (point.x <= IRect.right)) 
		&& (point.y >= IRect.top && point.y <= IRect.bottom))//�ж�����������Ƿ���ͼ����
	{
		m_Point.x = point.x - IRect.left;
		m_Point.y = point.y - IRect.top;
	}
	m_Point.x = m_Point.x * imgsys.imgdatabase->getImageWidth(0)  / (IRect.Width());//�����������ת��Ϊ��������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	state = C;
	if(q.back()==nIDEvent)	//���������һ����ʱ����ʱ
		m_Point = { -1,-1 };//���㸴λ
	KillTimer(nIDEvent);	//ɾ����ʱ��
	q.pop();				//��������
	CDialog::OnTimer(nIDEvent);
}



void CtoolrecognDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

