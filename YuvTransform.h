#pragma once
#include "afxwin.h"


// CYuvTransform dialog

class CYuvTransform : public CDialogEx
{
	DECLARE_DYNAMIC(CYuvTransform)

public:
	CYuvTransform(CWnd* pParent = NULL);   // standard constructor
	virtual ~CYuvTransform();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRANSFORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
    int     m_nYuvFormat;
    int     m_nOutputFormat;
    char*   m_pbYuvData;    // YUV数据
    char*   m_pbOutputData;    // RGB数据
    UINT    m_nYuvSize; // 一幅图像大小
    UINT    m_nOutputSize; // BMP图的大小
    INT     m_nTotalFrame;
    INT     m_nCurrentFrame;

    CFile   m_cFile;
    CFile   m_cOutputFile;

    CString m_strOutputFile;
    CString m_strFileTittle;
    CString m_strFileExtern;

public:
    void ParseFilename(const char* pFilename);
    INT Open(wchar_t* pFile);
    BOOL IsOpen();
    INT Malloc();
    void Read(INT nCurrentFrame);
    INT Transform();
    INT Write(INT nCurrentFrame = -1);
    

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBOpen();
    afx_msg void OnBnClickedBCombine();
    afx_msg void OnBnClickedBSplit();
    afx_msg void OnBnClickedBTransform();
    CString m_strPathName;
    afx_msg void OnDropFiles(HDROP hDropInfo);
    UINT m_nWidth;
    UINT m_nHeight;
    CComboBox m_cbInput;
    CComboBox m_cbOutput;
    afx_msg void OnBnClickedOk();
    CStatic m_csInfo;
};
