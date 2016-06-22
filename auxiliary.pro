
CONFIG(debug, debug|release) {
	message("auxiliary_debug")

	TARGET = auxiliary_debug

	DESTDIR = $$_PRO_FILE_PWD_/../.dist
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../.int/auxiliary_debug

} else {
	message("auxiliary_release")

	TARGET = auxiliary

	DESTDIR = $$_PRO_FILE_PWD_/../.dist
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../.int/auxiliary_release
}

TEMPLATE = lib
CONFIG += staticlib precompile_header c++14
CONFIG -= qt
MAKEFILE = $$_PRO_FILE_PWD_/auxiliary.makefile

#-------------------------------------------------------------------------------------------------
# warnings
#-------------------------------------------------------------------------------------------------
QMAKE_CXXFLAGS_WARN_ON += \
	-Wno-parentheses \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-local-typedefs \
	-Wno-unused-but-set-variable \
	-Wno-sign-compare \
	-Wno-unused-function

#-------------------------------------------------------------------------------------------------
# compiler flags
#-------------------------------------------------------------------------------------------------
QMAKE_CXXFLAGS += \
	-m64 \
	-msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mf16c \
	-g \
	-fpic \
	-fdata-sections \
	-ffunction-sections \
	-fno-strict-aliasing \
	-I$$_PRO_FILE_PWD_/../auxiliary \
	-I$$_PRO_FILE_PWD_/platform/linux

PRECOMPILED_HEADER = $$_PRO_FILE_PWD_/platform/linux/platform.h

CONFIG(debug, debug|release) {
	DEFINES += _DEBUG DEBUG

} else {
	DEFINES += NDEBUG

	QMAKE_CFLAGS_RELEASE -= -O0 -O1 -O2
	QMAKE_CFLAGS_RELEASE *= -O3

	QMAKE_CXXFLAGS_RELEASE -= -O0 -O1 -O2
	QMAKE_CXXFLAGS_RELEASE *= -O3
}

#-------------------------------------------------------------------------------------------------
# files
#-------------------------------------------------------------------------------------------------
SOURCES += \
    auxiliary/FixedStream.cpp \
    auxiliary/VectorStream.cpp \
	google/farmhash/farmhash.cc

HEADERS += \
    auxiliary/ChunkedStorage.h \
    auxiliary/CompressedStorage.h \
    auxiliary/FixedArray.h \
    auxiliary/FixedStream.h \
    auxiliary/IStream.h \
    auxiliary/JsonPrinter.h \
    auxiliary/Miscellaneous.h \
    auxiliary/VectorStream.h \
    platform/linux/platform.h \
    auxiliary/Clock.h \
	google/farmhash/farmhash.h \
    auxiliary.h \
    auxiliary/FileSystemUtils.h
