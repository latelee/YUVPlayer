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
    m_nHeight = 0;
    m_nWidth = 0;
    m_nFps = 0;
    m_nYuvFormat = 0;
    m_fLoop = FALSE;
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
END_MESSAGE_MAP()


// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_cbYuvFormat.SetCurSel(0);
    m_cbFps.SetCurSel(7);
    m_cbResolution.SetCurSel(0);
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    CString strTemp;
    m_cbResolution.GetWindowText(strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%dx%d"), &m_nWidth, &m_nHeight);
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

    m_pParentWnd->SetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}


void CSettingDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnCancel();
}


void CSettingDlg::OnBnClickedOk()
{
    m_pParentWnd->SetParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
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
