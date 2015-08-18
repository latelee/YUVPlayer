
// YUVPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YUVPlayerDlg.h"
#include "YuvTransform.h"
#include "afxdialogex.h"

#include "yuv2rgb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CYUVPlayerDlg::m_fPause = FALSE;
BOOL CYUVPlayerDlg::m_fEnd = FALSE;
BOOL CYUVPlayerDlg::m_fPlay = TRUE;

UINT Play(LPVOID pParam);

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
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
    RECT m_pRectLink;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CYUVPlayerDlg 对话框
BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(&m_pRectLink);
    ScreenToClient(&m_pRectLink);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CString strLink;

    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y < m_pRectLink.bottom && point.y > m_pRectLink.top)
    {
        if (nFlags == MK_LBUTTON)
        {
            GetDlgItem(IDC_STATIC_WEB)->GetWindowText(strLink);
            ShellExecute(NULL, NULL, strLink, NULL, NULL, SW_NORMAL);
        }
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}


void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y < m_pRectLink.bottom && point.y > m_pRectLink.top)
    {
        //变成手形
        HCURSOR hCursor;
        hCursor = ::LoadCursor (NULL, IDC_HAND);
        ::SetCursor(hCursor);
    }

    CDialogEx::OnMouseMove(nFlags, point);
}


CYUVPlayerDlg::CYUVPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYUVPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pSettingDlg = NULL;
    m_fInit = FALSE;

    // just in case
    m_nWidth = 176;
    m_nHeight = 144;
    m_nFps = 30;
    m_nYuvFormat = 0;
    m_fLoop = FALSE;

    m_pbYuvData = NULL;
    m_pbBmpData = NULL;
    m_pWinThread = NULL;

    m_nTotalFrame = m_nCurrentFrame = 0;
}

CYUVPlayerDlg::~CYUVPlayerDlg()
{
    if (m_pSettingDlg != NULL)
    {
        delete m_pSettingDlg;
        m_pSettingDlg = NULL;
    }

    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbBmpData != NULL)
    {
        delete[] m_pbBmpData;
        m_pbBmpData = NULL;
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
    DDX_Control(pDX, IDC_SLIDER1, m_slProgress);
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
    ON_COMMAND(ID_FILE_SAVEFRAME, &CYUVPlayerDlg::OnFileSave)
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
    ON_WM_DROPFILES()
    ON_COMMAND(ID_HELP_TRANSFORM, &CYUVPlayerDlg::OnHelpTransform)
    ON_WM_HSCROLL()
    ON_WM_TIMER()
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

	// 初始化 设置  对话框
    if (m_pSettingDlg == NULL)
    {
        m_pSettingDlg = new CSettingDlg();
        m_pSettingDlg->Create(IDD_DIALOG_SETTING, this);
        m_pSettingDlg->SetParentWnd(this);
        // 将子窗口的值传递给本窗口
        m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    }
    
    // 各类按钮ID
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

    CRect rect;
    for (int i = 0; i < 10; i++)
    {
        GetDlgItem(m_nStartX[0][i])->GetWindowRect(rect);
        ScreenToClient(rect);
        m_nStartX[1][i] = rect.left;
    }
    // 贴图
    m_bOpenFile.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_OPEN)));
    m_bSaveFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SAVE)));
    m_bSaveFrame.EnableWindow(FALSE);
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bPlay.EnableWindow(FALSE);
    m_bStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_STOP)));
    m_bStop.EnableWindow(FALSE);
    m_bPrevFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PREV)));
    m_bPrevFrame.EnableWindow(FALSE);
    m_bNextFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NEXT)));
    m_bNextFrame.EnableWindow(FALSE);
    m_bFirstFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_FIRST)));
    m_bFirstFrame.EnableWindow(FALSE);
    m_bLastFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_LAST)));
    m_bLastFrame.EnableWindow(FALSE);
    m_bSetting.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SETTING)));

    m_slProgress.EnableWindow(FALSE);

    m_fInit = TRUE;
    m_fShowBlack = TRUE;

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
    //picture控件背景色为黑色
    if (m_fShowBlack)
    {
        CRect rtTop;
        CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
        CDC *cDc = pWnd->GetDC();
        pWnd->GetClientRect(&rtTop);
        cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
        Invalidate(FALSE);
    }

    ShowPicture((BYTE *)m_pbBmpData, m_iBmpSize);

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
    OnBnClickedButtonOpen();
}


void CYUVPlayerDlg::OnFileSave()
{
    OnBnClickedButtonSave();
}


void CYUVPlayerDlg::OnFileExit()
{
    OnRExit();
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

/*
程序可自动通过文件名解析YUV格式及宽高，文件名称需要有YUV格式字符串及分辨率。YUV格式参考设置对话框除Y以外所有的格式。分辨率可用qcif、cif、720p、1080p或直接用宽x高，示例：foo_yuv422_176x144.yuv。
*/
void CYUVPlayerDlg::OnHelpHelp()
{
    wchar_t* help = _T("YUV播放器说明：\r\n1、支持常见YUV格式文件播放；\r\n2、支持保存YUV或BMP文件；\r\n3、直接拖拽文件到播放器或通过菜单打开文件；\r\n4、设置框中只有点击Apply或OK方可保存；\r\n\r\n\r\nTrick\r\n程序可自动通过文件名解析YUV格式及宽高，文件名称需要有YUV格式字符串及分辨率。YUV格式参考设置对话框除Y以外所有的格式。分辨率可用qcif、cif、720p、1080p或直接用宽x高，示例：foo_yuv422_176x144.yuv。");
    MessageBox((LPCTSTR)help);
}

void CYUVPlayerDlg::OnHelpTransform()
{
    CYuvTransform dlg;
    dlg.DoModal();
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
    OnBnClickedButtonOpen();
}


void CYUVPlayerDlg::OnRSetting()
{
    ShowSettingWindow();
}


void CYUVPlayerDlg::OnRExit()
{
    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbBmpData != NULL)
    {
        delete[] m_pbBmpData;
        m_pbBmpData = NULL;
    }
    OnCancel();
}

/////////////////////////////////////////// <------ 右键菜单事件

/////////////////////////////////////////// ------> 单击按钮事件
void CYUVPlayerDlg::OnBnClickedButtonOpen()
{
    KillTimer(1);
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_fPlay = TRUE;

    wchar_t szFilter[] = _T("YUV Files(*.yuv;*.raw)|*.yuv;*.raw|All Files(*.*)|*.*||");
    CFileDialog fileDlg(TRUE, _T("YUV"), NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Open YUV File");   // 标题
    if (fileDlg.DoModal() != IDOK)
        return;

    m_strPathName = fileDlg.GetPathName();

    // 显示标题
    CString strTemp;
    strTemp.Format(_T("%s-%s"), m_strPathName.GetBuffer(), APP_NAM);
    this->SetWindowText(strTemp);

    // 找文件名
    wchar_t* tmp = wcsrchr(m_strPathName.GetBuffer(), '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);

    m_pSettingDlg->ParseFilename(szFilename);
    m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    // 显示第一帧
    ShowOpenedFrame();

    return;
}

void CYUVPlayerDlg::OnBnClickedButtonSave()
{
    // 默认yuv
    wchar_t szFilter[128] = _T("YUV Files(*.%s)|*.%s|BMP(*.bmp)|*.bmp||");

    CFile cFile;
    CString strFile;
    wchar_t szFileName[128] = {0};
    wchar_t szExt[16] = {0};
    wchar_t* pExt = _T("yuv");
    char* pData = m_pbYuvData;
    UINT nSize = m_iYuvSize;

    int nType = 0;

    KillTimer(1);
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_fPlay = TRUE;

    // 找文件名
    _wsplitpath(m_strPathName, NULL, NULL, szFileName, szExt);
    if (wcscmp(szExt, _T("yuv")))
    {
        swprintf_s(szFilter, _T("YUV Files(*.%s)|*.%s|BMP(*.bmp)|*.bmp||"), &szExt[1], &szExt[1]);
        pExt = &szExt[1];
    }

    strFile.Format(_T("%s_%d.%s"), szFileName, m_nCurrentFrame, pExt);

    CFileDialog fileDlg(FALSE, _T("yuv"), strFile.GetBuffer(), OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Save Frame");
    if (fileDlg.DoModal() != IDOK)
        return;

    CString strTemp = fileDlg.GetFileExt();
    if (!strTemp.Compare(_T("bmp")))
    {
        pExt = _T("bmp");
        pData = m_pbBmpData;
        nSize = m_iBmpSize;
    }

    cFile.Open(fileDlg.GetPathName(), CFile::modeWrite|CFile::modeCreate);
    cFile.Write(pData, nSize);
    cFile.Close();
}


void CYUVPlayerDlg::OnBnClickedButtonPlay()
{
    m_bStop.EnableWindow(TRUE);

    if (m_fPlay)
    {
        SetTimer(1,1000/m_nFps,NULL);
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE)));
        m_bStop.EnableWindow(TRUE);
        m_fPlay = FALSE;
    }
    else
    {
        KillTimer(1);
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
        
        if (m_fPause == FALSE)
        {
            m_bStop.EnableWindow(FALSE);
            m_fPlay = TRUE;
        }
        if (m_fEnd == TRUE)
        {
            m_bStop.EnableWindow(FALSE);
            m_fPlay = TRUE;
        }
    }

    if (m_fPause)
    {
        SetTimer(1,1000/m_nFps,NULL);
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE)));
        m_bStop.EnableWindow(TRUE);
        m_fPause = FALSE;
    }
    if (m_fEnd)
    {
        SetTimer(1,1000/m_nFps,NULL);
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE)));
        m_bStop.EnableWindow(TRUE);
        m_nCurrentFrame = 1;
        m_fEnd = FALSE;
    }
}

void CYUVPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
    m_nCurrentFrame++;
    Read(m_nCurrentFrame);
    Show();
}

void CYUVPlayerDlg::OnBnClickedButtonStop()
{
    m_fPause = TRUE;

    KillTimer(1);
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bStop.EnableWindow(FALSE);
    OnBnClickedButtonFirst();
}


void CYUVPlayerDlg::OnBnClickedButtonPrev()
{
    KillTimer(1);
    m_fPlay = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_nCurrentFrame--;
    if (m_nCurrentFrame < 1)
    {
        m_nCurrentFrame = 1;
        return;
    }
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonNext()
{
    KillTimer(1);
    m_fPlay = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_nCurrentFrame++;

    if (m_nCurrentFrame >= m_nTotalFrame)
    {
        m_nCurrentFrame = m_nTotalFrame;

    }
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonFirst()
{
    m_nCurrentFrame = 1;
    this->Read(m_nCurrentFrame);
    this->Show();
}


void CYUVPlayerDlg::OnBnClickedButtonLast()
{
    KillTimer(1);
    m_fPlay = TRUE;
    m_fEnd = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_nCurrentFrame = m_nTotalFrame;
    this->Read(m_nCurrentFrame);
    this->Show();
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
    if (!m_fInit) return;
//    if (cx<=333 || cy <= 343) return;

    CDialogEx::OnSize(nType, cx, cy);

    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, cx, cy-26-20-21-4-4);
        pWnd->Invalidate();
        pWnd->UpdateData();
    }
    // 最右边的按钮
    pWnd = GetDlgItem(m_nStartX[0][10]);
    if (pWnd)
    {
       pWnd->SetWindowPos(NULL,cx-30-2,cy-26-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }

    int startx = 2;
    // 帧数统计
    pWnd = GetDlgItem(m_nStartX[0][0]);
    if (pWnd)
    {
        pWnd->SetWindowPos(NULL,startx+m_nStartX[1][0],cy-26-20-21-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
    }
    // 滚动条
    pWnd = GetDlgItem(m_nStartX[0][1]);
    if (pWnd)
    {
        pWnd->SetWindowPos( NULL,startx+m_nStartX[1][1],cy-26-26-4,cx,26,SWP_NOZORDER);
    }
    // 水平位置相同的按钮
    for (int i = 2; i < 10; i++)
    {
        pWnd = GetDlgItem(m_nStartX[0][i]);
        if (pWnd)
        {
            pWnd->SetWindowPos(NULL,startx+m_nStartX[1][i],cy-26-4,0,0,SWP_NOZORDER|SWP_NOSIZE);
        }
    }
}


void CYUVPlayerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialogEx::OnSizing(fwSide, pRect);
}

void CYUVPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    // note 用m_strPathName的话，关闭窗口后，进程还在
    CDialogEx::OnDropFiles(hDropInfo);
    wchar_t* pFilePathName =(wchar_t *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);
    m_strPathName.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);

    // 显示标题
    CString strTemp;
    strTemp.Format(_T("%s-%s"), pFilePathName, APP_NAM);
    this->SetWindowText(strTemp);

    // 找文件名
    wchar_t* tmp = wcsrchr(pFilePathName, '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    free(pFilePathName);

    m_pSettingDlg->ParseFilename(szFilename);
    m_pSettingDlg->SetParametersToParentWnd(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    // 显示第一帧
    ShowOpenedFrame();

#if 0
    save_yuv_file("rainbow_176x144_yuyv.yuv", 176, 144, FMT_YUYV);
    save_yuv_file("rainbow_176x144_yvyu.yuv", 176, 144, FMT_YVYU);
    save_yuv_file("rainbow_176x144_uyvy.yuv", 176, 144, FMT_UYVY);
    save_yuv_file("rainbow_176x144_vyuy.yuv", 176, 144, FMT_VYUY);
    save_yuv_file("rainbow_176x144_yuv444.yuv", 176, 144, FMT_YUV444);
#endif

    //change_yuv_file("suzie_qcif_yuv420p_00.yuv", "suzie_qcif_nv21.yuv", 176, 144, 0);
    //change_yuv_file("suzie_qcif_sp.yuv", "suzie_qcif_p.yuv", 176, 144, 1);
    //change_yuv_file("suzie_qcif_yuv422p.yuv", "suzie_qcif_nv61.yuv", 176, 144, 2);
}

/////////////////////////////
// 内部实现
void CYUVPlayerDlg::Open()
{
    // 打开文件
    if (CFile::hFileNull != m_cFile.m_hFile)
    {
        m_cFile.Close();
    }
    if (!m_cFile.Open(m_strPathName.GetBuffer(), CFile::modeRead))
    {
        MessageBox(_T("Open YUV file failed."));
        return;
    }
}

BOOL CYUVPlayerDlg::IsOpen()
{
    if (CFile::hFileNull != m_cFile.m_hFile)
        return TRUE;
    else
        return FALSE;
}

void CYUVPlayerDlg::Malloc()
{
    // 根据YUV格式分配内存
    switch (m_nYuvFormat)
    {
    case FMT_YUV420:
    case FMT_YV12:
    case FMT_NV12:
    case FMT_NV21:
        m_iYuvSize = m_nWidth * m_nHeight * 3 / 2;
        break;
    case FMT_YUV422:
    case FMT_YV16:
    case FMT_YUYV:
    case FMT_YVYU:
    case FMT_UYVY:
    case FMT_VYUY:
    case FMT_NV16:
    case FMT_NV61:
        m_iYuvSize = m_nWidth * m_nHeight * 2;
        break;
    case FMT_YUV444:
        m_iYuvSize = m_nWidth * m_nHeight * 3;
        break;
    case FMT_Y:
        m_iYuvSize = m_nWidth * m_nHeight;
        break;
    default:
        break;
    }
    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    // 此处有问题，更改YUV格式后会挂掉
    if (m_pbBmpData != NULL)
    {
        delete[] m_pbBmpData;
        m_pbBmpData = NULL;
    }

    m_iBmpSize = m_nWidth * m_nHeight * 3 + 54; // 这里申请BMP图片的空间，方便保存

    m_pbYuvData = new char[m_iYuvSize];
    m_pbBmpData  = new char[m_iBmpSize];

    m_nTotalFrame = (UINT)(m_cFile.GetLength() / m_iYuvSize); // 计算总帧数
    m_slProgress.SetRange(1, m_nTotalFrame);
    m_slProgress.SetPos(m_nCurrentFrame);

    this->ShowFrameCount(m_nCurrentFrame);
}

void CYUVPlayerDlg::Read(INT nCurrentFrame)
{
    // 防止越界
    if (m_nCurrentFrame > m_nTotalFrame)
    {
        if (m_fLoop)
        {
            m_nCurrentFrame = 1;
        }
        else
        {
            m_fEnd = TRUE;
            m_bStop.EnableWindow(FALSE);
            m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
            KillTimer(1);
            return;
        }
    }
    this->ShowFrameCount(nCurrentFrame); // 读一帧时，顺便显示出当前帧
    m_cFile.Seek(m_iYuvSize * (nCurrentFrame - 1), SEEK_SET);
    m_cFile.Read(m_pbYuvData, m_iYuvSize);
}

void CYUVPlayerDlg::Show()
{
    // 先添加BMP头
    m_bmHeader.bfType = 'MB';
    m_bmHeader.bfSize = m_iBmpSize;// + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    m_bmHeader.bfReserved1 = 0;
    m_bmHeader.bfReserved2 = 0;
    m_bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    m_bmInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
    m_bmInfo.bmiHeader.biWidth  = m_nWidth;
    // note BMP图像是倒过来的
    m_bmInfo.bmiHeader.biHeight = -m_nHeight;
    m_bmInfo.bmiHeader.biPlanes = 1;
    m_bmInfo.bmiHeader.biBitCount = 24;
    m_bmInfo.bmiHeader.biCompression = BI_RGB;
    m_bmInfo.bmiHeader.biSizeImage   = m_iBmpSize - 54;
    m_bmInfo.bmiHeader.biXPelsPerMeter = 0;
    m_bmInfo.bmiHeader.biYPelsPerMeter = 0;
    m_bmInfo.bmiHeader.biClrUsed = 0;
    m_bmInfo.bmiHeader.biClrImportant = 0;

    memcpy(m_pbBmpData, &m_bmHeader, sizeof(BITMAPFILEHEADER));
    memcpy(m_pbBmpData+sizeof(BITMAPFILEHEADER), &m_bmInfo, sizeof(BITMAPINFOHEADER));

    // 再转换格式
    yuv_to_rgb24((YUV_TYPE)m_nYuvFormat, (unsigned char *)m_pbYuvData, (unsigned char *)m_pbBmpData+54, m_nWidth, m_nHeight);
    
    // BMP是BGR格式的，要转换 rgb->bgr
    swaprgb((BYTE*)(m_pbBmpData+54), m_iBmpSize-54);
    // 显示
   ShowPicture((BYTE *)m_pbBmpData, m_iBmpSize);
}

// 显示当前帧/总帧数
void CYUVPlayerDlg::ShowFrameCount(int nCurrentFrame)
{
    CString strTemp;
    strTemp.Format(_T("%d/%d"), nCurrentFrame, m_nTotalFrame);
    GetDlgItem(IDC_STATIC_FRAMECNT)->SetWindowText(strTemp);
    m_slProgress.SetPos(nCurrentFrame);
}

// 打开文件 时显示第一帧
void CYUVPlayerDlg::ShowOpenedFrame()
{
    m_nCurrentFrame = 1;

    this->Open();
    this->Malloc();
    
    this->Read(m_nCurrentFrame);
    this->Show();

    m_bSaveFrame.EnableWindow(TRUE);
    m_bPlay.EnableWindow(TRUE);
    m_bStop.EnableWindow(FALSE);
    m_bPrevFrame.EnableWindow(TRUE);
    m_bNextFrame.EnableWindow(TRUE);
    m_bFirstFrame.EnableWindow(TRUE);
    m_bLastFrame.EnableWindow(TRUE);

    m_slProgress.EnableWindow(TRUE);
}

inline void RenderBitmap(CWnd *pWnd, Bitmap* pbmp)
{
    RECT rect;
    pWnd->GetClientRect( &rect );

    Graphics grf( pWnd->m_hWnd );
    if ( grf.GetLastStatus() == Ok )
    {
        Rect rc( rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top );

        grf.DrawImage(pbmp, rc);
    }
}

// 显示图片
void CYUVPlayerDlg::ShowPicture(BYTE* pbData, int iSize)
{
    if (pbData == NULL) return;
    CWnd* pWnd=GetDlgItem(IDC_VIDEO);   // IDC_VIDEO：picture contral 控件ID
    IStream* pPicture = NULL;
    CreateStreamOnHGlobal(NULL,TRUE,&pPicture);
    if( pPicture != NULL )
    {
        pPicture->Write(pbData,  iSize, NULL);
        LARGE_INTEGER liTemp = { 0 };
        pPicture->Seek(liTemp, STREAM_SEEK_SET, NULL);
        Bitmap TempBmp(pPicture);
        RenderBitmap(pWnd, &TempBmp);
    }
    if(pPicture != NULL)
    {
        pPicture->Release();
        pPicture = NULL;
    }

    m_fShowBlack = FALSE;
}

void CYUVPlayerDlg::SetParentParameters(int width, int height, int fps, int fmt, BOOL loop)
{
    m_nWidth = width;
    m_nHeight = height;
    m_nFps = fps;
    m_nYuvFormat = fmt;
    m_fLoop = loop;

    // 恢复原始状态
    if (!IsOpen()) return;

    ShowOpenedFrame();
}

// 滚动条处理函数
void CYUVPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    //CSliderCtrl  *pSlidCtrl=(CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
    m_nCurrentFrame = m_slProgress.GetPos();
    this->Read(m_nCurrentFrame);
    this->Show();
    //this->ShowFrameCount(m_nCurrentFrame);

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
