#ifndef QWINEVENTNOTIFIER_P_H
#define QWINEVENTNOTIFIER_P_H
#include "QtCore/qobject.h"
#include "QtCore/qt_windows.h"

class Q_CORE_EXPORT QWinEventNotifier : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QObject)

public:
    explicit QWinEventNotifier(QObject *parent = 0);
    explicit QWinEventNotifier(HANDLE hEvent, QObject *parent = 0);
    ~QWinEventNotifier();

    void setHandle(HANDLE hEvent);
    HANDLE handle() const;

    bool isEnabled() const;

public Q_SLOTS:
    void setEnabled(bool enable);

Q_SIGNALS:
    void activated(HANDLE hEvent);

protected:
    bool event(QEvent * e);

private:
    Q_DISABLE_COPY(QWinEventNotifier)

    HANDLE handleToEvent;
    bool enabled;
};

#endif // QWINEVENTNOTIFIER_P_H
