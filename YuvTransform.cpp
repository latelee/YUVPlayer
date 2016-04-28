// YuvTransform.cpp : implementation file
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "YuvTransform.h"
#include "afxdialogex.h"

#include "SettingDlg.h"

#include "yuv2rgb.h"

#include <direct.h>
#include <io.h>
#include <windows.h>


#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 256
#endif
// CYuvTransform dialog

IMPLEMENT_DYNAMIC(CYuvTransform, CDialogEx)

CYuvTransform::CYuvTransform(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYuvTransform::IDD, pParent)
    , m_strPathName(_T(""))
    , m_nWidth(0)
    , m_nHeight(0)
{
    m_pbYuvData = NULL;
    m_pbOutputData = NULL;
    m_nYuvFormat = -1;
    m_nOutputFormat = -1;
    m_nCurrentFrame = 0;
    m_nTotalFrame = 0;
}

CYuvTransform::~CYuvTransform()
{
    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbOutputData != NULL)
    {
        delete[] m_pbOutputData;
        m_pbOutputData = NULL;
    }
}

void CYuvTransform::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_E_INPUT, m_strPathName);
    DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    DDX_Text(pDX, IDC_E_HEIGHT, m_nHeight);
    DDX_Control(pDX, IDC_CB_INFMT, m_cbInput);
    DDX_Control(pDX, IDC_CB_OUTFMT, m_cbOutput);
    DDX_Control(pDX, IDC_S_PROMAT, m_csInfo);
}


BEGIN_MESSAGE_MAP(CYuvTransform, CDialogEx)
    ON_BN_CLICKED(IDC_B_OPEN, &CYuvTransform::OnBnClickedBOpen)
    ON_BN_CLICKED(IDC_B_COMBINE, &CYuvTransform::OnBnClickedBCombine)
    ON_BN_CLICKED(IDC_B_SPLIT, &CYuvTransform::OnBnClickedBSplit)
    ON_BN_CLICKED(IDC_B_TRANSFORM, &CYuvTransform::OnBnClickedBTransform)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDOK, &CYuvTransform::OnBnClickedOk)
END_MESSAGE_MAP()


void CYuvTransform::OnBnClickedBOpen()
{
    wchar_t szFilter[] = _T("YUV Files(*.yuv;*.raw)|*.yuv;*.raw|All Files(*.*)|*.*||");
    CFileDialog fileDlg(TRUE, _T("YUV"), NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Open YUV File");   // 标题
    if (fileDlg.DoModal() != IDOK)
        return;

    m_strPathName = fileDlg.GetPathName();
    m_strFileTittle = fileDlg.GetFileTitle();
    m_strFileExtern = fileDlg.GetFileExt();

    // 找文件名
    wchar_t* tmp = wcsrchr(m_strPathName.GetBuffer(), '\\');
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    ParseFilename(szFilename);

    UpdateData(FALSE);
}

// 查找目录的yuv文件，不支持其它后缀名
void CYuvTransform::OnBnClickedBCombine()
{
    UpdateData();

    m_nYuvFormat = m_cbInput.GetCurSel();

    if (m_strPathName.IsEmpty() || m_nWidth == 0 || m_nHeight == 0 || m_nYuvFormat == -1)
    {
        MessageBox(_T("Sorry, can not do it.Maybe you forgot sth."));
        return;
    }
    //CreateDirectory(_T("./out"), NULL);

    wchar_t szDir[256] = {0};
    _wsplitpath(m_strPathName.GetBuffer(), NULL, szDir, NULL, NULL);

    m_strPathName = szDir;

    CString strTemp;
    strTemp.Format(_T("Open %s ok."), m_strPathName);
    m_csInfo.SetWindowText(strTemp);

    if (Malloc() < 0) return;

    struct _wfinddata_t cfile;    // cfile.name cfile.size
    intptr_t handle;
    intptr_t handle_next;
    int ret = 0;

    ret = _wchdir(m_strPathName.GetBuffer());
    if (ret < 0)
    {
        strTemp.Format(_T("Dir %s not exist."), m_strPathName);
        m_csInfo.SetWindowText(strTemp);
        return;
    }
    handle = _wfindfirst(_T("*.yuv"), &cfile);    // 查找.yuv
    if (handle < 0)
    {
        strTemp.Format(_T("Dir %s not exist."), m_strPathName);
        m_csInfo.SetWindowText(strTemp);
        return;
    }
    handle_next = handle;
    int first = 1;
    while (handle_next != -1)
    {
        if (first)
        {
            first = 0;

            wchar_t szExt[16] = {0};
            wchar_t szFilename1[256] = {0};
            _wsplitpath(cfile.name, NULL, NULL, szFilename1, szExt); // 要单独开辟空间存储，否则窗口关闭后有非法内存使用

            m_strFileTittle = szFilename1;
            m_strFileExtern = &szExt[1];
            
            m_strOutputFile.Format(_T("%s%s_all.%s"), szDir, m_strFileTittle, m_strFileExtern);
            m_cOutputFile.Open(m_strOutputFile.GetBuffer(), CFile::modeWrite|CFile::modeCreate);
        }
        if (Open(cfile.name) < 0) return;

        Read(m_nCurrentFrame);

        m_cOutputFile.Write(m_pbYuvData, m_nYuvSize);
        strTemp.Format(_T("combining %s."), cfile.name);
        m_csInfo.SetWindowText(strTemp);
        handle_next = _wfindnext(handle, &cfile);    // 查找下一文件
    }

    _findclose(handle);

    strTemp.Format(_T("Combine finish."));
    m_csInfo.SetWindowText(strTemp);

    m_cOutputFile.Close();
}


void CYuvTransform::OnBnClickedBSplit()
{
    UpdateData();

    m_nYuvFormat = m_cbInput.GetCurSel();

    if (m_strPathName.IsEmpty() || m_nWidth == 0 || m_nHeight == 0 || m_nYuvFormat == -1)
    {
        MessageBox(_T("Sorry, can not do it.Maybe you forgot sth."));
        return;
    }
    CreateDirectory(_T("./out"), NULL);

    if (Open(m_strPathName.GetBuffer()) < 0) return;

    CString strTemp;
    strTemp.Format(_T("Open %s ok."), m_strPathName);
    m_csInfo.SetWindowText(strTemp);

    if (Malloc() < 0) return;


    wchar_t extp[32] = {0};
    m_cbOutput.GetWindowText((LPTSTR)extp, 32);
    strTemp = extp;
    strTemp.MakeLower();
    
    while (m_nCurrentFrame < m_nTotalFrame)
    {
        if (m_nTotalFrame > 10000)
            m_strOutputFile.Format(_T("./out/%s_%08d.%s"), m_strFileTittle, m_nCurrentFrame, m_strFileExtern);
        else
            m_strOutputFile.Format(_T("./out/%s_%06d.%s"), m_strFileTittle, m_nCurrentFrame, m_strFileExtern);
        m_cOutputFile.Open(m_strOutputFile.GetBuffer(), CFile::modeWrite|CFile::modeCreate);

        Read(m_nCurrentFrame);
        m_cOutputFile.Write(m_pbYuvData, m_nYuvSize);
        m_cOutputFile.Close();
        
        strTemp.Format(_T("split to %s finish."), m_strOutputFile);
        m_csInfo.SetWindowText(strTemp);

        m_nCurrentFrame++;
    }

    strTemp.Format(_T("Split file finish."));
    m_csInfo.SetWindowText(strTemp);
}

// 格式转换，针对单一文件
void CYuvTransform::OnBnClickedBTransform()
{
    UpdateData();

    m_nYuvFormat = m_cbInput.GetCurSel();
    m_nOutputFormat = m_cbInput.GetCurSel();

    if (m_strPathName.IsEmpty() || m_nWidth == 0 || m_nHeight == 0 || m_nYuvFormat == -1 || m_nOutputFormat == -1)
    {
        MessageBox(_T("Sorry, can not do it.Maybe you forgot sth."));
        return;
    }
    CreateDirectory(_T("./out"), NULL);

    if (Open(m_strPathName.GetBuffer()) < 0) return;

    CString strTemp;
    strTemp.Format(_T("Open %s ok."), m_strPathName);
    m_csInfo.SetWindowText(strTemp);

    if (Malloc() < 0) return;

    wchar_t extp[32] = {0};
    m_cbOutput.GetWindowText((LPTSTR)extp, 32);
    strTemp = extp;
    strTemp.MakeLower();
    m_strOutputFile.Format(_T("./out/%s_%s.%s"), m_strFileTittle, strTemp.GetBuffer(), m_strFileExtern);
    m_cOutputFile.Open(m_strOutputFile.GetBuffer(), CFile::modeWrite|CFile::modeCreate);
    while (m_nCurrentFrame < m_nTotalFrame)
    {
        Read(m_nCurrentFrame);

        if (Transform() < 0)
        {
            m_cOutputFile.Close();
            DeleteFile(m_strOutputFile.GetBuffer());
            strTemp.Format(_T("Transform failed."));
            m_csInfo.SetWindowText(strTemp);
            return;
        }

        m_cOutputFile.Write(m_pbOutputData, m_nOutputSize);

        m_nCurrentFrame++;
    }
    strTemp.Format(_T("Transform finish."));
    m_csInfo.SetWindowText(strTemp);

    m_cOutputFile.Close();
}


void CYuvTransform::OnDropFiles(HDROP hDropInfo)
{
    CDialogEx::OnDropFiles(hDropInfo);

    wchar_t* pFilePathName =(wchar_t *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName, MAX_URL_LENGTH);
    m_strPathName.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);

    // 找文件名
    wchar_t* tmp = wcsrchr(pFilePathName, '\\');
    
    char szFilename[256] = {0};
    WideCharToMultiByte(CP_ACP, 0, tmp+1, wcslen(tmp+1), szFilename, 256, NULL, NULL);
    free(pFilePathName);

    ParseFilename(szFilename);

    wchar_t szExt[16] = {0};
    wchar_t szFilename1[256] = {0};
    _wsplitpath(m_strPathName, NULL, NULL, szFilename1, szExt); // 要单独开辟空间存储，否则窗口关闭后有非法内存使用

    m_strFileTittle = szFilename1;
    m_strFileExtern.Format(_T("%s"), &szExt[1]);

    UpdateData(FALSE);
}

void CYuvTransform::ParseFilename(const char* pFilename)
{
    int nYuvFormat = 0;
    int nWidth = 0;
    int nHeight = 0;

    find_resolution((char*)pFilename, nYuvFormat, nWidth, nHeight);

    // 保留原来的值
    if (nYuvFormat != -1)
    {
        m_nYuvFormat = nYuvFormat;
    }
    if (nWidth > 0 && nHeight > 0)
    {
        m_nWidth = nWidth;
        m_nHeight = nHeight;
    }

    m_cbInput.SetCurSel(m_nYuvFormat);
    UpdateData(FALSE);
}

// 内部实现
INT CYuvTransform::Open(wchar_t* pFile)
{
    if (CFile::hFileNull != m_cFile.m_hFile)
    {
        m_cFile.Close();
    }
    if (!m_cFile.Open(pFile,CFile::modeRead))
    {
        MessageBox(_T("Open YUV file failed."));
        return - 1;
    }

    return 0;
}

BOOL CYuvTransform::IsOpen()
{
    if (CFile::hFileNull != m_cFile.m_hFile)
        return TRUE;
    else
        return FALSE;
}

INT CYuvTransform::Malloc()
{
    // 根据YUV格式分配内存
    switch (m_nYuvFormat)
    {
    case FMT_YUV420:
    case FMT_YV12:
    case FMT_NV12:
    case FMT_NV21:
        m_nYuvSize = m_nWidth * m_nHeight * 3 / 2;
        break;
    case FMT_YUV422:
    case FMT_YV16:
    case FMT_YUYV:
    case FMT_YVYU:
    case FMT_UYVY:
    case FMT_VYUY:
    case FMT_NV16:
    case FMT_NV61:
        m_nYuvSize = m_nWidth * m_nHeight * 2;
        break;
    case FMT_YUV444:
        m_nYuvSize = m_nWidth * m_nHeight * 3;
        break;
    case FMT_Y:
        m_nYuvSize = m_nWidth * m_nHeight;
        break;
    default:
        return -1;
        break;
    }

    m_nOutputSize = m_nYuvSize;

    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbOutputData != NULL)
    {
        delete[] m_pbOutputData;
        m_pbOutputData = NULL;
    }

    m_pbYuvData = new char[m_nYuvSize];
    m_pbOutputData  = new char[m_nOutputSize];

    m_nCurrentFrame = 0;
    if (CFile::hFileNull != m_cFile.m_hFile)
        m_nTotalFrame = (UINT)(m_cFile.GetLength() / m_nYuvSize);

    return 0;
}

void CYuvTransform::Read(INT nCurrentFrame)
{
    m_cFile.Seek(m_nYuvSize * (nCurrentFrame), SEEK_SET);
    m_cFile.Read(m_pbYuvData, m_nYuvSize);
}

INT CYuvTransform::Write(INT nAppend)
{
    wchar_t extp[32] = {0};

    m_cbOutput.GetWindowText((LPTSTR)extp, 32);

    CString strTemp = extp;
    strTemp.MakeLower();

    if (nAppend > 0)
        m_strOutputFile.Format(_T("./out/%s_%s.%s"), m_strFileTittle, strTemp.GetBuffer(), m_strFileExtern);
    else
        m_strOutputFile.Format(_T("./out/%s_%s_%d.%s"), m_strFileTittle, strTemp.GetBuffer(), m_nCurrentFrame, m_strFileExtern);

    m_cOutputFile.Open(m_strOutputFile.GetBuffer(), CFile::modeWrite|CFile::modeCreate | CFile::modeNoTruncate);
    m_cOutputFile.Write(m_pbOutputData, m_nOutputSize);
    m_cOutputFile.Close();

    return 0;
}

// todo 找个好点的方法
INT CYuvTransform::Transform()
{
    UpdateData();

    m_nOutputFormat = m_cbOutput.GetCurSel();

    if (m_nYuvFormat == FMT_YUV422 && (m_nOutputFormat == FMT_NV16 || m_nOutputFormat == FMT_NV61))
    {
        yuv422p_to_yuv422sp((YUV_TYPE)m_nOutputFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);
    }
    else if ((m_nYuvFormat == FMT_NV16 || m_nYuvFormat == FMT_NV61) &&m_nOutputFormat == FMT_YUV422)
    {
        yuv422sp_to_yuv422p((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if (m_nYuvFormat == FMT_YUV420 && (m_nOutputFormat == FMT_NV12 || m_nOutputFormat == FMT_NV21))
    {
        yuv420p_to_yuv420sp((YUV_TYPE)m_nOutputFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if ((m_nYuvFormat == FMT_NV12 || m_nYuvFormat == FMT_NV21) && m_nOutputFormat == FMT_YUV420)
    {
        yuv420sp_to_yuv420p((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if (m_nYuvFormat == FMT_YUV422 && m_nOutputFormat == FMT_YV16)
    {
        yu_to_yv((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if (m_nYuvFormat == FMT_YUV420 && m_nOutputFormat == FMT_YV12)
    {
        yu_to_yv((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if (m_nYuvFormat == FMT_YV16 && m_nOutputFormat == FMT_YUV422)
    {
        yv_to_yu((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if (m_nYuvFormat == FMT_YV12 && m_nOutputFormat == FMT_YUV420)
    {
        yv_to_yu((YUV_TYPE)m_nYuvFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);

    }
    else if ((m_nYuvFormat == FMT_YUV422 || m_nYuvFormat == FMT_YV16) && (m_nOutputFormat == FMT_YUYV || m_nOutputFormat == FMT_YVYU || m_nOutputFormat == FMT_UYVY || m_nOutputFormat == FMT_VYUY))
    {
        yuv422p_to_yuv422packed((YUV_TYPE)m_nYuvFormat, (YUV_TYPE)m_nOutputFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);
    }
        else if ((m_nYuvFormat == FMT_YUYV || m_nYuvFormat == FMT_YVYU || m_nYuvFormat == FMT_UYVY || m_nYuvFormat == FMT_VYUY) &&(m_nOutputFormat == FMT_YUV422 || m_nOutputFormat == FMT_YV16))
    {
        yuv422packed_to_yuv422p((YUV_TYPE)m_nYuvFormat, (YUV_TYPE)m_nOutputFormat, (unsigned char*)m_pbYuvData, (unsigned char*)m_pbOutputData, m_nWidth, m_nHeight);
    }
    else
    {
        MessageBox(_T("Sorry, can not do it."));
        return -1;
    }

    return 0;
}

void CYuvTransform::OnBnClickedOk()
{
    if (CFile::hFileNull != m_cFile.m_hFile)
    {
        m_cFile.Close();
    }
    if (m_pbYuvData != NULL)
    {
        delete[] m_pbYuvData;
        m_pbYuvData = NULL;
    }

    if (m_pbOutputData != NULL)
    {
        delete[] m_pbOutputData;
        m_pbOutputData = NULL;
    }

    CDialogEx::OnOK();
}
