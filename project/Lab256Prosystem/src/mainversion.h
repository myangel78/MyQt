#pragma once


#define SOFTWARE_VERSION		"V3.6.1.6"
#define SOFTWARE_NAME			"Lab256_Pro"
#define HARDWARE_VERSION		" VHARDWARE_VERSION "
#define COPYRIGHT				"Copyleft 2020-2021, BGI_LAB256_PRO"

#define _STR_(s)				#s
#define __STR(s)				_STR_(s)

#ifdef Q_OS_WIN32
#define BUILD_VERSION			_STR_(Qt_5_14_0_MSVC2019_64bit)
#else
#define BUILD_VERSION			_STR_(Qt_5_14_0_GCC10_2_64bit)
#endif

#ifdef QT_NO_DEBUG
#define SOFTWARE_TITLE      (SOFTWARE_NAME HARDWARE_VERSION SOFTWARE_VERSION)
#else
#define SOFTWARE_TITLE      (SOFTWARE_NAME HARDWARE_VERSION SOFTWARE_VERSION " (Build " BUILD_VERSION "-Debug)")
#endif

