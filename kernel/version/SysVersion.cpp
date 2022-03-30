#include "SysVersion.h"
#include "kstrcut/Struct.h"

#define MAX_VERIABLE 3000
ULONG_PTR GlobalVeriable[MAX_VERIABLE] = { 0 };

KERNEL_OFFSET KernelOffset = {0};

VOID SetGlobalVeriable(VERIABLE_INDEX Index, ULONG_PTR Value)
{
	GlobalVeriable[Index] = Value;
}

ULONG_PTR GetGlobalVeriable(VERIABLE_INDEX Index)
{
	return GlobalVeriable[Index];
}

ULONG_PTR *GetGlobalVeriable_Address(VERIABLE_INDEX Index)
{
	return &GlobalVeriable[Index];
}

ULONG_PTR GetGlobalVeriableAddress(VERIABLE_INDEX Index)
{
	return ((ULONG_PTR)GlobalVeriable + Index * sizeof(ULONG_PTR));
}


//
PVOID
GetFunctionAddressByName(
	WCHAR *szFunction
)
{
	UNICODE_STRING unFunction;
	PVOID pAddr = NULL;

	if (szFunction)
	{
		RtlInitUnicodeString(&unFunction, szFunction);
		pAddr = MmGetSystemRoutineAddress(&unFunction);
	}

	return pAddr;
}


WIN_VERSION GetWindowsVersion()
{
	RTL_OSVERSIONINFOEXW osverinfo = { sizeof(osverinfo) };
	pfnRtlGetVersion pRtlGetVersion = NULL;
	WIN_VERSION WinVersion;
	WCHAR szRtlGetVersion[] = { 'R','t','l','G','e','t','V','e','r','s','i','o','n','\0' };

	pRtlGetVersion = (pfnRtlGetVersion)GetFunctionAddressByName(szRtlGetVersion);
	if (pRtlGetVersion)
	{
		pRtlGetVersion((PRTL_OSVERSIONINFOW)&osverinfo);
	}
	else
	{
		PsGetVersion(&osverinfo.dwMajorVersion, &osverinfo.dwMinorVersion, &osverinfo.dwBuildNumber, NULL);
	}

	KdPrint(("[FB]build number: %d\n", osverinfo.dwBuildNumber));
	SetGlobalVeriable(enumNtBuildNumber, (ULONG)(osverinfo.dwBuildNumber));

	if (osverinfo.dwMajorVersion == 5 && osverinfo.dwMinorVersion == 0)
	{
		KdPrint(("[FB]WINDOWS_2K\n"));
		WinVersion = enumWINDOWS_2K;
	}
	else if (osverinfo.dwMajorVersion == 5 && osverinfo.dwMinorVersion == 1)
	{
		KdPrint(("[FB]WINDOWS_xp\n"));
		WinVersion = enumWINDOWS_XP;
	}
	else if (osverinfo.dwMajorVersion == 5 && osverinfo.dwMinorVersion == 2)
	{
		if (osverinfo.wServicePackMajor == 0)
		{
			KdPrint(("[FB]WINDOWS_2K3\n"));
			WinVersion = enumWINDOWS_2K3;
		}
		else
		{
			KdPrint(("[FB]WINDOWS_2K3_sp1_sp2\n"));
			WinVersion = enumWINDOWS_2K3_SP1_SP2;
		}
	}
	else if (osverinfo.dwMajorVersion == 6 && osverinfo.dwMinorVersion == 0)
	{
		if (osverinfo.dwBuildNumber == 6000)
		{
			KdPrint(("[FB]WINDOWS_VISTA\n"));
			WinVersion = enumWINDOWS_VISTA;
		}
		else if (osverinfo.dwBuildNumber == 6001 || osverinfo.dwBuildNumber == 6002)
		{
			KdPrint(("[FB]WINDOWS_VISTA_SP1_SP2\n"));
			WinVersion = enumWINDOWS_VISTA_SP1_SP2;
		}
	}
	else if (osverinfo.dwMajorVersion == 6 && osverinfo.dwMinorVersion == 1)
	{
		KdPrint(("[FB]WINDOWS 7\n"));
		WinVersion = enumWINDOWS_7;
	}
	else if (osverinfo.dwMajorVersion == 6 && osverinfo.dwMinorVersion == 2)
	{
		KdPrint(("[FB]WINDOWS 8\n"));
		switch (osverinfo.dwBuildNumber)
		{
		case 8250:
		case 8400:
		case 9200:
			WinVersion = enumWINDOWS_8;
			break;
		}
	}
	else if (osverinfo.dwMajorVersion == 6 && osverinfo.dwMinorVersion == 3)
	{
		KdPrint(("[FB]WINDOWS 8.1\n"));
		WinVersion = enumWINDOWS_9;
	}
	else if (osverinfo.dwMajorVersion == 10 && osverinfo.dwMinorVersion == 0)
	{
		if (osverinfo.dwBuildNumber == 22000)
		{
			KdPrint(("[FB]WINDOWS 11\n"));
			WinVersion = enumWINDOWS_11;
		}
		else
		{
			KdPrint(("[FB]WINDOWS 10\n"));
			WinVersion = enumWINDOWS_10;
		}
	}
	else
	{
		KdPrint(("[FB]WINDOWS_UNKNOW\n"));
		WinVersion = enumWINDOWS_UNKNOW;
	}

	return WinVersion;
}

void InitGlobalVariable()
{
	WIN_VERSION WinVersion = (WIN_VERSION)GetGlobalVeriable(enumWindowsVersion);

	ULONG_PTR dwNtBuildNumber = GetGlobalVeriable(enumNtBuildNumber);
	switch (dwNtBuildNumber)
	{
	case 0x47BA:	//18362
	case 0x47BB:
	{
		KernelOffset.Kthread.ContextSwitches = 0x154;
		KernelOffset.Kthread.State = 0x184;
		KernelOffset.Kthread.Win32StartAddress = 0x6a0;
		KernelOffset.Kthread.SuspendCount = 0x284;

		KernelOffset.Eprocess.ObjectTable = 0x418; /*句柄表偏移 位置*/
		KernelOffset.Eprocess.VadRoot = 0x658;
		KernelOffset.Eprocess.VadHint = 0x660;

		SetGlobalVeriable(enumObjectCallbackListOffset, 0xc8);
	}
	case 0x4A62:	//19042
		KernelOffset.Kthread.ContextSwitches = 0x154;
		KernelOffset.Kthread.State = 0x184;
		KernelOffset.Kthread.Win32StartAddress = 0x4d0;
		KernelOffset.Kthread.SuspendCount = 0x284;

		KernelOffset.Eprocess.ObjectTable = 0x570; /*句柄表偏移 位置*/
		KernelOffset.Eprocess.VadRoot = 0x7d8;
		KernelOffset.Eprocess.VadHint = 0x7e0;

		SetGlobalVeriable(enumObjectCallbackListOffset, 0xc8);
		break;

	default:
		break;
	}
}
////

BOOL GetKernelLdrDataTableEntry(PDRIVER_OBJECT DriverObject)
{
	BOOL bRet = FALSE;
	if (DriverObject)
	{
		PKLDR_DATA_TABLE_ENTRY entry = NULL, firstentry = NULL;
		WCHAR szNtoskrnl[] = { 'n','t','o','s','k','r','n','l','.','e','x','e','\0' };
		int nLen = wcslen(szNtoskrnl) * sizeof(WCHAR);

		firstentry = entry = (PKLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
		while ((PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry)
		{
			KdPrint(("[FB]BaseDllName: %wZ, FullDllName: %wZ\n", &(entry->BaseDllName), &(entry->FullDllName)));

			if (entry->BaseDllName.Buffer &&
				nLen == entry->BaseDllName.Length &&
				MmIsAddressValid((PVOID)entry->BaseDllName.Buffer) &&
				MmIsAddressValid((PVOID)&entry->BaseDllName.Buffer[entry->BaseDllName.Length / sizeof(WCHAR) - 1]) &&
				!_wcsnicmp(szNtoskrnl, entry->BaseDllName.Buffer, nLen / sizeof(WCHAR)))
			{
				SetGlobalVeriable(enumNtoskrnl_KLDR_DATA_TABLE_ENTRY, (ULONG_PTR)entry);
				bRet = TRUE;
				break;
			}

			entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////
BOOL InitWindows(PDRIVER_OBJECT DriverObject)
{
	if (!DriverObject)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	WIN_VERSION WinVersion = enumWINDOWS_UNKNOW;
	memset(&GlobalVeriable, 0, MAX_VERIABLE * sizeof(ULONG_PTR));

	WinVersion = GetWindowsVersion();
	if (WinVersion != enumWINDOWS_UNKNOW && GetKernelLdrDataTableEntry(DriverObject))
	{
		//初始化系统的一些结构体偏移
		InitGlobalVariable();
	}

	return TRUE;
}

NTSTATUS CheckUpdate()
{
	return STATUS_SUCCESS;
}

