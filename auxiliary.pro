
QT       -= core gui

CONFIG(debug, debug|release) {
	TARGET = auxiliary_debug
} else {
	TARGET = auxiliary_release
}

TEMPLATE = lib
CONFIG += staticlib c++14

QMAKE_CXXFLAGS_WARN_ON += \
	-Wno-parentheses \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-local-typedefs \
	-Wno-unused-but-set-variable \
	-Wno-sign-compare \
	-Wno-unused-function

QMAKE_CXXFLAGS += \
	-m64 \
	-msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mf16c \
	-fpic \
	-fdata-sections \
	-ffunction-sections \
	-I$$_PRO_FILE_PWD_/platform/linux \
	-I$$_PRO_FILE_PWD_/../boost

CONFIG(debug, debug|release) {
	message("auxiliary_debug")

	DESTDIR = $$_PRO_FILE_PWD_/../.dist
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../.int/auxiliary_debug

	DEFINES += _DEBUG DEBUG

} else {
	message("auxiliary_release")

	DESTDIR = $$_PRO_FILE_PWD_/../.dist
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../.int/auxiliary_release

	DEFINES += NDEBUG

	QMAKE_CFLAGS_RELEASE -= -O0 -O1 -O2
	QMAKE_CFLAGS_RELEASE *= -O3

	QMAKE_CXXFLAGS_RELEASE -= -O0 -O1 -O2
	QMAKE_CXXFLAGS_RELEASE *= -O3
}

SOURCES += \
    auxiliary/FixedStream.cpp \
    auxiliary/VectorStream.cpp \
    ../boost/libs/system/src/error_code.cpp \
    ../boost/libs/filesystem/src/codecvt_error_category.cpp \
    ../boost/libs/filesystem/src/operations.cpp \
    ../boost/libs/filesystem/src/path_traits.cpp \
    ../boost/libs/filesystem/src/path.cpp \
    ../boost/libs/filesystem/src/portability.cpp \
    ../boost/libs/filesystem/src/unique_path.cpp \
    ../boost/libs/filesystem/src/utf8_codecvt_facet.cpp

HEADERS += \
    auxiliary/ChunkedStorage.h \
    auxiliary/CompressedStorage.h \
    auxiliary/FixedArray.h \
    auxiliary/FixedStream.h \
    auxiliary/IStream.h \
    auxiliary/JsonPrinter.h \
    auxiliary/Miscellaneous.h \
    auxiliary/VectorStream.h \
    platform/linux/platform.h
