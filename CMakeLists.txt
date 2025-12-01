# 指定 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.16)

# 定义项目名称、版本号和使用的语言
project(TimestampHotkey VERSION 0.1 LANGUAGES CXX)


# 启用 Qt 的自动处理功能：
# AUTOUIC：自动处理 .ui 文件
# AUTOMOC：自动处理 Qt 的 MOC（元对象编译器）
# AUTORCC：自动处理 Qt 的资源文件 (.qrc)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准为 C++17，并且强制要求支持
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 库，优先使用 Qt6，如果没有则使用 Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 定义项目的源文件列表
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# 如果使用的是 Qt6 及以上版本
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)









    # 使用 Qt6 的专用函数 qt_add_executable 创建可执行文件
    qt_add_executable(TimestampHotkey
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        addons/QHotkey/qhotkey.cpp
        addons/QHotkey/qhotkey.h
        addons/QHotkey/qhotkey_p.h
        addons/QHotkey/qhotkey_win.cpp
    )

    # 添加 qhotkey 的头文件目录到 TimestampHotkey 的 include 路径
    target_include_directories(TimestampHotkey PUBLIC ${CMAKE_SOURCE_DIR}/addons/qhotkey)






# 如果是 Android 平台，可以设置额外的包目录
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TimestampHotkey APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

else()
    # 如果是 Qt5 且在 Android 平台上
    if(ANDROID)
        # 创建共享库（Android 应用需要）
        add_library(TimestampHotkey SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
# 设置 Android 包目录
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        # 在非 Android 平台上，使用普通的 add_executable 创建可执行文件
        add_executable(TimestampHotkey
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 链接 Qt Widgets 库
target_link_libraries(TimestampHotkey PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)





# 针对 macOS/iOS 的 bundle 配置

# Qt6.1 之后会自动设置 MACOSX_BUNDLE_GUI_IDENTIFIER
# 如果 Qt 版本小于 6.1，需要手动设置
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TimestampHotkey)
endif()

# 设置应用的属性（版本号、bundle 配置等）
set_target_properties(TimestampHotkey PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装规则：定义安装路径
include(GNUInstallDirs)
install(TARGETS TimestampHotkey
    BUNDLE DESTINATION .                     # macOS/iOS bundle 安装路径
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 库文件安装路径
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # 可执行文件安装路径
)

# 如果是 Qt6，需要调用 qt_finalize_executable 完成构建
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TimestampHotkey)
endif()
