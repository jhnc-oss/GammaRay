/*
  connectpage.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "connectpage.h"
#include "ui_connectpage.h"

#include <launcher/core/networkdiscoverymodel.h>
#include <launcher/core/clientlauncher.h>

#include <common/endpoint.h>

#include <QAction>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>

#include <qplatformdefs.h>

#ifdef Q_OS_LINUX
#include <sys/stat.h>
#endif

using namespace GammaRay;

ConnectPage::ConnectPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectPage)
    , m_valid(false)
{
    ui->setupUi(this);
    m_implicitPortWarningSign =
        new QAction(QIcon(":/launcher/warning.png"), tr("No port given, will use 11732"), this);

    m_fileIsNotASocketWarning =
        new QAction(qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical),
                    tr("File is not a socket"), this);
    connect(ui->host, &QLineEdit::textChanged, this, &ConnectPage::validateHostAddress);
    connect(ui->host, &QLineEdit::textChanged, this, &ConnectPage::updateButtonState);

    auto *model = new NetworkDiscoveryModel(this);
    ui->instanceView->setModel(model);
    connect(ui->instanceView->selectionModel(),
            &QItemSelectionModel::selectionChanged, this, &ConnectPage::instanceSelected);
    connect(ui->instanceView, &QAbstractItemView::activated, this, &ConnectPage::activate);

    QSettings settings;
    ui->host->setText(settings.value(QStringLiteral("Connect/Url"), QString()).toString());
}

const QString ConnectPage::localPrefix = QStringLiteral("local://");
const QString ConnectPage::tcpPrefix = QStringLiteral("tcp://");

void ConnectPage::validateHostAddress(const QString &address)
{
    QString stillToParse = address;
    m_valid = false;
    bool correctSoFar = false;
    m_currentUrl.clear();

    // Initially, set the text to red
    QPalette errorPalette;
    errorPalette.setColor(QPalette::Text, Qt::red);
    ui->host->setPalette(errorPalette);
    clearWarnings();

    handleLocalAddress(stillToParse, correctSoFar);
    handleIPAddress(stillToParse, correctSoFar);

    QRegularExpression re(QStringLiteral("^([a-zA-Z][a-zA-Z0-9\\-\\.]+[a-zA-Z0-9](:[0-9]{1,5})?)$"));
    if (re.match(stillToParse).hasMatch())
        handleHostName(stillToParse);

    // if we came down here and there's nothing more to parse, we are good
    // set text back to black again
    if (correctSoFar && stillToParse.isEmpty()) {
        m_valid = true;
        ui->host->setPalette(this->style()->standardPalette());
    }
    emit userInputParsed();
}

void ConnectPage::handleLocalAddress(QString &stillToParse, bool &correctSoFar)
{
#ifdef Q_OS_UNIX
    if (stillToParse.startsWith(localPrefix))
        stillToParse.remove(localPrefix); // don't remove second slash

    // It's okay if only a path to an existing file is given
    QFileInfo localSocketFile(stillToParse);
    if (localSocketFile.exists() && !localSocketFile.isDir() && !localSocketFile.isSymLink()) {
        QT_STATBUF statbuf;
        if (QT_STAT(QFile::encodeName(localSocketFile.filePath()), &statbuf) == 0) {
            if (!S_ISSOCK(statbuf.st_mode)) {
                showFileIsNotSocketWarning();
            } else {
                stillToParse = "";
                correctSoFar = true;
                m_currentUrl.setScheme("local");
                m_currentUrl.setPath(localSocketFile.filePath());
            }
        }
    }
#else
    Q_UNUSED(stillToParse);
    Q_UNUSED(correctSoFar);
#endif
}

void ConnectPage::handleIPAddress(QString &stillToParse, bool &correctSoFar)
{
    // handle tcp prefix
    if (stillToParse.startsWith(tcpPrefix))
        stillToParse.remove(0, tcpPrefix.size());

    // Speculate on the address format
    const auto possibleIPv4Address = QHostAddress(stillToParse.split(QLatin1Char(':')).first());

    QHostAddress possibleIPv6Address;
    if (!stillToParse.contains(QLatin1Char('%')))
        possibleIPv6Address = QHostAddress(stillToParse);

    QHostAddress possibleIPv6BracketAddress;
    QRegularExpression bracketFormatRE(QStringLiteral(R"(^\[([0-9a-f\:\.]*)\].*$)"));
    QRegularExpressionMatch bracketMatch = bracketFormatRE.match(stillToParse);
    if (bracketMatch.hasMatch())
        possibleIPv6BracketAddress = QHostAddress(bracketMatch.captured(1));

    QHostAddress possibleIPv6InterfaceAddress;
    QRegularExpression interfaceFormatRE(QStringLiteral(R"(^([^\%]*)(\%[^\:]+)(:[0-9]+)?$)"));
    QRegularExpressionMatch interfaceMatch = interfaceFormatRE.match(stillToParse);
    if (interfaceMatch.hasMatch())
        possibleIPv6InterfaceAddress = QHostAddress(interfaceMatch.captured(1));

    const auto skipPort = true;
    if (!possibleIPv4Address.isNull()) {
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv4Address.toString());
    }

    if (!possibleIPv6Address.isNull()) {
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv6Address.toString(), skipPort);
    }

    if (!possibleIPv6BracketAddress.isNull()) {
        handleAddressAndPort(stillToParse, correctSoFar,
                             QLatin1Char('[') + possibleIPv6BracketAddress.toString() + QLatin1Char(']'));
    }

    if (!possibleIPv6InterfaceAddress.isNull()) {
        stillToParse.replace(interfaceMatch.captured(2), QString());
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv6InterfaceAddress.toString());
    }
}

void ConnectPage::handleHostName(QString &stillToParse)
{
    // handle tcp prefix
    if (stillToParse.startsWith(tcpPrefix)) {
        stillToParse.remove(0, tcpPrefix.size());
    }
    m_currentUrl.setScheme("tcp");

    // cut off port first and handle port
    auto portStart = stillToParse.indexOf(QLatin1Char(':'));
    bool portCorrectSoFar = true;
    if (portStart > -1) {
        auto portString = stillToParse.right(portStart);
        handlePortString(portString, portCorrectSoFar);
        stillToParse = stillToParse.left(portStart);
    } else {
        showStandardPortAssumedWarning();
    }

    // don't do lookup if port was wrong
    if (!portCorrectSoFar)
        return;

    QHostInfo::lookupHost(stillToParse, this, SLOT(hostResponse(QHostInfo)));
}

void ConnectPage::hostResponse(const QHostInfo &hostInfo)
{
    if (hostInfo.error() != QHostInfo::NoError)
        return;

    if (hostInfo.addresses().empty())
        return;

    m_currentUrl.setHost(hostInfo.hostName());
    m_valid = true;
    ui->host->setPalette(this->style()->standardPalette());
    emit dnsResolved();
    emit updateButtonState();
}

void ConnectPage::handleAddressAndPort(QString &stillToParse, bool &correctSoFar, const QString &possibleAddress, bool skipPort)
{
    stillToParse.replace(possibleAddress, QString());
    if (stillToParse.isEmpty()) {
        correctSoFar = true;
        m_currentUrl.setScheme("tcp");
        m_currentUrl.setHost(possibleAddress);
        m_currentUrl.setPort(Endpoint::defaultPort());
        showStandardPortAssumedWarning();
    } else if (!skipPort) {
        m_currentUrl.setScheme("tcp");
        m_currentUrl.setHost(possibleAddress);
        handlePortString(stillToParse, correctSoFar);
    }
}

void ConnectPage::handlePortString(QString &stillToParse, bool &correctSoFar)
{
    QRegularExpression re(QStringLiteral("\\:[0-9]{1,5}"));
    QRegularExpressionMatch match = re.match(stillToParse);
    if (match.hasMatch()) {
        auto portString = match.captured(0);
        stillToParse = stillToParse.replace(portString, QString());
        auto portNumber = portString.replace(QLatin1Char(':'), QString()).toInt();
        if (portNumber <= 65535) {
            m_currentUrl.setPort(portNumber);
            correctSoFar = true;
        }
    }
}

void ConnectPage::showStandardPortAssumedWarning()
{
    ui->host->addAction(m_implicitPortWarningSign, QLineEdit::TrailingPosition);
}

void ConnectPage::showFileIsNotSocketWarning()
{
    ui->host->addAction(m_fileIsNotASocketWarning, QLineEdit::TrailingPosition);
}

void ConnectPage::clearWarnings()
{
    ui->host->removeAction(m_implicitPortWarningSign);
    ui->host->removeAction(m_fileIsNotASocketWarning);
}

QUrl ConnectPage::currentUrl() const
{
    return m_currentUrl;
}

ConnectPage::~ConnectPage() = default;

bool ConnectPage::isValid() const
{
    return m_valid;
}

void ConnectPage::launchClient()
{
    ClientLauncher::launchDetached(m_currentUrl);
}

void ConnectPage::writeSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("Connect/Url"), ui->host->text());
}

void ConnectPage::instanceSelected()
{
    const QModelIndexList rows = ui->instanceView->selectionModel()->selectedRows();
    if (rows.size() != 1)
        return;

    ui->host->setText(rows.first().data(NetworkDiscoveryModel::UrlStringRole).toString());
}
