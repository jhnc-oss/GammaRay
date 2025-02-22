# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
if(Qt5Quick_VERSION GREATER 5.5)
    add_subdirectory(quick-batching)
    add_subdirectory(quick-event-handling)
    add_subdirectory(state-machine)
endif()

if(TARGET Qt::Widgets)
    add_subdirectory(signal-slot)
    add_subdirectory(timer)
    add_subdirectory(widget-layouting)
endif()

if(Qt${QT_VERSION_MAJOR}3DExtras_FOUND)
    add_subdirectory(qt3d-geometry)
endif()
