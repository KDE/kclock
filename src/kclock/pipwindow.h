#pragma once

#include <QQuickWindow>

class PipWindowPrivate;

class PipWindow : public QQuickWindow
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit PipWindow();
    ~PipWindow();
    bool event(QEvent *event) override;

private:
    QScopedPointer<PipWindowPrivate> d;
};
