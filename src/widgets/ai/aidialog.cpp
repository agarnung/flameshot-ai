// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#include "aidialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QPlainTextEdit>
#include <QVBoxLayout>

AIDialog::AIDialog(const QString& defaultMode,
                   const QString& defaultPrompt,
                   QWidget* parent)
  : QDialog(parent)
{
    setWindowTitle(tr("AI Assistant"));
    setMinimumWidth(480);

    m_presets = {
      { "ask", tr("Ask"), tr("Type your question about the capture...") },
      { "translate",
        tr("Translate text"),
        tr("Translate all visible text to the configured language.") },
      { "explain",
        tr("Explain"),
        tr("Explain in detail what appears in the image.") },
    };

    auto* layout = new QVBoxLayout(this);
    auto* formLayout = new QFormLayout();

    m_modeCombo = new QComboBox(this);
    int defaultIndex = 0;
    for (int i = 0; i < m_presets.size(); ++i) {
        m_modeCombo->addItem(m_presets[i].label, m_presets[i].id);
        if (m_presets[i].id == defaultMode) {
            defaultIndex = i;
        }
    }
    m_modeCombo->setCurrentIndex(defaultIndex);
    formLayout->addRow(tr("Mode:"), m_modeCombo);

    m_promptEdit = new QPlainTextEdit(this);
    m_promptEdit->setPlaceholderText(m_presets[defaultIndex].placeholder);
    m_promptEdit->setPlainText(defaultPrompt);
    formLayout->addRow(tr("Prompt / instruction:"), m_promptEdit);

    layout->addLayout(formLayout);

    auto* buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                           this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    connect(m_modeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &AIDialog::onModeChanged);
}

void AIDialog::onModeChanged(int index)
{
    if (index < 0 || index >= m_presets.size()) {
        return;
    }

    // Only replace placeholder text if the user has not typed anything yet.
    if (m_promptEdit->toPlainText().trimmed().isEmpty()) {
        updatePlaceholder();
    }
}

void AIDialog::updatePlaceholder()
{
    int index = m_modeCombo->currentIndex();
    if (index >= 0 && index < m_presets.size()) {
        m_promptEdit->setPlaceholderText(m_presets[index].placeholder);
    }
}

QString AIDialog::mode() const
{
    int index = m_modeCombo->currentIndex();
    if (index >= 0 && index < m_presets.size()) {
        return m_presets[index].id;
    }
    return QStringLiteral("ask");
}

QString AIDialog::prompt() const
{
    return m_promptEdit->toPlainText().trimmed();
}
