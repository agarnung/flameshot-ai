// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#include "openaiclient.h"

#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>

OpenAIClient::OpenAIClient(QObject* parent)
  : QObject(parent)
  , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager,
            &QNetworkAccessManager::finished,
            this,
            &OpenAIClient::handleReply);
}

void OpenAIClient::setApiUrl(const QString& url)
{
    m_apiUrl = url;
}

void OpenAIClient::setApiToken(const QString& token)
{
    m_apiToken = token;
}

void OpenAIClient::setModel(const QString& model)
{
    m_model = model;
}

QString OpenAIClient::systemPromptForMode(const QString& mode) const
{
    if (mode == QStringLiteral("translate")) {
        return QObject::tr(
          "You are an expert translator. Extract and translate ALL visible text "
          "in the image to the requested language. Keep format and context.");
    }
    if (mode == QStringLiteral("explain")) {
        return QObject::tr(
          "You are a helpful teaching assistant. Explain in detail what appears "
          "in the screenshot, including text, UI elements, errors or other "
          "relevant items.");
    }
    // ask and default
    return QObject::tr(
      "You are a helpful assistant that analyzes screenshots. Answer clearly "
      "and concisely in the user's language.");
}

QString OpenAIClient::userPromptForMode(const QString& mode,
                                        const QString& prompt) const
{
    if (mode == QStringLiteral("translate")) {
        return QObject::tr(
          "Translate all visible text in this image. Return only the "
          "translation, keeping the original structure.");
    }
    if (mode == QStringLiteral("explain")) {
        return QObject::tr(
          "Explain in detail what appears in this image.");
    }
    // ask
    if (prompt.isEmpty()) {
        return QObject::tr("Describe this image.");
    }
    return prompt;
}

void OpenAIClient::requestWithImage(const QString& mode,
                                    const QString& prompt,
                                    const QPixmap& image)
{
    if (m_apiUrl.isEmpty() || m_model.isEmpty()) {
        emit errorOccurred(QObject::tr("API URL or model not configured."));
        return;
    }

    // Encode image to base64 PNG.
    QByteArray imageBytes;
    QBuffer buffer(&imageBytes);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "PNG")) {
        emit errorOccurred(QObject::tr("Failed to encode capture to PNG."));
        return;
    }
    QString imageBase64 = QString::fromLatin1(imageBytes.toBase64());

    QJsonObject imageUrlObj;
    imageUrlObj[QStringLiteral("url")] =
      QStringLiteral("data:image/png;base64,") + imageBase64;

    QJsonArray contentArray;
    contentArray.append(QJsonObject{
      { QStringLiteral("type"), QStringLiteral("text") },
      { QStringLiteral("text"), userPromptForMode(mode, prompt) } });
    contentArray.append(QJsonObject{
      { QStringLiteral("type"), QStringLiteral("image_url") },
      { QStringLiteral("image_url"), imageUrlObj } });

    QJsonArray messagesArray;
    messagesArray.append(QJsonObject{
      { QStringLiteral("role"), QStringLiteral("system") },
      { QStringLiteral("content"), systemPromptForMode(mode) } });
    messagesArray.append(QJsonObject{
      { QStringLiteral("role"), QStringLiteral("user") },
      { QStringLiteral("content"), contentArray } });

    QJsonObject payload;
    payload[QStringLiteral("model")] = m_model;
    payload[QStringLiteral("messages")] = messagesArray;
    payload[QStringLiteral("max_tokens")] = 2048;
    payload[QStringLiteral("temperature")] = 0.4;

    QNetworkRequest request;
    request.setUrl(QUrl(m_apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/json"));
    if (!m_apiToken.isEmpty()) {
        request.setRawHeader("Authorization",
                             QStringLiteral("Bearer %1").arg(m_apiToken).toUtf8());
    }

    m_networkManager->post(request, QJsonDocument(payload).toJson());
}

void OpenAIClient::handleReply(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString() + ": " + reply->readAll());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();
    QJsonArray choices = root[QStringLiteral("choices")].toArray();
    if (choices.isEmpty()) {
        emit errorOccurred(QObject::tr("No response choices returned by the API."));
        return;
    }

    QJsonObject firstChoice = choices.first().toObject();
    QJsonObject message = firstChoice[QStringLiteral("message")].toObject();
    QString text = message[QStringLiteral("content")].toString();
    emit responseReceived(text);
}
