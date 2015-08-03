
// YUVPlayerDlg.h : 头文件
//

#pragma once

#include "SettingDlg.h"

#define APP_NAM _T("YUV Player")



#define MAX_URL_LENGTH 256
typedef struct yuv_pic_tag
{
    char* yuv_data[4];
    int   yuv_linesize[4]; //number of bytes per line
} yuv_pic;

// CYUVPlayerDlg 对话框
class CYUVPlayerDlg : public CDialogEx
{
// 构造
public:
	CYUVPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CYUVPlayerDlg();
// 对话框数据
	enum { IDD = IDD_YUVPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
    BOOL m_fInit;
    BOOL m_fShowBlack;

    CSettingDlg* m_pSettingDlg;
    int m_nWidth;
    int m_nHeight;
    int m_nFps;
    int m_nYuvFormat;
    BOOL m_fLoop;

    int m_nStartX[2][11];   // 窗口拖动所需位置

    CString m_strPathName;  // 文件名称(包括路径)
    char*   m_pbYuvData;    // YUV数据
    char*   m_pbRgbData;    // RGB数据
    UINT    m_iYuvSize; // 一幅图像大小
    UINT    m_iRgbSize; //

    BITMAPFILEHEADER m_bmHeader;
    BITMAPINFO       m_bmInfo;

    CFile m_cFile;
    CWinThread* m_pWinThread;

    // 共用内部函数
    void ShowOpenedFrame();
    void ShowPicture(BYTE* pbData, int iSize);

public:
    void GetParameters(int& width, int height, int fps, int fmt, BOOL loop)
    {
        m_nWidth = width;
        m_nHeight = height;
        m_nFps = fps;
        m_nYuvFormat = fmt;
        m_fLoop = loop;
    }

    void ShowSettingWindow();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSet();
    afx_msg void OnBnClickedButtonOpen();
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonPlay();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

    afx_msg void OnFileOpen();
    afx_msg void OnFileClose();
    afx_msg void OnFileSave();
    afx_msg void OnFileExit();
    afx_msg void OnPlayingPlay();
    afx_msg void OnPlayingSetting();
    afx_msg void OnPlayingStop();
    afx_msg void OnHelpHelp();
    afx_msg void OnHelpAbout();
    afx_msg void OnROpen();
    afx_msg void OnRSetting();
    afx_msg void OnRExit();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonPrev();
    afx_msg void OnBnClickedButtonNext();
    afx_msg void OnBnClickedButtonFirst();
    afx_msg void OnBnClickedButtonLast();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnStnClickedSTest();
    CButton m_bFirstFrame;
    CButton m_bLastFrame;
    CButton m_bNextFrame;
    CButton m_bOpenFile;
    CButton m_bPlay;
    CButton m_bPrevFrame;
    CButton m_bSaveFrame;
    CButton m_bSetting;
    CButton m_bStop;
    afx_msg void OnDropFiles(HDROP hDropInfo);
};
