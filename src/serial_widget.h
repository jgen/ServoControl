#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QDateTime>

namespace Ui {
    class SerialWidget;
}

QT_BEGIN_NAMESPACE
class SerialDeviceEnumerator;
class AbstractSerial;
class InfoWidget;
class TraceWidget;
QT_END_NAMESPACE

class SerialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWidget(QWidget *parent = 0);
    ~SerialWidget();
    AbstractSerial* getSerialPointer();

protected:
    void changeEvent(QEvent *e);

private slots:
    //
    void procEnumerate(const QStringList &l);
    void procSerialMessages(const QString &msg, QDateTime dt);
    void procSerialDataReceive();
    void procSerialDataTransfer(const QByteArray &data);
    void procCtsChanged(bool val);
    void procDsrChanged(bool val);
    void procRingChanged(bool val);
    // Proc buttons click
    void procControlButtonClick();
    void procInfoButtonClick();
    void procIOButtonClick();
    void on_btnApplyOptions_pressed();
    void procRtsButtonClick();
    void procDtrButtonClick();
    // Proc box item change
    void procBoxChange(const QString &item);
    void procOptionsBoxChanged();

private:
    Ui::SerialWidget *ui;
    InfoWidget *infoWidget;
    TraceWidget *traceWidget;

    SerialDeviceEnumerator *enumerator;
    AbstractSerial *serial;

    void initSerialWidgetCloseState();
    void initSerialWidgetOpenState();
    bool initInfoWidget();
    void initOptionsWidget();
    bool initTraceWidget();
    void initEnumerator();
    void deinitEnumerator();
    void initSerial();
    void deinitSerial();
    void initButtonConnections();
    void initBoxConnections();
    void deinitWidgets();
    void setRtsDtrButtonsCaption(bool opened, bool rts, bool dtr);
    void detectSerialLineStates();
    void updateInfoData(const QString &name);
    void setDefaultOptions();
};

#endif // SERIALWIDGET_H
