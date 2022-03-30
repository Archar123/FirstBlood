#include <ntifs.h>
#include <windef.h>
#include "../common/common.h"
#include "kstrcut/Struct.h"
#include "version/SysVersion.h"

WCHAR szDeviceName[256] = {'\\','D','e','v','i','c','e','\0'};
WCHAR szDosName[256] = {'\\','D','o','s','D','e','v','i','c','e','s','\0'};

extern PSHORT NtBuildNumber;

NTSTATUS 
DispatchClose(
    IN PDEVICE_OBJECT		DeviceObject,
    IN PIRP					Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS 
DispatchCreate(
				IN PDEVICE_OBJECT		DeviceObject,
				IN PIRP					Irp
				)
{
	NTSTATUS status = STATUS_SUCCESS;
	/*PEPROCESS pCurrentEprocess = PsGetCurrentProcess();
	ULONG nDebugportOffset = GetGlobalVeriable(enumDebugportOffset_EPROCESS);
	if (pCurrentEprocess && nDebugportOffset)
	{
		ULONG DebugObject = *(PULONG)((ULONG)pCurrentEprocess + nDebugportOffset);
		if (DebugObject)
		{
			status = STATUS_INVALID_PARAMETER;
		}
	}*/

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS 
DispatchDeviceControl(
    IN PDEVICE_OBJECT		DeviceObject,
    IN PIRP					Irp
    )
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
	ULONG uIoControlCode = irpSp->Parameters.DeviceIoControl.IoControlCode;
	DWORD uInSize = irpSp->Parameters.DeviceIoControl.InputBufferLength;
	DWORD uOutSize = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
	DWORD dwRet = 0;

    switch(uIoControlCode)
    {
	case IOCTL_NEITHER_CONTROL:
		{
			PVOID pOutBuffer = Irp->UserBuffer;
			PVOID pInBuffer = irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
			//status = CommunicatNeitherControl(pInBuffer, uInSize, pOutBuffer, uOutSize, &dwRet);
		}
		break;

    default:
		KdPrint(("IOCTL Code Error.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = dwRet;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

VOID 
DriverUnload(
    IN PDRIVER_OBJECT	DriverObject
    )
{
    PDEVICE_OBJECT pdoNextDeviceObj = DriverObject->DeviceObject;
	UNICODE_STRING usSymlinkName;
	
	//UnInitProcessVariable();

	RtlInitUnicodeString(&usSymlinkName, szDosName);
    IoDeleteSymbolicLink(&usSymlinkName);
    while(pdoNextDeviceObj)
    {
        PDEVICE_OBJECT pdoThisDeviceObj = pdoNextDeviceObj;
        pdoNextDeviceObj = pdoThisDeviceObj->NextDevice;
        IoDeleteDevice(pdoThisDeviceObj);
    }
}

//
//					
//////////////////////////////////////////////////////////////////
extern "C"
NTSTATUS 
DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
    )
{
    PDEVICE_OBJECT pdoDeviceObj = 0;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING usSymlinkName, usDeviceName;
	WCHAR* szRegName = NULL;
	WCHAR *szRegPath = NULL;
	ULONG nLen = 0;

	KdPrint(("[FB]Enter DriverEntry\n"));
	if (!DriverObject || !RegistryPath)
	{
		return status;
	}

	nLen = RegistryPath->MaximumLength + sizeof(WCHAR);
	szRegPath = (WCHAR *)ExAllocatePoolWithTag(PagedPool, nLen, POOLTAG);
	if (szRegPath == NULL)
	{
		return status;
	}
	
	memset(szRegPath, 0, nLen);
	wcsncpy(szRegPath, RegistryPath->Buffer, RegistryPath->Length / sizeof(WCHAR));
	KdPrint(("[FB]szRegPath: %S\n", szRegPath));

	szRegName = wcsrchr(szRegPath, '\\');
	if (szRegName && szRegName[1] != 0 && wcslen(szRegName) < 200)
	{
		InitWindows(DriverObject);

		wcscat(szDeviceName, szRegName);
		wcscat(szDosName, szRegName);
 		KdPrint(("[FB]szRegName: %S, szDeviceName: %S, szDosName: %S\n", szRegName, szDeviceName, szDosName)); 

		RtlInitUnicodeString(&usDeviceName, szDeviceName);
		if(!NT_SUCCESS(status = IoCreateDevice(
			DriverObject,
			0,
			&usDeviceName,
			FILE_DEVICE_UNKNOWN,
			FILE_DEVICE_SECURE_OPEN,
			FALSE,
			&pdoDeviceObj
			)))
		{
			KdPrint(("[FB]IoCreateDevice Error\n"));
			ExFreePool(szRegPath);
			return status;
		}

		RtlInitUnicodeString(&usSymlinkName, szDosName);
		if(!NT_SUCCESS(status = IoCreateSymbolicLink(
			&usSymlinkName,
			&usDeviceName
			)))
		{
			KdPrint(("[FB]IoCreateSymbolicLink Error\n"));
			ExFreePool(szRegPath);
			IoDeleteDevice(pdoDeviceObj);
			return status;
		}

		DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
		DriverObject->DriverUnload = DriverUnload;

		status = STATUS_SUCCESS;
 	}

	ExFreePool(szRegPath);
    return status;
}