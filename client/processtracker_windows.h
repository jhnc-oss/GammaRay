/*
  processtracker_windows.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROCESSTRACKER_WINDOWS_H
#define GAMMARAY_PROCESSTRACKER_WINDOWS_H

#include "processtracker.h"

namespace GammaRay {

class GAMMARAY_CLIENT_EXPORT ProcessTrackerBackendWindows : public ProcessTrackerBackend
{
    Q_OBJECT

public:
    explicit ProcessTrackerBackendWindows(QObject *parent = nullptr);

public slots:
    void checkProcess(qint64 pid) override;
};

}

#endif // GAMMARAY_PROCESSTRACKER_WINDOWS_H
