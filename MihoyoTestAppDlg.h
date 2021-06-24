
// MihoyoTestAppDlg.h : header file
//

#pragma once


// CMihoyoTestAppDlg dialog
class CMihoyoTestAppDlg : public CDialogEx
{
// Construction
public:
	CMihoyoTestAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MIHOYOTESTAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeMfceditbrowse1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeMfceditbrowse2();
//	afx_msg void OnEnChangeEdit3();
};
