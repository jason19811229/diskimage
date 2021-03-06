// DiskImageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DiskImage.h"
#include "DiskImageDlg.h"
#include "tgram_image.h"
#include "tgram_image.h"
#include <io.h>
#include <assert.h>
#include <sys\stat.h>
#include <winioctl.h>
#include<map>
#include <time.h>  
#include <stdio.h> 
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct  partition_image_t{
	struct tangram_image_header header;
	struct partition_item item[PART_END_NUM];
};

struct  partition_image_t partition_image;

map<UINT64,int>  recordMap;

static const UINT32  crc32_table[256] = {
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
	0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
	0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
	0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
	0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
	0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
	0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
	0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
	0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
	0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
	0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
	0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
	0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
	0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
	0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
	0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
	0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

extern const char *const partition_name[PART_END_NUM] ={
	PART_TABLE_NAME,"NULL",PART_MCU_PBL_NAME,"NULL",PART_CPU_PBL_NAME,"NULL",PART_RTOS_NAME,"NULL",PART_BL_NAME,
	"NULL",PART_SHARED_NAME,PART_RESERVED_NAME,PART_KERNEL_NAME,PART_FILESYS_NAME,PART_PERSIST_NAME,PART_CACHE_NAME,
	PART_RECOVERY_NAME,PART_USERDATA_NAME
	,
};



static UINT32 crc32_byte(UINT32 crc, UINT8 data)
{
	return (crc << 8) ^ crc32_table[(crc >> 24) ^ data];
}

UINT32 crc32(UINT32 crc, const UINT8 *buf, UINT32 len)
{
	while (len--)
		crc = crc32_byte(crc, *buf++);
	return crc;
}



#define BUFSIZE 1024
BOOL GetDriverInfo(LPSTR szDrive);

extern struct  partition_image_t partition_image;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDiskImageDlg 对话框




CDiskImageDlg::CDiskImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiskImageDlg::IDD, pParent)
	, m_path(_T(""))
	, m_sector(_T(""))
	, m_pathfile(_T(""))
	, mStatus(_T(""))
	, mSpeed(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiskImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_path);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_COMBO1, m_driver_list);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_EDIT2, m_sector);
	//DDX_Text(pDX, IDC_EDIT3, m_pathfile);
	DDX_Text(pDX, IDC_STATUS, mStatus);
	DDX_Text(pDX, IDC_SPEED, mSpeed);
}

BEGIN_MESSAGE_MAP(CDiskImageDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDiskImageDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDiskImageDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MYMESSAGE, OnMyMessage)  
	ON_BN_CLICKED(IDC_BUTTON3, &CDiskImageDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDiskImageDlg::OnBnClickedButton4)
END_MESSAGE_MAP()



LONG CDiskImageDlg::OnMyMessage(WPARAM wParam, LPARAM lParam ) 
{   
	
		
		if(wParam == 0) {

			m_progress.SetPos((int)lParam);
			mStatus = "正在写入image...";
			//mSpeed.Format("速度是：%dK/s",(int)lParam);
		//	UpdateData(FALSE);
				UpdateData(FALSE);

		}
		else if(wParam == 1) {
			mStatus = "正在擦除预留空间...";
		}else if(wParam == 2) {
			mStatus = "完成";
			UpdateData(FALSE);
		} else if(wParam == 4) {
			if((int)lParam < 0)
				lParam = 8000;
			mSpeed.Format("速度是：%dK/s",(int)lParam);
			UpdateData(FALSE);
			
		
		}
			
		
	//	GetDlgItem(IDC_STATUS)->SetDlgItemText(IDC_STATUS,mStatus);
		return (LONG)1;

}
// CDiskImageDlg 消息处理程序

BOOL CDiskImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);
	scanDisk();

	(CEdit *)GetDlgItem(IDC_EDIT2)->ShowWindow(false);;
	(CButton *)GetDlgItem(IDC_BUTTON3)->ShowWindow(false);

	

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDiskImageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDiskImageDlg::OnPaint()
{
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDiskImageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDiskImageDlg::OnBnClickedButton1()
{


	BROWSEINFO bi;

	char Buffer[MAX_PATH];

	bi.hwndOwner = NULL;

	bi.pidlRoot =NULL;

	bi.pszDisplayName = Buffer;

	bi.lpszTitle = "选择目标文件路径";

	bi.ulFlags = BIF_EDITBOX;

	bi.lpfn = NULL;

	bi.lParam = 0;

	bi.iImage=IDR_MAINFRAME;


	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);

	if(pIDList)

	{

		SHGetPathFromIDList(pIDList, Buffer);

		m_path = Buffer;

		UpdateData(false);
		//this->SendMessage(WM_MYMESSAGE,0,0);

		CString filePath;
		//UpdateData(true);
		if (m_path == "") {
			AfxMessageBox(_T("路径不能为空!"));
		}
		m_ListBox.ResetContent();
		m_file_array.RemoveAll();

		CFileFind finder;
		BOOL bWorking = finder.FindFile(m_path+"\\*.*");
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if(finder.GetFileName() != "." && finder.GetFileName() != ".." )
			{
				m_ListBox.AddString(finder.GetFileName());
				m_file_array.Add(finder.GetFileName());

			}

		}
	}


	// TODO: 在此添加控件通知处理程序代码
}

void CDiskImageDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	

	hThread=CreateThread(NULL,0,ThreadFunc,(LPVOID)this,0,NULL);
	if(!hThread) {
		printf("create thread failed");
		return ;
	}


}

ULONG CDiskImageDlg:: ThreadFunc(LPVOID pParam)
{
	CDiskImageDlg * pDlg = (CDiskImageDlg *)pParam;
	pDlg->writeToSD();
	return 0;

}

int CDiskImageDlg::burn_gpt(HANDLE hDev,const char *path) 
{

	char file_name[PATH_MAX];
	FILE *fp = NULL;
	UINT32 chunk = 0;
	UINT8 buffer[SECTOR_SIZE];
	DWORD dwCB;
	UINT64 off = 0;
	CFile file;
	BOOL bRet;
	_snprintf(file_name, PATH_MAX, "%s\\%s", path, PART_PTA_NAME);


	if(!file.Open(CString(file_name),CFile::modeRead|CFile::typeBinary))
	{
		CString strName;
		strName.Format("打开文件失败 文件路径%s  %d \n",file_name,GetLastError());
		AfxMessageBox(strName);
		return -1;


	}

	memset(buffer,0,SECTOR_SIZE);
	while ((chunk =file.Read(buffer,SECTOR_SIZE)) >0) {
		

			PBYTE pBuffer = (PBYTE)buffer;

			if(chunk <= SECTOR_SIZE)
			{
				chunk = SECTOR_SIZE;

			}
		
		
			bRet = WriteFile(hDev, pBuffer, chunk, &dwCB, NULL); 
			if(!bRet || chunk!= (UINT64)dwCB){
			
				file.Close();
				int result = GetLastError();
				printf("WriteFile error %d",GetLastError());
				AfxMessageBox("写SD卡失败!");
				return -1;
			}
			memset(buffer,0,SECTOR_SIZE);
	}
	
	file.Close();
	return 0;
}

int CDiskImageDlg::burn_mbr(HANDLE hDev,const char *path)
{
	char file_name[PATH_MAX];
	FILE *fp = NULL;
	UINT32 chunk = 0;
	UINT8 buffer[SECTOR_SIZE];
	DWORD dwCB;
	int bRet;
	
	_snprintf(file_name, PATH_MAX, "%s\\%s", path, PART_MBR_NAME);
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("open file error! path:%s", file_name);
		AfxMessageBox("不能打开mbr.img文件!");
		return -1;
	}

	SetFilePointer(hDev, 0, 0, FILE_BEGIN);

	PBYTE pBuffer = buffer;

	if((chunk = fread(pBuffer, 1,512, fp)) >0) {

		bRet = WriteFile(hDev, pBuffer, chunk, &dwCB, NULL); 

		if(!bRet || chunk!= (UINT64)dwCB){
			CloseHandle(hDev);
			fclose(fp);
			int result = GetLastError();
			printf("WriteFile error %d",GetLastError());
			AfxMessageBox("写mbr失败!");
			return -1;
		}
	}
	fclose(fp);
	return 0;
}

int CDiskImageDlg:: burn_sd(HANDLE hDev, const char *path, int part)
{
	const char *name = NULL;
	char file_name[PATH_MAX];
	UINT8 buffer[2048];
	FILE *fp = NULL;
	UINT64 size = 0;
	UINT32 chunk = 0;
	LONGLONG off = 0;
	UINT32 count = 0;
	int ret = 0;
	UINT64 file_size = 0;
	UINT64 percent = 0;
	DWORD dwCB; 
	BOOL bRet;
	clock_t start ,end ;  
	double totalT;  
	int count_time = 0;
	switch (part) {
		case PART_TABLE:
		case PART_MCU_PBL:
		case PART_CPU_PBL:
		case PART_RTOS:
		case PART_BL:
		case PART_SHARED:
		case PART_RESERVED:
		case PART_KERNEL:
		case PART_FILESYS:
		case PART_PERSIST:
		case PART_CACHE:
		case PART_RECOVERY:
		case PART_USERDATA:
			name = partition_name[part];
			break;
		default:
			name = partition_name[part - 1];
			break; 
	}
	do {
		if (name == NULL)
			break;
		_snprintf(file_name, PATH_MAX, "%s\\%s", path, name);
		m_ListBox.SelectString(0,CString(name));

		CFile file;
			
		
		if(!file.Open(CString(file_name),CFile::modeRead|CFile::typeBinary))
		{
			CString strName;
			strName.Format("打开文件失败 文件路径%s  %d \n",file_name,GetLastError());

			if(part == PART_MCU_PBL || part == PART_CPU_PBL || part == PART_RTOS

				|| part == PART_BL || part == PART_KERNEL ||part == PART_FILESYS  )
			{
				AfxMessageBox(strName);
				return -1;
			}

			break;
			
		}

		struct _stat info;
		_stat(file_name, &info);
		file_size = info.st_size;

		this->SendMessage(WM_MYMESSAGE,0,0);
		

		off = SECTOR_SIZE * ((partition_image.item[part].start_sector));

		LARGE_INTEGER li;

		li.QuadPart = off;

		li.LowPart = SetFilePointer (hDev, 
			li.LowPart, 
			&li.HighPart, 
			FILE_BEGIN);

		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() 
			!= NO_ERROR)
		{
			li.QuadPart = -1;
			file.Close();
			printf("SetFilePointer error %d",GetLastError());
			AfxMessageBox("文件指针移动失败！");
			return -1;
		}

	
		memset(buffer,SECTOR_SIZE*4,0);

		start = clock();	

		while ((chunk =file.Read(buffer,(SECTOR_SIZE*4))) >0) {
		
			start = clock();
			
			PBYTE pBuffer = (PBYTE)buffer;

			if(chunk <= 4*SECTOR_SIZE) {
				chunk = 4*SECTOR_SIZE;
			}
		
		
			bRet = WriteFile(hDev, pBuffer, chunk, &dwCB, NULL); 
			if(!bRet || chunk!= (UINT64)dwCB) {
				//fclose(fp);
				file.Close();
				int result = GetLastError();
				printf("WriteFile error %d",GetLastError());
				AfxMessageBox("写SD卡失败!");
				return -1;
			}
	
	
			size += chunk;
			count_time++;
			if(count_time == 5){
				end = clock();
				totalT= end - start;
				count_time = 0;
				start = end;
				this->SendMessage(WM_MYMESSAGE,4,(UINT32)(chunk *5)/1024.0/(totalT/1000.0));

			}
		
			percent = size * 100  / file_size;
			if(percent >= 100)
				percent = 100;
			this->SendMessage(WM_MYMESSAGE,0,(int)percent);
			count++;
			memset(pBuffer,SECTOR_SIZE*4,0);
		}

		file.Close();
		
	} while (0);
	printf("%20s, %20d, %20d\n", name,partition_image.item[part].start_sector, size);
	trace("%20s, %20d, %20d\n", name, partition_image.item[part].start_sector, size);
	return 0;
}


int CDiskImageDlg::testWriteDisk(CString  driverTxt)
{
	BOOL bRet;
	DWORD dwLen;
	UINT8 buffer[SECTOR_SIZE];

	HANDLE hFile;

	
	hFile = CreateFile(driverTxt,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0, 
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		printf("open driver d: error \n");
		printf("error number %d",GetLastError());
		AfxMessageBox("打开SD磁盘失败！");
		return -2;

	}
	

	SetFilePointer(hFile, 0, 0, FILE_BEGIN); 

	bRet = WriteFile(hFile, buffer, 512, &dwLen, NULL);
	if(!bRet ||  dwLen != 512){
		CloseHandle(hFile);
		return -1;
		
	}

	 CloseHandle(hFile);

	return 0;

}
int CDiskImageDlg::writeToSD()
{
	
	char file_name[PATH_MAX];
	int i = 0;
	int fd = -1;
	UINT64 sector = START_SECTOR;
	UINT32 size = 0;
	int    chunk = 0;
	UINT32 off = 0;
	UINT32 count = 0;
	FILE  *fp;
	int index;
	CString driverTxt;
	CString txt;
	CString temp;
	//char *file_name = "c:\\commit\\temp";
	int ret = 0;

	index = m_driver_list.GetCurSel(); 

	if(index == -1 || m_path == "")
	{
		AfxMessageBox("没有选择sd卡选择!");
		return -1;
	}
	if(m_path == ""){
		AfxMessageBox("目录有错!");
		return -1;
	}

	m_driver_list.GetLBText(index,txt);
	driverTxt = CString("\\\\.\\");
	driverTxt +=  txt.Left(txt.GetLength()-1);
   
	if(testWriteDisk(driverTxt) != 0){
		printf("write disk failed \n");
		AfxMessageBox("SD磁盘写测试失败！");
		return -2;
	}

	int number = (int)GetPhysicalDriveFromPartitionLetter(txt.GetAt(0));
	driverTxt.Format("\\\\.\\PhysicalDrive%d",number);


	HANDLE hFile;

	hFile = CreateFile(driverTxt,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0, 
		NULL);

	assert(hFile && "CreateFile failed!");
	if(hFile == INVALID_HANDLE_VALUE)
	{
		printf("open driver d: error \n");
		printf("error number %d",GetLastError());
		AfxMessageBox("打开SD磁盘失败！");
		return -2;

	}

	_snprintf(file_name, PATH_MAX, "%s\\partition.img", m_path.GetBuffer(m_path.GetLength() + 1));
	fp = fopen(file_name, "rb");
	fread(&partition_image, 1, sizeof(partition_image), fp);
	fclose(fp);

	recordMap.clear();

	for(i=0;i<PART_END_NUM;i++) {
		recordMap[partition_image.item[i].start_sector] = i;
	}

	map<UINT64,int>::iterator it;
	for(it=recordMap.begin();it!=recordMap.end();++it) {
		trace("%u   %d\n",it->first,it->second);
		
		if (burn_sd(hFile, m_path.GetBuffer(m_path.GetLength()+1), it->second) != 0) {
			printf("burn sd error path : %s\n", m_path.GetBuffer(100));
			CloseHandle(hFile);
			return -3;
		}
	}
	
	if(write_head(hFile,m_path.GetBuffer(m_path.GetLength()+1)) == 0)
		AfxMessageBox("成功写入SD卡");

	this->SendMessage(WM_MYMESSAGE,2,100);

	CloseHandle(hFile);
	
	return 0;
}



void CDiskImageDlg::scanDisk()
{
	
	CHAR szLogicDriveStrings[BUFSIZE];
	PCHAR szDrive;

	ZeroMemory(szLogicDriveStrings,BUFSIZE);

	GetLogicalDriveStrings(BUFSIZE-1,szLogicDriveStrings);
	szDrive = (PCHAR)szLogicDriveStrings;

	do {
		if(GetDriverInfo(szDrive)) {
			m_driver_list.AddString(szDrive);

		}
		szDrive += (lstrlen(szDrive)+1);
	} while (*szDrive !='\x00');

	m_driver_list.SetCurSel(0);


}


BOOL GetDriverInfo(LPSTR szDrive)
{
	UINT uDriverType;
	int  flag = 0;

	uDriverType = GetDriveType(szDrive);

	switch(uDriverType)	{
	case DRIVE_UNKNOWN:
		printf("The driver type cannot be determined!");
		break;
	case DRIVE_NO_ROOT_DIR:
		printf("The root path is invalid,for example,no volume is mounted at the path");
		break;
	case DRIVE_REMOVABLE:
		flag = 1;
		printf("The drive is a type that has removable media,for example:a floppy drive or removable hard disk");
		break;
	case DRIVE_FIXED:
		printf("The drive is a type that cannot be removed, for example,a fixed hard drive");
		break;
	case DRIVE_REMOTE:
		printf("This drive is a remote(network) drive");
		break;
	case DRIVE_CDROM:
		printf("This drive is a CD-ROM drive.");
		break;
	case DRIVE_RAMDISK:
		printf("This drive is a RAM disk");
		break;
	default:
		break;
	}

	if(!flag) {
		return false;
	}

	return TRUE;

}


void CDiskImageDlg::read()
{

	HANDLE hFile;
	CString driverTxt,txt;
	int index;
	index = m_driver_list.GetCurSel(); 
	m_driver_list.GetLBText(index,txt);
	int number = (int)GetPhysicalDriveFromPartitionLetter(txt.GetAt(0));
	driverTxt.Format("\\\\.\\PhysicalDrive%d",number);

	hFile = CreateFile(driverTxt,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0, 
		NULL);

	PBYTE pBuffer = (PBYTE)malloc(512);
	int dwStartSector = 0;

	LARGE_INTEGER li;
	UpdateData(TRUE);
	dwStartSector = atoi(m_sector);
	


	for(int j =0;j<34;j++) {

	UINT64 off =  (dwStartSector+j) * 512;
	
	
	li.QuadPart = off;

	li.LowPart = SetFilePointer (hFile, 
		li.LowPart, 
		&li.HighPart, 
		FILE_BEGIN);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() 
		!= NO_ERROR)
	{
		li.QuadPart = -1;
		CloseHandle(hFile);
		printf("SetFilePointer error  off %u",dwStartSector);
		AfxMessageBox("文件指针partition移动失败！");
		return;
	}
		
	BOOL	bRet ;

		DWORD dwCB;  
		bRet = ReadFile(hFile, pBuffer, 512, &dwCB, NULL); 
		for(int i=0;i<512;i++)
		{
			trace("%2x  ",pBuffer[i]);

			if(i%10 == 0){
				trace("\n");
			}

		}

	}
	CloseHandle(hFile);
	free(pBuffer);

	

}
void CAboutDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDiskImageDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	read();
}

DWORD CDiskImageDlg::GetPhysicalDriveFromPartitionLetter(CHAR letter)

{

	HANDLE hDevice;										 // handle to the drive to be examined

	BOOL result;										 // results flag

	DWORD readed;										 // discard results

	STORAGE_DEVICE_NUMBER number;						//use this to get disk numbers

#define DISK_PATH_LEN  256

	CHAR path[DISK_PATH_LEN];

	sprintf(path, "\\\\.\\%c:", letter);

	hDevice = CreateFile(path,							// drive to open

		GENERIC_READ | GENERIC_WRITE,					// access to the drive

		FILE_SHARE_READ | FILE_SHARE_WRITE,				//share mode

		NULL,											// default security attributes

		OPEN_EXISTING,									// disposition

		0,												// file attributes

		NULL);											// do not copy file attribute

	if (hDevice == INVALID_HANDLE_VALUE)				// cannot open the drive

	{

		fprintf(stderr, "CreateFile() Error: %ld\n", GetLastError());

		return DWORD(-1);

	}



	result = DeviceIoControl(

		hDevice,											// handle to device

		IOCTL_STORAGE_GET_DEVICE_NUMBER,					// dwIoControlCode

		NULL,												// lpInBuffer

		0,													// nInBufferSize

		&number,											 // output buffer

		sizeof(number),										 // size of output buffer

		&readed,											 // number of bytes returned

		NULL												// OVERLAPPED structure

		);

	if (!result)											// fail

	{

		fprintf(stderr, "IOCTL_STORAGE_GET_DEVICE_NUMBER Error: %ld\n", GetLastError());

		(void)CloseHandle(hDevice);

		return (DWORD)-1;

	}

	printf("%d %d %d\n\n", number.DeviceType, number.DeviceNumber, number.PartitionNumber);



	(void)CloseHandle(hDevice);

	return number.DeviceNumber;

}

int CDiskImageDlg::erase_image(HANDLE hDev,int part,UINT64 earseSize,UINT64 startSector)
{

		const char *name = NULL;
		UINT8 buffer[SECTOR_SIZE] ={0};
		FILE *fp = NULL;
		UINT64 size = 0;
		UINT32 chunk = 0;
		UINT64 off = 0;
		UINT32 count = 0;
		int ret = 0;
		UINT64 file_size = 0;
		UINT64 percent = 0;
		DWORD dwCB; 
		BOOL bRet;
		file_size = earseSize;

		while((file_size - count * SECTOR_SIZE) >0) {

		off = SECTOR_SIZE * (startSector + count);

			
			LARGE_INTEGER li;

			li.QuadPart = off;

			li.LowPart = SetFilePointer (hDev, 
				li.LowPart, 
				&li.HighPart, 
				FILE_BEGIN);

			if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() 
				!= NO_ERROR)
			{
				li.QuadPart = -1;
				int result = GetLastError();
				printf("SetFilePointer error %d",GetLastError());
				AfxMessageBox("文件指针移动失败！");
				return -1;
			}
		
			PBYTE pBuffer = (PBYTE)buffer;
		
			chunk = SECTOR_SIZE;
			

			bRet = WriteFile(hDev, pBuffer, chunk, &dwCB, NULL); 
			if(!bRet || chunk!= (UINT64)dwCB){
				int result = GetLastError();
				printf("WriteFile error %d",GetLastError());
				AfxMessageBox("写SD卡失败!");
				return -1;
			}
			size += chunk;
			percent = size * 100  / file_size;
			if(percent >= 100)
				percent = 100;
			this->SendMessage(WM_MYMESSAGE,1,(int)percent);
			count++;
			memset(buffer,SECTOR_SIZE,0);

		}
		return 0;
}

ULONG CDiskImageDlg:: ThreadburnFunc(LPVOID pParam)
{
	CDiskImageDlg * pDlg = (CDiskImageDlg *)pParam;
	pDlg->burn_sd_one_image();
	return 0;

}

int CDiskImageDlg::write_head(HANDLE hFile,CString path)
{
	

	burn_mbr(hFile, path);

	burn_gpt(hFile, path);

	FlushFileBuffers(hFile);

	return 0;

}

int CDiskImageDlg::burn_sd_one_image()
{

	HANDLE hFile;
	CString driverTxt,txt;
	int index;
	int part;
	int i;
	CString prePath;
	CString szGetName;
	const char *name = NULL;
	char file_name[PATH_MAX];
	FILE *fp = NULL;

	CFileDialog *lpszOpenFile;    //定义一个CfileDialog对象
	lpszOpenFile = new CFileDialog(TRUE," ","",OFN_FILEMUSTEXIST |OFN_HIDEREADONLY , "文件类型(*.img)|*.img||*.bin");//生成一个对话框


	if(lpszOpenFile->DoModal()==IDOK)//点击对话框确定按钮,读文件
	{

		szGetName = lpszOpenFile->GetPathName();//得到保存文件的路径

		int pos = szGetName.ReverseFind('\\');
		CString name = szGetName.Right(szGetName.GetLength()-pos-1);
		prePath = szGetName.Left(pos) ;

		index = m_driver_list.GetCurSel(); 

		if(index == -1 )
		{
			AfxMessageBox("没有选择sd卡选择!");
			return -1 ;
		}

		m_driver_list.GetLBText(index,txt);
		driverTxt = CString("\\\\.\\");
		driverTxt +=  txt.Left(txt.GetLength()-1);

		int number = (int)GetPhysicalDriveFromPartitionLetter(txt.GetAt(0));
		driverTxt.Format("\\\\.\\PhysicalDrive%d",number);


		hFile = CreateFile(driverTxt,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0, 
			NULL);

		assert(hFile && "CreateFile failed!");
		if(hFile == INVALID_HANDLE_VALUE)
		{
			printf("open driver d: error \n");
			printf("error number %d",GetLastError());
			AfxMessageBox("打开SD磁盘失败！");
			return -1;

		}

		_snprintf(file_name, PATH_MAX, "%s\\partition.img", prePath.GetBuffer(prePath.GetLength() + 1));
		fp = fopen(file_name, "rb");
		fread(&partition_image, 1, sizeof(partition_image), fp);
		fclose(fp);

		for(i = 0;i < PART_END_NUM;i++)
		{
			if( strcmp(name.GetBuffer(name.GetLength()),partition_name[i]) == 0 ){

				part = i;
				break;
			}

		}
		if(i == PART_END_NUM) {
			AfxMessageBox("find image err\n");
			return -2;
		}

		write_head(hFile,prePath.GetBuffer(prePath.GetLength()+1));
	
		if (burn_sd(hFile, prePath.GetBuffer(prePath.GetLength()+1),part) != 0) {
			printf("burn image error path : %s  image: %s \n", prePath.GetBuffer(prePath.GetLength()),partition_name[i]);
			CloseHandle(hFile);
			return -1;
		}
		
		switch (part) {
			case  PART_TABLE:
			case  PART_MCU_PBL:
			case  PART_RTOS:
			case  PART_BL:
				burn_sd(hFile, prePath.GetBuffer(prePath.GetLength()+1),part+1);
				break;
			default:
				break;
		}
		this->SendMessage(WM_MYMESSAGE,2,100);

		if(hFile!=NULL)
			CloseHandle(hFile);

		AfxMessageBox("成功写入SD卡");

		return 0;


	}
	return -1;

}



void CDiskImageDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码


	hThread=CreateThread(NULL,0,ThreadburnFunc,(LPVOID)this,0,NULL);
	if(!hThread) {
		printf("create thread failed");
		return ;
	}
}
