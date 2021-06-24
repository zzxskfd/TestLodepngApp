
// MihoyoTestAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MihoyoTestApp.h"
#include "MihoyoTestAppDlg.h"
#include "afxdialogex.h"
#include "MihoyoTestLodepng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CMihoyoTestAppDlg dialog



CMihoyoTestAppDlg::CMihoyoTestAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MIHOYOTESTAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMihoyoTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMihoyoTestAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CMihoyoTestAppDlg::OnEnChangeMfceditbrowse1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMihoyoTestAppDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE2, &CMihoyoTestAppDlg::OnEnChangeMfceditbrowse2)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE2, &CMihoyoTestAppDlg::OnEnChangeMfceditbrowse2)
//	ON_EN_CHANGE(IDC_EDIT3, &CMihoyoTestAppDlg::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CMihoyoTestAppDlg message handlers

BOOL CMihoyoTestAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMihoyoTestAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMihoyoTestAppDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMihoyoTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


std::filesystem::path input_folder;
std::filesystem::path output_folder;

void CMihoyoTestAppDlg::OnEnChangeMfceditbrowse1()
{
	CString selectedPath;
	GetDlgItemText(IDC_MFCEDITBROWSE1, selectedPath);
	input_folder = selectedPath.GetString();
}

void CMihoyoTestAppDlg::OnEnChangeMfceditbrowse2()
{
	CString selectedPath;
	GetDlgItemText(IDC_MFCEDITBROWSE2, selectedPath);
	output_folder = selectedPath.GetString();
}

void CMihoyoTestAppDlg::OnBnClickedButton2()
{
	bool ok_flag = true;
	std::wstring wstr;
	CWnd* messageWindow = GetDlgItem(IDC_EDIT3);
	if (!std::filesystem::exists(input_folder)) {
		wstr += L"Input folder does not exist!\r\n";
		ok_flag = false;
	}
	if (!std::filesystem::is_directory(output_folder) &&
		!std::filesystem::create_directories(output_folder)) {
		wstr += L"Output folder is not valid!\r\n";
		ok_flag = false;
	}
	messageWindow->SetWindowTextW(wstr.c_str());
	if (ok_flag) {
		CProgressCtrl* progressBar = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS2);
		run_flip_png(input_folder, output_folder, messageWindow, progressBar);
	}
}


