/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pipshellsurface.h"

#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QtWaylandClient/private/qwaylandinputdevice_p.h>

XdgWmBase::XdgWmBase()
    : QWaylandClientExtensionTemplate<XdgWmBase>(6)
{
    initialize();
    if (!isActive()) {
        qFatal("The xdg-shell protocol is unsupported by the compositor");
    }
}

XXPipShell::XXPipShell()
    : QWaylandClientExtensionTemplate<XXPipShell>(1)
{
    initialize();
    if (!isActive()) {
        qInfo("The xx-pip-v1 protocol is unsupported by the compositor");
    }
}

PipShellIntegration &PipShellIntegration::instance()
{
    static PipShellIntegration s_instance;
    return s_instance;
}

bool PipShellIntegration::xxPipShellAvailable()
{
    return instance().m_xxPipShell->isActive();
}

void PipShellIntegration::assignPipRole(QWindow *window)
{
    window->create();

    auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
    if (!waylandWindow) {
        return;
    }

    waylandWindow->setShellIntegration(&instance());
}

PipShellIntegration::PipShellIntegration()
    : m_xdgWmBase(std::make_unique<XdgWmBase>())
    , m_xxPipShell(std::make_unique<XXPipShell>())
{
}

bool PipShellIntegration::initialize(QtWaylandClient::QWaylandDisplay *display)
{
    return m_xdgWmBase->isInitialized() && m_xxPipShell->isInitialized();
}

QtWaylandClient::QWaylandShellSurface *PipShellIntegration::createShellSurface(QtWaylandClient::QWaylandWindow *window)
{
    ::xdg_surface *xdgSurface = m_xdgWmBase->get_xdg_surface(window->wlSurface());
    ::xx_pip_v1 *xxPip = m_xxPipShell->get_pip(xdgSurface);
    return new PipShellSurface(xdgSurface, xxPip, window);
}

PipShellSurface::PipShellSurface(::xdg_surface *xdgSurface, ::xx_pip_v1 *xxPip, QtWaylandClient::QWaylandWindow *window)
    : QWaylandShellSurface(window)
    , QtWayland::xdg_surface(xdgSurface)
    , QtWayland::xx_pip_v1(xxPip)
{
}

PipShellSurface::~PipShellSurface()
{
    xx_pip_v1::destroy();
    xdg_surface::destroy();
}

bool PipShellSurface::isExposed() const
{
    return m_configured;
}

void PipShellSurface::applyConfigure()
{
    QSize size = window()->windowContentGeometry().size();
    if (m_pendingSize.width() > 0) {
        size.setWidth(m_pendingSize.width());
    }
    if (m_pendingSize.height() > 0) {
        size.setHeight(m_pendingSize.height());
    }

    window()->resizeFromApplyConfigure(size);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0)
void PipShellSurface::setWindowGeometry(const QRect &rect)
#else
void PipShellSurface::setContentGeometry(const QRect &rect)
#endif
{
    if (!isExposed()) {
        return;
    }
    if (m_contentGeometry != rect) {
        m_contentGeometry = rect;
        xdg_surface::set_window_geometry(rect.x(), rect.y(), rect.width(), rect.height());
    }
}

bool PipShellSurface::move(QtWaylandClient::QWaylandInputDevice *inputDevice)
{
    if (!m_configured) {
        return false;
    }
    xx_pip_v1::move(inputDevice->wl_seat(), inputDevice->serial());
    return true;
}

bool PipShellSurface::resize(QtWaylandClient::QWaylandInputDevice *inputDevice, Qt::Edges edges)
{
    if (!m_configured) {
        return false;
    }

    const resize_edge edge =
        static_cast<enum resize_edge>(((edges & Qt::TopEdge) ? resize_edge_top : 0) | ((edges & Qt::BottomEdge) ? resize_edge_bottom : 0)
                                      | ((edges & Qt::LeftEdge) ? resize_edge_left : 0) | ((edges & Qt::RightEdge) ? resize_edge_right : 0));

    xx_pip_v1::resize(inputDevice->wl_seat(), inputDevice->serial(), edge);
    return true;
}

void PipShellSurface::xdg_surface_configure(uint32_t serial)
{
    xdg_surface::ack_configure(serial);

    if (!m_configured) {
        m_configured = true;
        applyConfigure();
    } else {
        window()->applyConfigureWhenPossible();
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0)
    setWindowGeometry(window()->windowContentGeometry());
    window()->sendRecursiveExposeEvent();
#else
    setContentGeometry(window()->windowContentGeometry());
    window()->updateExposure();
#endif
}

void PipShellSurface::xx_pip_v1_configure_bounds(int32_t width, int32_t height)
{
}

void PipShellSurface::xx_pip_v1_configure_size(int32_t width, int32_t height)
{
    m_pendingSize = QSize(width, height);
}

void PipShellSurface::xx_pip_v1_closed()
{
    QWindowSystemInterface::handleCloseEvent(window()->window());
}

#include "moc_pipshellsurface.cpp"
