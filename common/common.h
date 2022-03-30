#ifndef _COMMON_H_
#define _COMMON_H_

#define DEVICE_NAME			L"\\Device\\FirstBlood"
#define SYMLINK_NAME		L"\\DosDevices\\FirstBlood"
#define WIN32_DEVICE_NAME	L"\\\\.\\FirstBlood"

#define IOCTL_BASE          0x800

#define MY_NEITHER_CTL_CODE(i)		\
		CTL_CODE					\
		(							\
			FILE_DEVICE_UNKNOWN,	\
			IOCTL_BASE + i + 0x100,	\
			METHOD_NEITHER,			\
			FILE_ANY_ACCESS			\
		)

#define IOCTL_NEITHER_CONTROL    MY_NEITHER_CTL_CODE(1)

typedef enum _OPERATE_TYPE_
{
	enumUnKnow = 0,
	enumListProcess,	
	
} OPERATE_TYPE;

#endif