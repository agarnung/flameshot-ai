// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#pragma once

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPixmap>

class OpenAIClient : public QObject
{
    Q_OBJECT

public:
    explicit OpenAIClient(QObject* parent = nullptr);

    void setApiUrl(const QString& url);
    void setApiToken(const QString& token);
    void setModel(const QString& model);

    void requestWithImage(const QString& mode,
                          const QString& prompt,
                          const QPixmap& image);

signals:
    void responseReceived(const QString& text);
    void errorOccurred(const QString& error);

private:
    QString systemPromptForMode(const QString& mode) const;
    QString userPromptForMode(const QString& mode,
                              const QString& prompt) const;
    void handleReply(QNetworkReply* reply);

    QNetworkAccessManager* m_networkManager;
    QString m_apiUrl;
    QString m_apiToken;
    QString m_model;
};
