/*
 * SPDX-FileCopyrightText: 2025 David Edmundson <kde@davidedmundson.co.uk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "pipwindow.h"

#include "wayland/pipshellsurface.h"

#include <optional>

#include <QGuiApplication>
#include <QStyleHints>

class PipWindowPrivate
{
public:
    PipWindowPrivate(PipWindow *parent)
        : q(parent)
    {
    }
    PipWindow *q;

    bool updateMouseCursor(const QPointF &globalMousePos);
    void unsetMouseCursor();

    Qt::Edges hitTest(const QPointF &pos);
    QMargins margins = QMargins(10, 10, 10, 10);
    std::optional<QPointF> lastPress;

private:
    bool overridingCursor = false;
    bool hitTestLeft(const QPointF &pos);
    bool hitTestRight(const QPointF &pos);
    bool hitTestTop(const QPointF &pos);
    bool hitTestBottom(const QPointF &pos);
};

PipWindow::PipWindow()
    : QQuickWindow()
    , d(new PipWindowPrivate(this))
{
    setFlags(Qt::FramelessWindowHint);
    setFlags(Qt::WindowStaysOnTopHint); // for X11
    PipShellIntegration::instance().assignPipRole(this); // for Wayland
    setColor(Qt::transparent);

    // need to set windowContentMargins in sync with our shadow
    //  or use plasma dialog shadows?
}

PipWindow::~PipWindow()
{
}

bool PipWindow::isSupported() const
{
    // FIXME always supported on !wayland
    return PipShellIntegration::instance().xxPipShellAvailable();
}

// most of this (except move) is taken from libplasma's WindowResizeHandler
// maybe that could be uplifted into frameworks

bool PipWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter: {
        QEnterEvent *ee = static_cast<QEnterEvent *>(event);
        d->updateMouseCursor(ee->globalPosition());
        return false;
    }
    case QEvent::Leave:
        d->lastPress.reset();
        d->unsetMouseCursor();
        return false;
    case QEvent::HoverMove: {
        auto *he = static_cast<QHoverEvent *>(event);
        d->updateMouseCursor(he->globalPosition());
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        d->updateMouseCursor(me->globalPosition());
        if (me->buttons() && d->lastPress) {
            if ((*d->lastPress - me->globalPosition()).manhattanLength() >= qGuiApp->styleHints()->startDragDistance()) {
                startSystemMove();
                d->lastPress.reset();
            }
        }
    } break;
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

        const QPointF globalMousePos = me->globalPosition();
        const Qt::Edges sides = d->hitTest(globalMousePos);
        if (sides) {
            startSystemResize(sides);
            return true;
        } else {
            bool rc = QQuickWindow::event(event);
            if (!me->isAccepted()) {
                // startSystemMove only when distance exceeds startDragDistance.
                d->lastPress = me->globalPosition();
            }
            return rc;
        }
    } break;
    case QEvent::MouseButtonRelease: {
        d->lastPress.reset();
        break;
    }
    default:
        break;
    }

    return QQuickWindow::event(event);
}

void PipWindowPrivate::unsetMouseCursor()
{
    if (overridingCursor) {
        q->setCursor(Qt::ArrowCursor); // FIXME, clearly unset doesn't work very well...
        q->unsetCursor();
        overridingCursor = false;
    }
}

bool PipWindowPrivate::updateMouseCursor(const QPointF &globalMousePos)
{
    Qt::Edges sides = hitTest(globalMousePos);
    if (!sides) {
        unsetMouseCursor();
        return false;
    }

    if (sides == Qt::Edges(Qt::LeftEdge | Qt::TopEdge)) {
        q->setCursor(Qt::SizeFDiagCursor);
    } else if (sides == Qt::Edges(Qt::RightEdge | Qt::TopEdge)) {
        q->setCursor(Qt::SizeBDiagCursor);
    } else if (sides == Qt::Edges(Qt::LeftEdge | Qt::BottomEdge)) {
        q->setCursor(Qt::SizeBDiagCursor);
    } else if (sides == Qt::Edges(Qt::RightEdge | Qt::BottomEdge)) {
        q->setCursor(Qt::SizeFDiagCursor);
    } else if (sides.testFlag(Qt::TopEdge)) {
        q->setCursor(Qt::SizeVerCursor);
    } else if (sides.testFlag(Qt::LeftEdge)) {
        q->setCursor(Qt::SizeHorCursor);
    } else if (sides.testFlag(Qt::RightEdge)) {
        q->setCursor(Qt::SizeHorCursor);
    } else if (sides.testFlag(Qt::BottomEdge)) {
        q->setCursor(Qt::SizeVerCursor);
    } else {
        q->setCursor(Qt::DragMoveCursor);
    }

    overridingCursor = true;
    return true;
}

Qt::Edges PipWindowPrivate::hitTest(const QPointF &pos)
{
    bool left = hitTestLeft(pos);
    bool right = hitTestRight(pos);
    bool top = hitTestTop(pos);
    bool bottom = hitTestBottom(pos);
    Qt::Edges edges;
    if (left) {
        edges.setFlag(Qt::LeftEdge);
    }
    if (right) {
        edges.setFlag(Qt::RightEdge);
    }
    if (bottom) {
        edges.setFlag(Qt::BottomEdge);
    }
    if (top) {
        edges.setFlag(Qt::TopEdge);
    }

    return edges;
}

// Dave, why are we using global positions here? Eww.

bool PipWindowPrivate::hitTestLeft(const QPointF &pos)
{
    const QRect geometry = q->geometry();
    const QRectF rect(geometry.x(), geometry.y(), margins.left(), geometry.height());
    return rect.contains(pos);
}

bool PipWindowPrivate::hitTestRight(const QPointF &pos)
{
    const QRect geometry = q->geometry();
    const QRectF rect(geometry.x() + geometry.width() - margins.right(), geometry.y(), margins.right(), geometry.height());
    return rect.contains(pos);
}

bool PipWindowPrivate::hitTestTop(const QPointF &pos)
{
    const QRect geometry = q->geometry();
    const QRectF rect(geometry.x(), geometry.y(), geometry.width(), margins.top());
    return rect.contains(pos);
}

bool PipWindowPrivate::hitTestBottom(const QPointF &pos)
{
    const QRect geometry = q->geometry();
    const QRectF rect(geometry.x(), geometry.y() + geometry.height() - margins.bottom(), geometry.width(), margins.bottom());
    return rect.contains(pos);
}
