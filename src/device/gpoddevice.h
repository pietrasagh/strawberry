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

#ifndef GPODDEVICE_H
#define GPODDEVICE_H

#include "config.h"

#include <memory>

#include <gpod/itdb.h>

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QTemporaryFile>

#include "core/song.h"
#include "core/musicstorage.h"
#include "connecteddevice.h"
#include "gpodloader.h"

class QThread;
class Application;
class DeviceLister;
class DeviceManager;

class GPodDevice : public ConnectedDevice, public virtual MusicStorage {
  Q_OBJECT

 public:
  Q_INVOKABLE GPodDevice(const QUrl &url, DeviceLister *lister, const QString &unique_id, DeviceManager *manager, Application *app, const int database_id, const bool first_time);
  ~GPodDevice() override;

  bool Init() override;
  void ConnectAsync() override;
  void Close() override;
  bool IsLoading() override { return loader_; }
  QObject *Loader() { return loader_; }

  static QStringList url_schemes() { return QStringList() << "ipod"; }

  bool GetSupportedFiletypes(QList<Song::FileType> *ret) override;

  bool StartCopy(QList<Song::FileType> *supported_types) override;
  bool CopyToStorage(const CopyJob &job) override;
  void FinishCopy(bool success) override;

  void StartDelete() override;
  bool DeleteFromStorage(const DeleteJob &job) override;
  void FinishDelete(bool success) override;

 protected slots:
  void LoadFinished(Itdb_iTunesDB *db, bool success);
  void LoaderError(const QString& message);

 protected:
  Itdb_Track *AddTrackToITunesDb(const Song &metadata);
  void AddTrackToModel(Itdb_Track *track, const QString &prefix);
  bool RemoveTrackFromITunesDb(const QString &path, const QString &relative_to = QString());

 private:
  void Start();
  void Finish(const bool success);
  bool WriteDatabase();

 protected:
  GPodLoader *loader_;
  QThread *loader_thread_;

  QWaitCondition db_wait_cond_;
  QMutex db_mutex_;
  Itdb_iTunesDB *db_;
  bool closing_;

  QMutex db_busy_;
  SongList songs_to_add_;
  SongList songs_to_remove_;
  QList<std::shared_ptr<QTemporaryFile>> cover_files_;
};

#endif // GPODDEVICE_H
