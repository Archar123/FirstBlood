#pragma once
#include <ntifs.h>
#include <windef.h>

//only support above win7
typedef enum WIN_VERSION {
	enumWINDOWS_UNKNOW,
	enumWINDOWS_2K,
	enumWINDOWS_XP,
	enumWINDOWS_2K3,
	enumWINDOWS_2K3_SP1_SP2,
	enumWINDOWS_VISTA,
	enumWINDOWS_VISTA_SP1_SP2,
	//start support
	enumWINDOWS_7,
	enumWINDOWS_8,
	enumWINDOWS_9,	//win8.1
	enumWINDOWS_10,
	enumWINDOWS_11,
} WIN_VERSION;

//ϵͳ�в�����Ϣ��ƫ�Ƶ�
typedef enum _VERIABLE_INDEX_
{
	enumNtBuildNumber,
	enumWindowsVersion,
	enumNtoskrnl_KLDR_DATA_TABLE_ENTRY,
	enumObjectCallbackListOffset,


}VERIABLE_INDEX;

//// ϵͳ�����ṹ����
// EPROCESS�ṹ�����ƫ��
typedef struct _EPROCESS_OFFSET
{
	ULONG UniqueProcessId;		// ����pid
	ULONG CreateTime;			// CreateTimeƫ��
	ULONG DebugPort;			// DebugPortƫ��
	ULONG SectionObject;		// SectionObject
	ULONG ObjectTable;			// ObjectTableƫ��
	ULONG VadRoot;
	ULONG VadHint;
} EPROCESS_OFFSET, *PEPROCESS_OFFSET;

// KPROCESS�ṹ�����ƫ��
typedef struct _KPROCESS_OFFSET
{
	ULONG Reserve;

} KPROCESS_OFFSET, *PKPROCESS_OFFSET;

// ETHREAD�ṹ�����ƫ��
typedef struct _ETHREAD_OFFSET
{
	ULONG Reserve;

} ETHREAD_OFFSET, *PETHREAD_OFFSET;

// KTHREAD�ṹ�����ƫ��
typedef struct _KTHREAD_OFFSET
{
	ULONG ContextSwitches;
	ULONG State;
	ULONG Win32StartAddress;
	ULONG SuspendCount;
} KTHREAD_OFFSET, *PKTHREAD_OFFSET;

// �ں�ƫ��Ӳ����
typedef struct _KERNEL_OFFSET
{
	EPROCESS_OFFSET Eprocess;
	KPROCESS_OFFSET Kprocess;
	ETHREAD_OFFSET Ethread;
	KTHREAD_OFFSET Kthread;
} KERNEL_OFFSET, *PKERNEL_OFFSET;

VOID SetGlobalVeriable(VERIABLE_INDEX Index, ULONG_PTR Value);
ULONG_PTR GetGlobalVeriable(VERIABLE_INDEX Index);
ULONG_PTR GetGlobalVeriableAddress(VERIABLE_INDEX Index);
ULONG_PTR *GetGlobalVeriable_Address(VERIABLE_INDEX Index);

BOOL InitWindows(PDRIVER_OBJECT DriverObject);
NTSTATUS CheckUpdate();