// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#pragma once

#include <QDialog>
#include <QString>

class QComboBox;
class QPlainTextEdit;

class AIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIDialog(const QString& defaultMode,
                      const QString& defaultPrompt,
                      QWidget* parent = nullptr);

    QString mode() const;
    QString prompt() const;

private:
    void onModeChanged(int index);
    void updatePlaceholder();

    QComboBox* m_modeCombo;
    QPlainTextEdit* m_promptEdit;

    struct ModePreset
    {
        QString id;
        QString label;
        QString placeholder;
    };

    QVector<ModePreset> m_presets;
};
