/*
 * Strawberry Music Player
 * This file was part of Clementine.
 * Copyright 2010, David Sansome <me@davidsansome.com>
 * Copyright 2019-2021, Jonas Kvinge <jonas@jkvinge.net>
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

#ifndef PARSERBASE_H
#define PARSERBASE_H

#include <QtGlobal>
#include <QObject>
#include <QDir>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "config.h"
#include "core/song.h"
#include "playlist/playlist.h"

class QIODevice;
class CollectionBackendInterface;

class ParserBase : public QObject {
  Q_OBJECT

 public:
  explicit ParserBase(CollectionBackendInterface *collection, QObject *parent = nullptr);

  virtual QString name() const = 0;
  virtual QStringList file_extensions() const = 0;
  virtual QString mime_type() const { return QString(); }

  virtual bool TryMagic(const QByteArray &data) const = 0;

  // Loads all songs from playlist found at path 'playlist_path' in directory 'dir'.
  // The 'device' argument is an opened and ready to read from represantation of this playlist.
  // This method might not return all of the songs found in the playlist.
  // Any playlist parser may decide to leave out some entries if it finds them incomplete or invalid.
  // This means that the final resulting SongList should be considered valid (at least from the parser's point of view).
  virtual SongList Load(QIODevice *device, const QString &playlist_path = "", const QDir &dir = QDir()) const = 0;
  virtual void Save(const SongList &songs, QIODevice *device, const QDir &dir = QDir(), Playlist::Path path_type = Playlist::Path_Automatic) const = 0;

 protected:
  // Loads a song.  If filename_or_url is a URL (with a scheme other than "file") then it is set on the song and the song marked as a stream.
  // If it is a filename or a file:// URL then it is made absolute and canonical and set as a file:// url on the song.
  // Also sets the song's metadata by searching in the Collection, or loading from the file as a fallback.
  // This function should always be used when loading a playlist.
  Song LoadSong(const QString &filename_or_url, qint64 beginning, const QDir &dir) const;
  void LoadSong(const QString &filename_or_url, qint64 beginning, const QDir &dir, Song *song) const;

  // If the URL is a file:// URL then returns its path, absolute or relative to the directory depending on the path_type option.
  // Otherwise returns the URL as is. This function should always be used when saving a playlist.
  QString URLOrFilename(const QUrl &url, const QDir &dir, Playlist::Path path_type) const;

 private:
  CollectionBackendInterface *collection_;
};

#endif  // PARSERBASE_H
