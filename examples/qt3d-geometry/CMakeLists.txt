# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
add_executable(
    example-qt3d-geometry
    mycylinder.cpp qt3d-geometry.cpp
)
target_link_libraries(
    example-qt3d-geometry Qt::3DExtras
)
if(QNXNTO)
    target_link_libraries(example-qt3d-geometry m)
endif()
