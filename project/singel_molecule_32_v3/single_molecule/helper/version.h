#ifndef __VERSION_H
#define __VERSION_H

#define MAIN_VERSION       1.1.6

#define SOFTWARE_NAME       "Single_Molecule_32"
#define HARDWARE_VERSION    " V3"
#define COPYRIGHT           "Copyleft 2020-2021, BGI_SINGEL_MOLECULE"

#define _STR_(s)            #s
#define __STR(s)            _STR_(s)

#define BUILD_VERSION       _STR_(Qt_5_14_0_MSVC2017_64bit)
#define SOFTWARE_VERSION    __STR(MAIN_VERSION)

#if QT_NO_DEBUG
#define SOFTWARE_TITLE      (SOFTWARE_NAME HARDWARE_VERSION " V" SOFTWARE_VERSION)
#else
#define SOFTWARE_TITLE      (SOFTWARE_NAME HARDWARE_VERSION " V" SOFTWARE_VERSION \
                            " (Build " BUILD_VERSION "-Debug)")
#endif

#endif
