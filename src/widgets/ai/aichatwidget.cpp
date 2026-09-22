// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#include "aichatwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

AIChatWidget::AIChatWidget(QWidget* parent)
  : QWidget(parent)
  , m_client(new OpenAIClient(this))
{
    setWindowTitle(tr("AI Assistant"));
    setMinimumSize(520, 360);

    auto* layout = new QVBoxLayout(this);

    m_statusLabel = new QLabel(tr("Asking the model…"), this);
    layout->addWidget(m_statusLabel);

    m_responseEdit = new QTextEdit(this);
    m_responseEdit->setReadOnly(true);
    m_responseEdit->setPlaceholderText(tr("The response will appear here."));
    layout->addWidget(m_responseEdit);

    auto* buttonLayout = new QHBoxLayout();
    m_copyButton = new QPushButton(tr("Copy"), this);
    m_copyButton->setEnabled(false);
    connect(m_copyButton, &QPushButton::clicked, this, &AIChatWidget::onCopyClicked);

    m_closeButton = new QPushButton(tr("Close"), this);
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_copyButton);
    buttonLayout->addWidget(m_closeButton);
    layout->addLayout(buttonLayout);

    connect(m_client,
            &OpenAIClient::responseReceived,
            this,
            &AIChatWidget::onResponseReceived);
    connect(m_client,
            &OpenAIClient::errorOccurred,
            this,
            &AIChatWidget::onErrorOccurred);
}

void AIChatWidget::startRequest(const QString& apiUrl,
                                const QString& apiToken,
                                const QString& model,
                                const QString& mode,
                                const QString& prompt,
                                const QPixmap& image)
{
    m_client->setApiUrl(apiUrl);
    m_client->setApiToken(apiToken);
    m_client->setModel(model);
    m_client->requestWithImage(mode, prompt, image);
}

void AIChatWidget::onResponseReceived(const QString& text)
{
    m_statusLabel->setText(tr("Response received."));
    m_responseEdit->setMarkdown(text);
    m_copyButton->setEnabled(true);
}

void AIChatWidget::onErrorOccurred(const QString& error)
{
    m_statusLabel->setText(tr("Error."));
    m_responseEdit->setPlainText(error);
    m_copyButton->setEnabled(false);
    QMessageBox::warning(this, tr("AI Assistant Error"), error);
}

void AIChatWidget::onCopyClicked()
{
    QApplication::clipboard()->setText(m_responseEdit->toPlainText());
    m_statusLabel->setText(tr("Response copied to clipboard."));
}
