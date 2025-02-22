# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    gammaray_add_plugin(
        gammaray_translatorinspector
        JSON
        gammaray_translatorinspector.json
        SOURCES
        translatorinspector.cpp
        translatorinspectorinterface.cpp
        translatorwrapper.cpp
        translatorsmodel.cpp
    )
    target_link_libraries(gammaray_translatorinspector gammaray_core Qt::Core Qt::CorePrivate)
    set_target_properties(gammaray_translatorinspector PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    gammaray_add_plugin(
        gammaray_translatorinspector_ui
        JSON
        gammaray_translatorinspector.json
        SOURCES
        translatorinspectorwidget.cpp
        translatorinspectorinterface.cpp
        translatorwrapperproxy.cpp
    )
    target_link_libraries(gammaray_translatorinspector_ui gammaray_ui)
endif()
