// toolrecognDlg.h : header file
//

#pragma once
#include "..\..\include\MVGigE.h"
#include "..\..\include\MVImage.h"
#include <atlimage.h>
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"
#include "myimagemat.h"
#include "recognition.h"
#include "ImgSys.h"
#include<deque>
#include <queue>
#define ZOOMRATIO 2
#define FLOATINGMARGIN 10
#define FTSIDEL 10
// CtoolrecognDlg dialog
class CtoolrecognDlg : public CDialog
{
private:
	HANDLE m_hCam;
	CMenu m_Menu;
	CRITICAL_SECTION m_csInvalidate;
	ImgSys imgsys;
	CPoint m_Point;
	int count;
	CListCtrl m_List;
	HACCEL hAccel;
	std::queue<int> q;
	CBrush m_bkBrush;
	//CStatic* pStatic;
// Construction
public:
	CtoolrecognDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CROSS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OpenCamera();
	afx_msg void OnDestroy();
	int OnStreamCB( MV_IMAGE_INFO *pInfo);
//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_ImageWin;
	afx_msg void OnCbnSelchangeComboRotate();
	void DrawImage(CPoint);
	afx_msg void OnOpenimage();
	afx_msg void OnOpenexcam();
	afx_msg void OnStartgrab();
	afx_msg void OnClosecam();
	afx_msg void OnClearimage();
	CEdit statebar;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	double etrs;
	int state;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_thresh;
	CEdit editfps;
	CStatic m_fps;
};

