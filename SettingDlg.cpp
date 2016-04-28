// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVPlayer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "YUVPlayerDlg.h"

#define ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

// 从文件名称解析分辨率及YUV格式
void find_resolution(char* filename, int& fmt_idx, int& width, int& height)
{
#define str_debug
    int i = 0;
    int j = 0;
    int len = 0;
    int pos = 0;
    char* tmp = NULL;
    char* p = NULL;
    char c_width[8] = {0};
    char c_height[8] = {0};
    char c_fmt[8] = {0};
    char c_res[8] = {0};
    char c_file[128] = {0};

    const char* fmt_str[] = {
        "yuv420",
        "yv12",
        "yuv422",
        "yv16",
        "yuv444",
        "yuyv",
        "yvyu",
        "uyvy",
        "vyuy",
        "nv12",
        "nv21",
        "nv16",
        "nv61",
        "rgb",
        "bgr",
        "yuv420sp",
        "yuv422sp",
    };

    const char* res_str[] = {
        "cif",
        "qcif",
        "480p",
        "720p",
        "1080p",
    };
    const char* wh_str[] = {
        "352x288",
        "176x144",
        "720x480",
        "1280x720",
        "1920x1080",
    };

    int idx = -1;
    fmt_idx = -2; // trick...

#if 0
    len = strlen(filename);
    str_debug("org string len:%d %s\b", len, filename);
#endif

    // 查找后缀名
    tmp = strrchr(filename, '.');
    if (tmp != NULL)
    {
        strncpy(c_file, filename, tmp-filename);
        pos = strlen(tmp+1);
        //str_debug("find ext: %s(%d) %s %s %d\n", tmp+1, pos, filename, c_file, tmp-filename);
        // 查找格式字符串
        for (i = 0; i < ARRAY_ELEMS(fmt_str); i++)
        {
            p = strstr(tmp+1, fmt_str[i]);
            if (p != NULL)
            {
                strncpy(c_fmt, p, strlen(fmt_str[i]));
                idx = i;
            }
        }
    }
    else
    {
        strcpy(c_file, filename);
    }
    str_debug("second string:%s \n", c_file);
    if (idx == 0 && pos != 1) idx = -1;
    // 处理sp字符串
    if (idx == 15) idx = 9;
    if (idx == 16) idx = 11;
    // 这里打印的是最大匹配的
    if (idx != -1)
        str_debug("fmt(externsion)[%d]: %s(=%s)\n", idx, c_fmt, fmt_str[idx]);    

    // 查找格式字符串
    for (i = 0; i < ARRAY_ELEMS(fmt_str); i++)
    {
        p = strstr(c_file, fmt_str[i]);
        //str_debug("p: %s\n", p);
        if (p != NULL)
        {
            strncpy(c_fmt, p, strlen(fmt_str[i]));
            fmt_idx = i;
        }
    }
    if (fmt_idx == 15) fmt_idx = 9;
    if (fmt_idx == 16) fmt_idx = 11;
    // 这里打印的是最大匹配的
    if (fmt_idx != -2)
        str_debug("fmt[%d]: %s(=%s)\n", fmt_idx, c_fmt, fmt_str[fmt_idx]);

    if (idx != -1 && fmt_idx == -2) fmt_idx = idx;
    if (fmt_idx != -2)
        str_debug("---final fmt[%d]: %s(=%s)---\n", fmt_idx, c_fmt, fmt_str[fmt_idx]);

    fmt_idx+=1;

    // 查找分辨率字符串
    for (i = 0; i < ARRAY_ELEMS(res_str); i++)
    {
        //str_debug("res--: %s\n", res_str[i]);
        p = strstr(c_file, res_str[i]);
        if (p != NULL)
        {
            strncpy(c_res, p, strlen(res_str[i]));
            idx = i;
            //str_debug("res: %s\n", c_res);
        }
    }

    if (idx != -1)
    {
        sscanf(wh_str[idx], "%dx%d", &width, &height);
        str_debug("res[%d]: %s %d %d\n", idx, c_res, width, height);
    }
    //////////////////////

    //  解析宽高
    len = strlen(c_file);
    pos = strcspn(c_file, "xX");
    if (len == pos) // 没有找到直接返回
    {
        str_debug("---final width: %d height: %d---\n", width, height);
        return;
    }
    tmp = c_file + pos;
    str_debug("find1 x[%d len: %d] %s\n", pos, len, tmp);

    //return;
    p = c_file;
    len = len - pos + 1;
    j = pos;
    // 找到'x'前面的数字
    for (i = 0; i < len && j > 0; i++)
    {
        str_debug("pos: %d %c\n", j, p[j]);
        if (!isdigit(p[--j]))
        {
            j++;
            break;
        }
    }
    str_debug("before x i:%d j: %d tmp: %s\n", i, j, p+j);
    strncpy(c_width, p+j, i);
    width = atoi(c_width);
    str_debug("c_width: %s %d\n", c_width, width);

    // 找到'x'后面的数字
    p = c_file + pos+1; // 跳过'x'
    len = p - c_file;
    str_debug("after find x[%d] %s\n", pos, p);
    for (i = 0; i < len; i++)
    {
        if (!isdigit(p[i]))
            break;
    }
    strncpy(c_height, p, i);
    height = atoi(c_height);
    str_debug("c_height: %s %d\n", c_height, height);

    if (idx == -1 && (width == 0 || height == 0))
        width = height = -1;
    if (idx != -1 && (width == 0 || height == 0))
        sscanf(wh_str[idx], "%dx%d", &width, &height);
    str_debug("---final width: %d height: %d---\n", width, height);

    str_debug("\n");
}

// 根据宽高排序
void BubbleSort(CStringArray &ca)
{
    int len = ca.GetCount();
    int width1 = 0;
    int height1 = 0;
    int width2 = 0;
    int height2 = 0;
    CString tmp;

    for (int i = 0; i < len; i++)
    {
        for (int j = i; j < len; j++)
        {
            swscanf_s(ca[i].GetBuffer(), _T("%dx%d"), &width1, &height1);
            swscanf_s(ca[j].GetBuffer(), _T("%dx%d"), &width2, &height2);
            if ((width1 > width2) || (width1 == width2 && height1 > height2))
            {
                tmp = ca.GetAt(i);
                ca.SetAt(i, ca.GetAt(j));
                ca.SetAt(j, tmp);
            }
        }
    }
}
// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{
    m_pParentWnd = NULL;
    m_strAddedSize = _T("160x120;176x144;320x240;352x288;640x480;704x576;720x480;1280x720;1600x1200;1920x1080");
    m_nWidth = 176;
    m_nHeight = 144;
    m_nFps = 30;
    m_nFpsIndex = 7; // index 7 == 30
    m_nYuvFormat = 1;   // default yuv420
    m_fLoop = FALSE;
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
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

    RegCreateKey(HKEY_CURRENT_USER, _T("Software\\YUVPlayer-latelee.org\\Setting"), &hKey);
    RegSetValueEx(hKey, _T("AddedSize"), 0, REG_SZ, (BYTE*)(strSize.GetBuffer()), strSize.GetLength()*sizeof(TCHAR));
    RegSetValueEx(hKey, _T("FrameWidth"), 0, REG_DWORD, (BYTE*)&width, 4);
    RegSetValueEx(hKey, _T("FrameHeight"), 0, REG_DWORD, (BYTE*)&height, 4);
    RegSetValueEx(hKey, _T("FrameRate"), 0, REG_DWORD, (BYTE*)&fpsidx, 4);
    RegSetValueEx(hKey, _T("CurrentPixelFormat"), 0, REG_DWORD, (BYTE*)&fmt, 4);
    RegSetValueEx(hKey, _T("Loop"), 0, REG_DWORD, (BYTE*)&loop, 4);

    RegCloseKey(hKey);
}

void CSettingDlg::GetRegistration(CString& strSize, int& width, int& height, int& fpsidx, int& fmt, int& loop)
{
    HKEY hKey;
    DWORD dwType = 0;
    DWORD dwLen = 1024; // 此处值是否适合？CString类型如何给定长度？
    BYTE szBuffer[1024] = {0};
    RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Software\\YUVPlayer-latelee.org\\Setting"), 0, KEY_QUERY_VALUE, &hKey);
    RegQueryValueEx(hKey, _T("AddedSize"), NULL, &dwType, szBuffer, &dwLen);

    RegQueryValueEx(hKey, _T("FrameWidth"), NULL, &dwType, (BYTE*)&width, &dwLen);
    RegQueryValueEx(hKey, _T("FrameHeight"), NULL, &dwType, (BYTE*)&height, &dwLen);
    RegQueryValueEx(hKey, _T("FrameRate"), NULL, &dwType, (BYTE*)&fpsidx, &dwLen);
    RegQueryValueEx(hKey, _T("CurrentPixelFormat"), NULL, &dwType, (BYTE*)&fmt, &dwLen);
    RegQueryValueEx(hKey, _T("Loop"), NULL, &dwType, (BYTE*)&loop, &dwLen);

    strSize.Format(_T("%s"), szBuffer);

    RegCloseKey(hKey);
}
// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CString strTemp;

    // default
    if (!ExistRegistration())
    {
        SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
    }
    // read
    GetRegistration(m_strAddedSize, m_nWidth, m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);

    int pos = 0;  
    strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
    m_strArrAddedSize.Add(strTemp);
    while (strTemp != _T(""))
    {
        strTemp = m_strAddedSize.Tokenize(_T(";"), pos);
        m_strArrAddedSize.Add(strTemp);
    }

    // 不知为何，最后一个是空的，这里删除，否则后面再添加时会多一个空字符串
    m_strArrAddedSize.RemoveAt(m_strArrAddedSize.GetCount()-1);

    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount(); i++)
    {
        m_cbResolution.AddString(m_strArrAddedSize[i]);
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

    m_cbYuvFormat.SetCurSel(m_nYuvFormat);
    m_cbFps.SetCurSel(m_nFpsIndex);
    m_cbResolution.SetCurSel(nResolutionIdx);
    m_nYuvFormat = m_cbYuvFormat.GetCurSel();
    m_cbFps.GetWindowText(strTemp);
    swscanf_s(strTemp.GetBuffer(), _T("%d"), &m_nFps);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
}

// 应用，不要关闭窗口
void CSettingDlg::OnBnClickedApply()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData();

    UpdateRes();

    m_pParentWnd->SetParentParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}


void CSettingDlg::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
}


void CSettingDlg::OnBnClickedOk()
{
    m_pParentWnd->SetParentParameters(m_nWidth, m_nHeight, m_nFps, m_nYuvFormat, m_fLoop);
    SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
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
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnSelchangeCbFps()
{
    CString strTemp;
    m_nFpsIndex = m_cbFps.GetCurSel();
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
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}


void CSettingDlg::OnChangeEHeight()
{
    UpdateData();
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

// 向注册表添加一种分辨率
void CSettingDlg::OnBnClickedBtAdd()
{
    CString strTemp;
    CString strAddedSize;

    UpdateData();

    for (int i = 0; i < m_strArrAddedSize.GetCount(); i++)
    {
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            return;
    }
    strTemp.Format(_T("%dx%d"), m_nWidth, m_nHeight);

    // 尾部添加
    m_strArrAddedSize.Add(strTemp);

    // 排序
    BubbleSort(m_strArrAddedSize);

    // 先清空
    m_strAddedSize.Empty(); 
    m_cbResolution.ResetContent();
    // 再添加
    for (int i = 0; i < m_strArrAddedSize.GetCount(); i++)
    {
        m_cbResolution.AddString(m_strArrAddedSize[i].GetBuffer());
        strTemp.Format(_T("%s;"), m_strArrAddedSize[i].GetBuffer());
        m_strAddedSize += strTemp;
    }
    UpdateRes();

    return;
    // todo 在Add时，到底要不要写注册表？还是等到apply时才做？
    SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);

}

// 在注册表删除一种分辨率
void CSettingDlg::OnBnClickedBtDel()
{
    UpdateData();
    wchar_t szRes[32] = {0};
    m_cbResolution.GetWindowText(szRes,32);

    int idx = m_cbResolution.GetCurSel();
    if (idx == -1) return;
    m_strArrAddedSize.RemoveAt(idx); // 删除当前下拉框中数据

    CString strTemp;
    m_strAddedSize.Empty();
    m_cbResolution.ResetContent();
    for (int i = 0; i < m_strArrAddedSize.GetCount() ; i++)
    {
        m_cbResolution.AddString(m_strArrAddedSize[i].GetBuffer());
        strTemp.Format(_T("%s;"), m_strArrAddedSize[i].GetBuffer());
        m_strAddedSize += strTemp;
    }

    UpdateRes();
    SetRegistration(m_strAddedSize, m_nWidth,m_nHeight, m_nFpsIndex, m_nYuvFormat, m_fLoop);
}

void CSettingDlg::ParseFilename(const char* pFilename)
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
    // 更新控件
    UpdateRes();
    m_cbYuvFormat.SetCurSel(m_nYuvFormat);
    UpdateData(FALSE);
}

void CSettingDlg::UpdateRes()
{
    int nResolutionIdx = -1;
    for (int i = 0; i < m_strArrAddedSize.GetCount(); i++)
    {
        int width = 0;
        int height = 0;
        swscanf_s(m_strArrAddedSize[i].GetBuffer(), _T("%dx%d"), &width, &height);
        if (width == m_nWidth && height == m_nHeight)
            nResolutionIdx = i;
    }

    m_cbResolution.SetCurSel(nResolutionIdx);
}