# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_mimetypes_srcs mimetypes.cpp mimetypesmodel.cpp)
    gammaray_add_plugin(
        gammaray_mimetypes
        JSON
        gammaray_mimetypes.json
        SOURCES
        ${gammaray_mimetypes_srcs}
    )
    target_link_libraries(gammaray_mimetypes gammaray_core Qt::Gui gammaray_kitemmodels)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_mimetypes_ui_srcs mimetypeswidget.cpp)
    gammaray_add_plugin(
        gammaray_mimetypes_ui
        JSON
        gammaray_mimetypes.json
        SOURCES
        ${gammaray_mimetypes_ui_srcs}
    )
    target_link_libraries(gammaray_mimetypes_ui gammaray_ui)
endif()
