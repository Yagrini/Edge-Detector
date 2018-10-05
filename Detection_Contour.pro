QT += widgets

FORMS += \
    moncontour.ui

HEADERS += \
    moncontour.h

SOURCES += \
    moncontour.cpp \
    main.cpp
INCLUDEPATH += "C:\opencv\opencv_build\include"
LIBS += -L "C:\opencv\opencv_build\lib" \
        -L "C:\opencv\opencv_build\lib/libopencv_core310.dll.a" \
        -L "C:\opencv\opencv_build\lib/libopencv_highgui310.dll.a" \
        -L "C:\opencv\opencv_build\lib/libopencv_ml310.dll.a" \
        -L "C:\opencv\opencv_build\lib/libopencv_imgproc310.dll.a" \
        -L "C:\opencv\opencv_build\lib/libopencv_video310.dll.a" \
        -L "C:\opencv\opencv_build\lib/libopencv_imgcodecs310.dll.a"
