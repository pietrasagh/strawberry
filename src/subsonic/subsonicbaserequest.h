/*
 * Strawberry Music Player
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

#ifndef SUBSONICBASEREQUEST_H
#define SUBSONICBASEREQUEST_H

#include "config.h"

#include <memory>

#include <QObject>
#include <QList>
#include <QSet>
#include <QPair>
#include <QVariant>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QSslError>
#include <QJsonObject>

#include "subsonicservice.h"

class QNetworkAccessManager;
class QNetworkReply;

class SubsonicBaseRequest : public QObject {
  Q_OBJECT

 public:
  explicit SubsonicBaseRequest(SubsonicService *service, QObject *parent);

  typedef QPair<QString, QString> Param;
  typedef QList<Param> ParamList;

  typedef QPair<QByteArray, QByteArray> EncodedParam;
  typedef QList<EncodedParam> EncodedParamList;

  QUrl CreateUrl(const QString &ressource_name, const QList<Param> &params_provided);
  QNetworkReply *CreateGetRequest(const QString &ressource_name, const QList<Param> &params_provided);
  QByteArray GetReplyData(QNetworkReply *reply);
  QJsonObject ExtractJsonObj(QByteArray &data);

  virtual void Error(const QString &error, const QVariant &debug = QVariant()) = 0;
  QString ErrorsToHTML(const QStringList &errors);

  QString client_name() { return service_->client_name(); }
  QString api_version() { return service_->api_version(); }
  QUrl server_url() { return service_->server_url(); }
  QString username() { return service_->username(); }
  QString password() { return service_->password(); }
  bool verify_certificate() { return service_->verify_certificate(); }
  bool download_album_covers() { return service_->download_album_covers(); }

 private slots:
  void HandleSSLErrors(QList<QSslError> ssl_errors);

 private:
  SubsonicService *service_;
  std::unique_ptr<QNetworkAccessManager> network_;

};

#endif  // SUBSONICBASEREQUEST_H
