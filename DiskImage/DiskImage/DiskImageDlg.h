// DiskImageDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDiskImageDlg �Ի���
class CDiskImageDlg : public CDialog
{
// ����
public:
	CDiskImageDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DISKIMAGE_DIALOG };

	protected:

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	LONG OnMyMessage(WPARAM wParam, LPARAM lParam);   


// ʵ��
protected:
	HICON m_hIcon;
	HANDLE hThread;
	DWORD ThreadID;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CArray<CString, CString> m_file_array;
	CString m_path;
	CListBox m_ListBox;
	afx_msg void OnBnClickedOk();
	int writeToSD();

	int burn_sd(HANDLE hDev, const char *path, int part);
	int burn_sd_one_image();
	static ULONG __stdcall ThreadFunc(LPVOID pParam);
	static ULONG __stdcall ThreadburnFunc(LPVOID pParam);
	void scanDisk();
	int testWriteDisk(CString driverTxt);
	CComboBox m_driver_list;
	afx_msg void OnCbnSelchangeCombo1();
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedButton2();
	void read();
	int burn_mbr(HANDLE hDev,const char *path);
	int burn_gpt(HANDLE hDev,const char *path);
	afx_msg void OnBnClickedButton3();
	DWORD GetPhysicalDriveFromPartitionLetter(CHAR letter);
	CString m_sector;
	afx_msg void OnBnClickedButton4();
	int write_head(HANDLE hFile,CString path);
	int erase_image(HANDLE hDev,int part,UINT64 earseSize,UINT64 startSector);
	CString m_pathfile;
	CString mSatus;
	CString mStatus;
	CString mSpeed;
};
