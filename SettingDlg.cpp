// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "YUVPlayerDlg.h"

// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{
    m_pParentWnd = NULL;
    m_nWidth = 176;
    m_nHeight = 144;
    m_nFps = 30;
    m_nFpsIndex = 7; // index 7 == 30
    m_nYuvFormat = 0;
    m_fLoop = FALSE;

    m_strAddedSize = _T("160x120;176x174;320x240;352x288;640x480;704x576");
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    //  DDX_Control(pDX, IDC_CB_YUV_FMT, m_cbResolution);
    DDX_Control(pDX, IDC_CB_FPS, m_cbFps);
    DDX_Control(pDX, IDC_CB_YUV_FMT, m_cbYuvFormat);
    DDX_Control(pDX, IDC_CB_SIZE, m_cbResolution);
    DDX_Text(pDX, IDC_E_HEIGHT, m_nHeight);
    DDV_MinMaxInt(pDX, m_nHeight, 1, 65535);
    DDX_Text(pDX, IDC_E_WIDTH, m_nWidth);
    DDV_MinMaxInt(pDX, m_nWidth, 1, 65535);
    DDX_Check(pDX, IDC_CK_LOOP, m_fLoop);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
    ON_BN_CLICKED(IDC_APPLY, &CSettingDlg::OnBnClickedApply)
    ON_BN_CLICKED(IDCANCEL, &CSettingDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_CB_SIZE, &CSettingDlg::OnSelchangeCbSize)
    ON_CBN_SELCHANGE(IDC_CB_YUV_FMT, &CSettingDlg::OnSelchangeCbYuvFmt)
    ON_CBN_SELCHANGE(IDC_CB_FPS, &CSettingDlg::OnSelchangeCbFps)
    ON_BN_CLICKED(IDC_CK_LOOP, &CSettingDlg::OnClickedCkLoop)
    ON_EN_CHANGE(IDC_E_WIDTH, &CSettingDlg::OnChangeEWidth)
    ON_EN_CHANGE(IDC_E_HEIGHT, &CSettingDlg::OnChangeEHeight)
    ON_BN_CLICKED(IDC_BT_ADD, &CSettingDlg::OnBnClickedBtAdd)
    ON_BN_CLICKED(IDC_BT_DEL, &CSettingDlg::OnBnClickedBtDel)
END_MESSAGE_MAP()

BOOL CSettingDlg::ExistRegistration()
{
    HKEY hKey;
    int ret = 0;
    ret = RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Software\\YUVPlayer-latelee.org"), 0, KEY_ALL_ACCESS, &hKey);
    RegCloseKey(hKey);
    return !ret;
}

void CSettingDlg::SetRegistration(CString& strSize, int width, int height, int fpsidx, int fmt, int loop)
{
    HKEY hKey;
    if (RegCreateKey(HKEY_CURRENT_USER, _T("Software\\YUVPlayer-latelee.org\\Setting"), &hKey) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegCreateKey failed with Erro "));
        return;
    }
    if (RegSetValueEx(hKey, _T("AddedSize"), 0, REG_SZ, (BYTE*)(strSize.GetBuffer()), strSize.GetLength()*sizeof(TCHAR)) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write addsize"));
        return;
    }
    if (RegSetValueEx(hKey, _T("FrameWidth"), 0, REG_DWORD, (BYTE*)&width, 4) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write width"));
        return;
    }
    if (RegSetValueEx(hKey, _T("FrameHeight"), 0, REG_DWORD, (BYTE*)&height, 4) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write height"));
        return;
    }
    if (RegSetValueEx(hKey, _T("FrameRate"), 0, REG_DWORD, (BYTE*)&fpsidx, 4) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write frame rate"));
        return;
    }
    if (RegSetValueEx(hKey, _T("CurrentPixelFormat"), 0, REG_DWORD, (BYTE*)&fmt, 4) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write fmt"));
        return;
    }
    if (RegSetValueEx(hKey, _T("Loop"), 0, REG_DWORD, (BYTE*)&loop, 4) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegSetValueEx failed to write loop"));
        return;
    }
    RegCloseKey(hKey);
}

void CSettingDlg::GetRegistration(CString& strSize, int& width, int& height, int& fpsidx, int& fmt, int& loop)
{
    HKEY hKey;
    DWORD dwType = 0;
    DWORD dwLen = 1024; // 此处值是否适合？CString类型如何给定长度？
    //BYTE szBuffer[1024] = {0};
    if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Software\\YUVPlayer-latelee.org\\Setting"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegOpenKeyEx failed with error"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("AddedSize"), NULL, &dwType, (BYTE*)(strSize.GetBuffer()), &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx AddedSize failed"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("FrameWidth"), NULL, &dwType, (BYTE*)&width, &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx FrameWidth failed"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("FrameHeight"), NULL, &dwType, (BYTE*)&height, &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx FrameWidth failed"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("FrameRate"), NULL, &dwType, (BYTE*)&fpsidx, &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx FrameWidth failed"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("CurrentPixelFormat"), NULL, &dwType, (BYTE*)&fmt, &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx FrameWidth failed"));
        return;
    }
    if (RegQueryValueEx(hKey, _T("Loop"), NULL, &dwType, (BYTE*)&loop, &dwLen) != ERROR_SUCCESS)
    {
        MessageBox(_T("RegQueryValueEx FrameWidth failed"));
        return;
    }

    RegCloseKey(hKey);
}
// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  使用注册表里的值
    // 
    CString strTemp;

    // default
    if (!ExistRegistration())
    {
        SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
    }
    // read
    GetRegistration(m_strAddedSize, m_nWidth, m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
#if 0
    // debug
    CString strMsg;
    strMsg.Format(_T("%s %d %d %d %d %d"), m_strAddedSize, m_nWidth, m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
    MessageBox(strMsg);
#endif

    int pos = 0;  
    strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
    m_strArrAddedSize.Add(strTemp);
    while (strTemp != _T(""))
    {
        strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
        m_strArrAddedSize.Add(strTemp);
    }

    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount()-1; i++)
    {
        m_cbResolution.AddString(m_strArrAddedSize[i]);
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

#if 0
    m_cbResolution.AddString(_T("160x120"));
    m_cbResolution.AddString(_T("176x174"));
    m_cbResolution.AddString(_T("320x240"));
    m_cbResolution.AddString(_T("352x288"));
    m_cbResolution.AddString(_T("640x480"));
    m_cbResolution.AddString(_T("704x576"));
#endif

    m_cbYuvFormat.SetCurSel(0);
    m_cbFps.SetCurSel(m_nFpsIndex);
    m_cbResolution.SetCurSel(nResolutionIdx);
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    m_cbResolution.GetWindowText(strTemp);
    //swscanf_s(strTemp.GetBuffer(), _T("%dx%d"), &m_nWidth, &m_nHeight);
    m_cbFps.GetWindowText(strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%d"), &m_nFps);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

    
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

// 应用，不要关闭窗口
void CSettingDlg::OnBnClickedApply()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData();
    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount()-1; i++)
    {
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

    m_cbResolution.SetCurSel(nResolutionIdx);

    m_pParentWnd->GetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}


void CSettingDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnCancel();
}


void CSettingDlg::OnBnClickedOk()
{
    m_pParentWnd->GetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    CDialogEx::OnOK();
}


void CSettingDlg::OnSelchangeCbSize()
{
    CString strTemp;
    m_cbResolution.GetLBText(m_cbResolution.GetCurSel(), strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%dx%d"), &m_nWidth, &m_nHeight);

    UpdateData(FALSE);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnSelchangeCbYuvFmt()
{
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    //UpdateData(FALSE);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnSelchangeCbFps()
{
    CString strTemp;
    m_cbFps.GetLBText(m_cbFps.GetCurSel(), strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%d"), &m_nFps);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnClickedCkLoop()
{
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CK_LOOP);
    m_fLoop = pBtn->GetCheck();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnChangeEWidth()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnChangeEHeight()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

// 向注册表添加一种分辨率
void CSettingDlg::OnBnClickedBtAdd()
{
    // TODO: 在此添加控件通知处理程序代码
}

// 在注册表删除一种分辨率
void CSettingDlg::OnBnClickedBtDel()
{
    // TODO: 在此添加控件通知处理程序代码
}
