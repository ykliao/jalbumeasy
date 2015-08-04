TARGET    = AlbumEasy
TEMPLATE  = app
CONFIG   += qt
QT       += widgets     #5.1

RESOURCES     += AlbumEasyRes.qrc
win32:RC_FILE += AlbumEasyWndRes.rc


   # By default qmake uses separate output directories for debug and release builds under MSYS/Windows.
   # For some reason it doesn't do that on LINUX so set it here to always use seperate directories
   # and also enable the console for debug builds

CONFIG(release,debug|release) {
BUILD_DIR=$$OUT_PWD/release
}

CONFIG(debug,debug|release) {
BUILD_DIR=$$OUT_PWD/debug
CONFIG        +=console
}

DESTDIR     =$$BUILD_DIR
OBJECTS_DIR =$$BUILD_DIR
MOC_DIR     =$$BUILD_DIR
RCC_DIR     =$$BUILD_DIR
UI_DIR      =$$BUILD_DIR



SOURCES = AlbumEasy.cpp                               \
          aeasy_mainwindow.cpp                        \
          aeasy_helpwindow.cpp                        \
          aeasy_parse.cpp                             \
          aeasy_album.cpp                             \
          aeasy_flistwindow.cpp                       \
          aeasy_config.cpp                            \
          aeasy_fonts.cpp                             \
          libhpdf-2.3.0RC2/src/hpdf_3dmeasure.c       \
          libhpdf-2.3.0RC2/src/hpdf_annotation.c      \
          libhpdf-2.3.0RC2/src/hpdf_array.c           \
          libhpdf-2.3.0RC2/src/hpdf_binary.c          \
          libhpdf-2.3.0RC2/src/hpdf_boolean.c         \
          libhpdf-2.3.0RC2/src/hpdf_catalog.c         \
          libhpdf-2.3.0RC2/src/hpdf_destination.c     \
          libhpdf-2.3.0RC2/src/hpdf_dict.c            \
          libhpdf-2.3.0RC2/src/hpdf_doc.c             \
          libhpdf-2.3.0RC2/src/hpdf_doc_png.c         \
          libhpdf-2.3.0RC2/src/hpdf_encoder.c         \
          libhpdf-2.3.0RC2/src/hpdf_encoder_cns.c     \
          libhpdf-2.3.0RC2/src/hpdf_encoder_cnt.c     \
          libhpdf-2.3.0RC2/src/hpdf_encoder_jp.c      \
          libhpdf-2.3.0RC2/src/hpdf_encoder_kr.c      \
          libhpdf-2.3.0RC2/src/hpdf_encoder_utf.c     \
          libhpdf-2.3.0RC2/src/hpdf_encrypt.c         \
          libhpdf-2.3.0RC2/src/hpdf_encryptdict.c     \
          libhpdf-2.3.0RC2/src/hpdf_error.c           \
          libhpdf-2.3.0RC2/src/hpdf_exdata.c          \
          libhpdf-2.3.0RC2/src/hpdf_ext_gstate.c      \
          libhpdf-2.3.0RC2/src/hpdf_font.c            \
          libhpdf-2.3.0RC2/src/hpdf_font_cid.c        \
          libhpdf-2.3.0RC2/src/hpdf_font_tt.c         \
          libhpdf-2.3.0RC2/src/hpdf_font_type1.c      \
          libhpdf-2.3.0RC2/src/hpdf_fontdef.c         \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_base14.c  \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_cid.c     \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_cns.c     \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_cnt.c     \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_jp.c      \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_kr.c      \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_tt.c      \
          libhpdf-2.3.0RC2/src/hpdf_fontdef_type1.c   \
          libhpdf-2.3.0RC2/src/hpdf_gstate.c          \
          libhpdf-2.3.0RC2/src/hpdf_image.c           \
          libhpdf-2.3.0RC2/src/hpdf_image_ccitt.c     \
          libhpdf-2.3.0RC2/src/hpdf_image_png.c       \
          libhpdf-2.3.0RC2/src/hpdf_info.c            \
          libhpdf-2.3.0RC2/src/hpdf_list.c            \
          libhpdf-2.3.0RC2/src/hpdf_mmgr.c            \
          libhpdf-2.3.0RC2/src/hpdf_name.c            \
          libhpdf-2.3.0RC2/src/hpdf_namedict.c        \
          libhpdf-2.3.0RC2/src/hpdf_null.c            \
          libhpdf-2.3.0RC2/src/hpdf_number.c          \
          libhpdf-2.3.0RC2/src/hpdf_objects.c         \
          libhpdf-2.3.0RC2/src/hpdf_outline.c         \
          libhpdf-2.3.0RC2/src/hpdf_page_label.c      \
          libhpdf-2.3.0RC2/src/hpdf_page_operator.c   \
          libhpdf-2.3.0RC2/src/hpdf_pages.c           \
          libhpdf-2.3.0RC2/src/hpdf_pdfa.c            \
          libhpdf-2.3.0RC2/src/hpdf_real.c            \
          libhpdf-2.3.0RC2/src/hpdf_streams.c         \
          libhpdf-2.3.0RC2/src/hpdf_string.c          \
          libhpdf-2.3.0RC2/src/hpdf_u3d.c             \
          libhpdf-2.3.0RC2/src/hpdf_utils.c           \
          libhpdf-2.3.0RC2/src/hpdf_xref.c

HEADERS = AlbumEasy.h           \
          aeasy_mainwindow.h    \
          aeasy_helpwindow.h    \
          aeasy_parse.h         \
          aeasy_album.h         \
          aeasy_flistwindow.h   \
          aeasy_config.h        \
          aeasy_fonts.h         \
          aeasy_ttf_structs.h

QMAKE_CXXFLAGS += -Wall

INCLUDEPATH += ./libhpdf-2.3.0RC2/include

win32 {
LIBS        += libpng \
                 libz
}

unix {
LIBS        += -lpng \
                 -lz
}

macx {
INCPATH += /usr/local/include
LIBS += -L/usr/local/lib
}

