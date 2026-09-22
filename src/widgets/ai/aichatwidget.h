// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#pragma once

#include "utils/openaiclient.h"

#include <QPixmap>
#include <QWidget>

class QCloseEvent;
class QLabel;
class QPushButton;
class QTextEdit;

class AIChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIChatWidget(QWidget* parent = nullptr);

    void startRequest(const QString& apiUrl,
                      const QString& apiToken,
                      const QString& model,
                      const QString& mode,
                      const QString& prompt,
                      const QPixmap& image);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onResponseReceived(const QString& text);
    void onErrorOccurred(const QString& error);
    void onCopyClicked();

private:
    OpenAIClient* m_client;
    QTextEdit* m_responseEdit;
    QLabel* m_statusLabel;
    QPushButton* m_copyButton;
    QPushButton* m_closeButton;
};
