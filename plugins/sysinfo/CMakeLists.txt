# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(sysinfo_probe_srcs
        sysinfo.cpp
        environmentmodel.cpp
        libraryinfomodel.cpp
        standardpathsmodel.cpp
        sysinfomodel.cpp
    )

    gammaray_add_plugin(
        gammaray_sysinfo_plugin
        JSON
        gammaray_sysinfo.json
        SOURCES
        ${sysinfo_probe_srcs}
    )
    target_link_libraries(gammaray_sysinfo_plugin gammaray_core)
    set_target_properties(gammaray_sysinfo_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(sysinfo_ui_srcs sysinfowidget.cpp standardpathswidget.cpp)

    gammaray_add_plugin(
        gammaray_sysinfo_ui_plugin
        JSON
        gammaray_sysinfo.json
        SOURCES
        ${sysinfo_ui_srcs}
    )
    target_link_libraries(gammaray_sysinfo_ui_plugin gammaray_ui)
endif()
