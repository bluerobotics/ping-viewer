QMAKE_LIBDIR = $$absolute_path($$PWD/fmt)

HEADERS += $$files($$QMAKE_LIBDIR/include/fmt/*.h)

SOURCES += $$files($$QMAKE_LIBDIR/src/*.cc)

INCLUDEPATH += $$QMAKE_LIBDIR/include/
