QT += gui

win32 {

INCLUDEPATH += H:/opencv/build/include \
               H:/opencv/build/include/opencv

LIBS    += -LH:/opencv/build/x86/vc9/lib -lopencv_core245  -lopencv_highgui245 -lopencv_imgproc245 -lopencv_ml245 \
           -LH:/work/github/Demo/lib -ltoupcam -ldetect -lMVCAMSDK

SOURCES += comm/driver/uart/win/qextserialbase.cpp \
    comm/driver/uart/win/qextserialenumerator.cpp \
    comm/driver/uart/win/qextserialport.cpp \
    comm/driver/uart/win/win_qextserialport.cpp
HEADERS += \
    comm/driver/include/qextserialbase.h \
    comm/driver/include/qextserialenumerator.h \
    comm/driver/include/qextserialport.h \
    comm/driver/include/win_qextserialport.h \


}

unix{
LIBS += -lMVSDK -lopencv_core -lopencv_highgui -lrt  -lopencv_imgproc -lopencv_ml
}

INCLUDEPATH += ./include \
                video/include \
                video/Interface/include \
                comm/driver/Interface/include \
                comm/driver/include \
                comm/protocol/include \
                app \
                util/include \
                comm/service/include \
                handle/Interface/include


OTHER_FILES +=



SOURCES += \
    main.cpp \
    video/videorender.cpp \
    util/imageproc/color.cpp \
    video/capture/cvcaptureopr.cpp \
    video/datamanager.cpp \
    debugredirect.cpp \
    util/imageproc/imageopr.cpp \
    video/imageprocesser.cpp \
    video/capture/industrycaptureopr.cpp \
    comm/service/ioservice/ioboardservice.cpp \
    app/mainwindown.cpp \
    video/render/scrollrenderopr.cpp \
    video/algo/simplealgo.cpp \
    video/capture/toupcaptureopr.cpp \
    video/capture/v4l2captureopr.cpp \
    video/videocapture.cpp \
    video/videomanager.cpp \
    appconfig.cpp \
    app/argssetting.cpp \
    app/basedialog.cpp \
    video/cameraparameterdef.cpp \
    video/algo/ceramicdetectalgo.cpp \
    comm/driver/deviceoprmanager.cpp \
    comm/protocol/mudbus/master/mudbusmaster.cpp \
    comm/driver/uart/uarttransferopr.cpp \
    comm/driver/uart/linux/uart.cpp \
    video/render/simplerenderopr.cpp \
    util/crc/crcopr.cpp \
    handle/handlermanager.cpp \
    handle/demohandler.cpp


HEADERS += \
    color.h \
    video/include/datamanager.h \
    include/ioboardservice.h \
    include/imageopr.h \
    video/include/imageprocesser.h \
    include/industrycaptureopr.h \
    video/Interface/include/irenderopr.h \
    comm/driver/Interface/include/ITransferOpr.h \
    video/Interface/include/libdatadef.h \
    include/mainwindown.h \
    video/include/scrollrenderopr.h \
    video/include/simplealgo.h \
    video/include/simplerenderopr.h \
    include/toupcaptureopr.h \
    comm/driver/include/uart.h \
    comm/driver/include/uarttransferopr.h \
    include/v4l2captureopr.h \
    video/include/videocapture.h \
    video/include/videomanager.h \
    video/include/videorender.h \
    video/Interface/include/ialgorithm.h \
    include/argssetting.h \
    include/basedialog.h \
    include/appconfig.h \
    comm/service/include/ioboardservice.h \
    video/Interface/include/icaptureopr.h \
    video/include/cameraparameterdef.h \
    video/include/ceramicdetectalgo.h \
    comm/driver/include/deviceoprmanager.h \
    util/include/imageopr.h \
    util/include/color.h \
    app/mainwindown.h \
    app/basedialog.h \
    app/argssetting.h \
    comm/protocol/include/mudbusmaster.h \
    comm/protocol/include/mudbusdatadef.h \
    comm/protocol/include/mudbusdef.h \
    util/crc/crcopr.h \
    util/include/crcopr.h \
    video/include/CameraStatus.h \
    video/include/v4l2captureopr.h \
    handle/Interface/include/IHandler.h \
    handle/handlermanager.h \
    handle/demohandler.h \
    include/algargs.h


