/*
 * Strawberry Music Player
 * This file was part of Clementine.
 * Copyright 2010, David Sansome <me@davidsansome.com>
 * Copyright 2018-2021, Jonas Kvinge <jonas@jkvinge.net>
 *
 * Strawberry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Strawberry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Strawberry.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include <QtGlobal>
#include <QFileInfo>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include <QRegularExpression>
#include <QUrl>

// This must come after Qt includes
#include <cdio/cdio.h>
#include <cdio/device.h>

#include "cddalister.h"
#include "core/logging.h"

QStringList CddaLister::DeviceUniqueIDs() { return devices_list_; }

QVariantList CddaLister::DeviceIcons(const QString &) {
  QVariantList icons;
  icons << QString("media-optical");
  return icons;
}

QString CddaLister::DeviceManufacturer(const QString &id) {

  CdIo_t* cdio = cdio_open(id.toLocal8Bit().constData(), DRIVER_DEVICE);
  cdio_hwinfo_t cd_info;
  if (cdio_get_hwinfo(cdio, &cd_info)) {
    cdio_destroy(cdio);
    return QString(cd_info.psz_vendor);
  }
  cdio_destroy(cdio);
  return QString();

}

QString CddaLister::DeviceModel(const QString &id) {

  CdIo_t* cdio = cdio_open(id.toLocal8Bit().constData(), DRIVER_DEVICE);
  cdio_hwinfo_t cd_info;
  if (cdio_get_hwinfo(cdio, &cd_info)) {
    cdio_destroy(cdio);
    return QString(cd_info.psz_model);
  }
  cdio_destroy(cdio);
  return QString();

}

quint64 CddaLister::DeviceCapacity(const QString &) { return 0; }

quint64 CddaLister::DeviceFreeSpace(const QString &) { return 0; }

QVariantMap CddaLister::DeviceHardwareInfo(const QString &) {
  return QVariantMap();
}

QString CddaLister::MakeFriendlyName(const QString &id) {

  CdIo_t *cdio = cdio_open(id.toLocal8Bit().constData(), DRIVER_DEVICE);
  cdio_hwinfo_t cd_info;
  if (cdio_get_hwinfo(cdio, &cd_info)) {
    cdio_destroy(cdio);
    return QString(cd_info.psz_model);
  }
  cdio_destroy(cdio);
  return QString("CD (") + id + ")";

}

QList<QUrl> CddaLister::MakeDeviceUrls(const QString &id) {
  return QList<QUrl>() << QUrl("cdda://" + id);
}

void CddaLister::UnmountDevice(const QString &id) {
  cdio_eject_media_drive(id.toLocal8Bit().constData());
}

void CddaLister::UpdateDeviceFreeSpace(const QString&) {}

bool CddaLister::Init() {

  cdio_init();
#ifdef Q_OS_MACOS
  if (!cdio_have_driver(DRIVER_OSX)) {
    qLog(Error) << "libcdio was compiled without support for macOS!";
  }
#endif
  char** devices = cdio_get_devices(DRIVER_DEVICE);
  if (!devices) {
    qLog(Debug) << "No CD devices found";
    return false;
  }
  for (; *devices != nullptr; ++devices) {
    QString device(*devices);
    QFileInfo device_info(device);
    if (device_info.isSymLink()) {
      device = device_info.symLinkTarget();
    }
#ifdef Q_OS_MACOS
    // Every track is detected as a separate device on Darwin. The raw disk looks like /dev/rdisk1
    if (!device.contains(QRegularExpression("^/dev/rdisk[0-9]$"))) {
      continue;
    }
#endif
    if (!devices_list_.contains(device)) {
      devices_list_ << device;
      emit DeviceAdded(device);
    }
  }

  return true;

}
