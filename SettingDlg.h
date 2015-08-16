#pragma once

extern void find_resolution(char* filename, int& fmt_idx, int& width, int& height);

class CYUVPlayerDlg;

// CSettingDlg 对话框

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTING };

public:
    void SetParentWnd(CYUVPlayerDlg* pWnd) {m_pParentWnd = pWnd;}
    void SetParametersToParentWnd(int& width, int& height, int& fps, int& fmt, BOOL& loop)
    {
        width = m_nWidth;
        height = m_nHeight;
        fps = m_nFps;
        fmt = m_nYuvFormat;
        loop = m_fLoop;
    }

    void SetRegistration(CString& strSize, int width, int height, int fpsidx, int fmt, int loop);
    void GetRegistration(CString& strSize, int& width, int& height, int& fpsidx, int& fmt, int& loop);
    BOOL ExistRegistration();

    void ParseFilename(const char* pFilename);

    void UpdateRes();

private:
    CYUVPlayerDlg *m_pParentWnd; // 窗口参数传递
    CString m_strAddedSize;   // 用户自定义的分辨率(含系统已有的)
    int m_nFpsIndex;
    CStringArray m_strArrAddedSize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedApply();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    CComboBox m_cbFps;
    CComboBox m_cbYuvFormat;
    CComboBox m_cbResolution;
    int m_nHeight;
    int m_nWidth;
    int m_nFps;
    int m_nYuvFormat;
    BOOL m_fLoop;
    afx_msg void OnSelchangeCbSize();
    afx_msg void OnSelchangeCbYuvFmt();
    afx_msg void OnSelchangeCbFps();
    afx_msg void OnClickedCkLoop();
    afx_msg void OnChangeEWidth();
    afx_msg void OnChangeEHeight();
    afx_msg void OnBnClickedBtAdd();
    afx_msg void OnBnClickedBtDel();
};
