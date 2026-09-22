// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Flameshot IA Contributors

#include "aitool.h"

AITool::AITool(QObject* parent)
  : AbstractActionTool(parent)
{}

bool AITool::closeOnButtonPressed() const
{
    return true;
}

QIcon AITool::icon(const QColor& background, bool inEditor) const
{
    Q_UNUSED(inEditor)
    return QIcon(iconPath(background) + "ai.svg");
}

QString AITool::name() const
{
    return tr("AI");
}

CaptureTool::Type AITool::type() const
{
    return CaptureTool::TYPE_AI;
}

QString AITool::description() const
{
    return tr("Analyze screenshot with AI");
}

CaptureTool* AITool::copy(QObject* parent)
{
    return new AITool(parent);
}

void AITool::pressed(CaptureContext& context)
{
    emit requestAction(REQ_CLEAR_SELECTION);
    context.request.addTask(CaptureRequest::AI);
    emit requestAction(REQ_CAPTURE_DONE_OK_KEEP_OPEN);
}
