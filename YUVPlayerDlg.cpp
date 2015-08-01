
// YUVPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YUVPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CYUVPlayerDlg 对话框




CYUVPlayerDlg::CYUVPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYUVPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pSettingDlg = NULL;
    m_fInit = FALSE;
}

CYUVPlayerDlg::~CYUVPlayerDlg()
{
    if (m_pSettingDlg != NULL)
    {
        delete m_pSettingDlg;
        m_pSettingDlg = NULL;
    }
}

void CYUVPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_FIRST, m_bFirstFrame);
    DDX_Control(pDX, IDC_BUTTON_LAST, m_bLastFrame);
    DDX_Control(pDX, IDC_BUTTON_NEXT, m_bNextFrame);
    DDX_Control(pDX, IDC_BUTTON_OPEN, m_bOpenFile);
    DDX_Control(pDX, IDC_BUTTON_PLAY, m_bPlay);
    DDX_Control(pDX, IDC_BUTTON_PREV, m_bPrevFrame);
    DDX_Control(pDX, IDC_BUTTON_SAVE, m_bSaveFrame);
    DDX_Control(pDX, IDC_BUTTON_SET, m_bSetting);
    DDX_Control(pDX, IDC_BUTTON_STOP, m_bStop);
}

BEGIN_MESSAGE_MAP(CYUVPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BUTTON_SET, &CYUVPlayerDlg::OnBnClickedButtonSet)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CYUVPlayerDlg::OnBnClickedButtonOpen)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CYUVPlayerDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_PLAY, &CYUVPlayerDlg::OnBnClickedButtonPlay)
    ON_WM_NCHITTEST()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_FILE_OPEN, &CYUVPlayerDlg::OnFileOpen)
    ON_COMMAND(ID_FILE_CLOSE, &CYUVPlayerDlg::OnFileClose)
    ON_COMMAND(ID_FILE_SAVE, &CYUVPlayerDlg::OnFileSave)
    ON_COMMAND(ID_FILE_EXIT, &CYUVPlayerDlg::OnFileExit)
    ON_COMMAND(ID_PLAYING_PLAY, &CYUVPlayerDlg::OnPlayingPlay)
    ON_COMMAND(ID_PLAYING_SETTING, &CYUVPlayerDlg::OnPlayingSetting)
    ON_COMMAND(ID_PLAYING_STOP, &CYUVPlayerDlg::OnPlayingStop)
    ON_COMMAND(ID_HELP_HELP, &CYUVPlayerDlg::OnHelpHelp)
    ON_COMMAND(ID_HELP_ABOUT, &CYUVPlayerDlg::OnHelpAbout)
    ON_COMMAND(ID_R_OPEN, &CYUVPlayerDlg::OnROpen)
    ON_COMMAND(ID_R_SETTING, &CYUVPlayerDlg::OnRSetting)
    ON_COMMAND(ID_R_EXIT, &CYUVPlayerDlg::OnRExit)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CYUVPlayerDlg::OnBnClickedButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CYUVPlayerDlg::OnBnClickedButtonPrev)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &CYUVPlayerDlg::OnBnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_FIRST, &CYUVPlayerDlg::OnBnClickedButtonFirst)
    ON_BN_CLICKED(IDC_BUTTON_LAST, &CYUVPlayerDlg::OnBnClickedButtonLast)
    ON_WM_SIZE()
    ON_WM_SIZING()
END_MESSAGE_MAP()

// CYUVPlayerDlg 消息处理程序

BOOL CYUVPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	//ShowWindow(SW_MINIMIZE); // 不要最小化窗口

	// TODO: 在此添加额外的初始化代码
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
        m_pSettingDlg->SetParentWnd(this);
        //m_pSettingDlg->CenterWindow(this);
    }
    m_nStartX[0][0] = IDC_STATIC_FRAMECNT;
    m_nStartX[0][1] = IDC_SLIDER1;
    m_nStartX[0][2] = IDC_BUTTON_OPEN;
    m_nStartX[0][3] = IDC_BUTTON_SAVE;
    m_nStartX[0][4] = IDC_BUTTON_PLAY;
    m_nStartX[0][5] = IDC_BUTTON_STOP;
    m_nStartX[0][6] = IDC_BUTTON_PREV;
    m_nStartX[0][7] = IDC_BUTTON_NEXT;
    m_nStartX[0][8] = IDC_BUTTON_FIRST;
    m_nStartX[0][9] = IDC_BUTTON_LAST;
    m_nStartX[0][10] = IDC_BUTTON_SET;

#if 01
    CRect rect;

    for (int i = 0; i < 10; i++)
    {
        GetDlgItem(m_nStartX[0][i])->GetWindowRect(rect);
        ScreenToClient(rect);
        m_nStartX[1][i] = rect.left;
    }
#endif

    m_bOpenFile.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_OPEN)));
    m_bSaveFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SAVE)));
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_STOP)));
    m_bPrevFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PREV)));
    m_bNextFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NEXT)));
    m_bFirstFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_FIRST)));
    m_bLastFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_LAST)));
    m_bSetting.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SETTING)));


    m_fInit = TRUE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CYUVPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYUVPlayerDlg::OnPaint()
{
#if 01
    //picture控件背景色为黑色
    CRect rtTop;
    CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
    CDC *cDc = pWnd->GetDC();
    pWnd->GetClientRect(&rtTop);
    cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
    Invalidate(FALSE);
#endif

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CYUVPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CYUVPlayerDlg::ShowSettingWindow()
{
    // 非模态对话框
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
    }

    m_pSettingDlg->ShowWindow(SW_SHOW);
    //m_pSettingDlg->CenterWindow(this);
    //m_pParentWnd->CenterWindow(this);

    /*
    CWnd *pWnd = GetDlgItem(IDD_DIALOG_SETTING);
    if (pWnd)
    {
        pWnd->SetWindowPos( NULL,4,10,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    */
}

// -------------------------------------------------------

// 实现非标题也可用鼠标拖动
LRESULT CYUVPlayerDlg::OnNcHitTest(CPoint point)
{
     UINT nHitTest = CDialogEx::OnNcHitTest(point);
     if ((nHitTest == HTCLIENT) && (::GetAsyncKeyState (MK_LBUTTON) < 0))
         nHitTest = HTCAPTION;
     return nHitTest;
    //return CDialogEx::OnNcHitTest(point); // not this....
}

// 右键菜单
void CYUVPlayerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    // TODO: Add your message handler code here
    CMenu popMenu;
    CMenu *pPopup;
    popMenu.LoadMenu(IDR_MENU2);

    pPopup=popMenu.GetSubMenu(0);

    pPopup->EnableMenuItem(ID_R_OPEN, MF_BYCOMMAND|MF_ENABLED);
    pPopup->EnableMenuItem(ID_R_SETTING, MF_BYCOMMAND|MF_ENABLED);
    pPopup->EnableMenuItem(ID_R_EXIT, MF_BYCOMMAND|MF_ENABLED);

    //pPopup->EnableMenuItem(ID_R_SETTING,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
    pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
    pPopup->Detach();
    popMenu.DestroyMenu();
}

/////////////////////////////////////////// ------> 标题菜单事件
void CYUVPlayerDlg::OnFileOpen()
{

}


void CYUVPlayerDlg::OnFileClose()
{

}


void CYUVPlayerDlg::OnFileSave()
{

}


void CYUVPlayerDlg::OnFileExit()
{

}


void CYUVPlayerDlg::OnPlayingPlay()
{

}


void CYUVPlayerDlg::OnPlayingSetting()
{
    ShowSettingWindow();
}


void CYUVPlayerDlg::OnPlayingStop()
{

}


void CYUVPlayerDlg::OnHelpHelp()
{

    MessageBox(_T("Help me"));
}


void CYUVPlayerDlg::OnHelpAbout()
{

    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
/////////////////////////////////////////// <------ 标题菜单事件

/////////////////////////////////////////// ------> 右键菜单事件
void CYUVPlayerDlg::OnROpen()
{

}


void CYUVPlayerDlg::OnRSetting()
{
    ShowSettingWindow();
}


void CYUVPlayerDlg::OnRExit()
{

}

/////////////////////////////////////////// <------ 右键菜单事件

/////////////////////////////////////////// ------> 单击按钮事件
void CYUVPlayerDlg::OnBnClickedButtonOpen()
{


    int aa =  m_nWidth;

    return;
}


void CYUVPlayerDlg::OnBnClickedButtonSave()
{

}


void CYUVPlayerDlg::OnBnClickedButtonPlay()
{
    static int play = 0;
    if (!play)
    {
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE)));
        play = 1;
    }
    else
    {
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
        play = 0;
    }
}


void CYUVPlayerDlg::OnBnClickedButtonStop()
{

}


void CYUVPlayerDlg::OnBnClickedButtonPrev()
{

}


void CYUVPlayerDlg::OnBnClickedButtonNext()
{

}


void CYUVPlayerDlg::OnBnClickedButtonFirst()
{

}


void CYUVPlayerDlg::OnBnClickedButtonLast()
{

}


void CYUVPlayerDlg::OnBnClickedButtonSet()
{
    ShowSettingWindow();
}

/////////////////////////////////////////// <------ 单击按钮事件

/////////////////////////////////////////// ------> 键盘快捷键事件



// 窗口缩放
// todo:只能宽、高同时缩放
void CYUVPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (!m_fInit) return;
    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, cx, cy-26-20-21);
        pWnd->Invalidate();
        pWnd->UpdateData();
    }
    // 最右边的按钮
    pWnd = GetDlgItem(m_nStartX[0][10]);
    if (pWnd)
    {
       pWnd->SetWindowPos(NULL,cx-30,cy-26,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }

    int startx = 2;
    // 帧数统计
    pWnd = GetDlgItem(m_nStartX[0][0]);
    if (pWnd)
    {
        pWnd->SetWindowPos(NULL,startx+m_nStartX[1][0],cy-26-20-21,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    // 滚动条
    pWnd = GetDlgItem(m_nStartX[0][1]);
    if (pWnd)
    {
        //pWnd->MoveWindow(startx+0, cy-26-26, cx, cy-26-20-21);
        //pWnd->Invalidate();
        pWnd->SetWindowPos( NULL,startx+m_nStartX[1][1],cy-26-26,cx,26,SWP_NOZORDER);
    }
    // 水平位置相同的按钮
    for (int i = 2; i < 10; i++)
    {
        pWnd = GetDlgItem(m_nStartX[0][i]);
        if (pWnd)
        {
            pWnd->SetWindowPos(NULL,startx+m_nStartX[1][i],cy-26,0,0,SWP_NOZORDER|SWP_NOSIZE);
        }
    }
}


void CYUVPlayerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialogEx::OnSizing(fwSide, pRect);

#if 0
    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, pRect->right - pRect->left, pRect->bottom - pRect->top);
        pWnd->Invalidate(FALSE);
        pWnd->UpdateData();
    }
#endif
}
