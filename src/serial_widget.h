#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QDateTime>
#include "serialwrapper.h"


QT_BEGIN_NAMESPACE
class SerialDeviceEnumerator;
class AbstractSerial;
class InfoWidget;
class TraceWidget;
class ConnectionController;
QT_END_NAMESPACE

namespace Ui {
    class SerialWidget;
}

class SerialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWidget(QWidget *parent = 0);
    ~SerialWidget();
    AbstractSerial* getSerialPointer();

    void setConnectionController(ConnectionController* con);//May need checks in every method to see if set
    void clearConnectionController();

    void updateEnumeratedDevices(const QStringList &l);
    void printToTrace(QByteArray data, bool isRx);
    void updateSerialLineStates(quint16 line,bool connected);
    void initSerialWidgetCloseState(quint16 line);
    void initSerialWidgetOpenState(quint16 line);
    void updateOptionsWidget(QStringList& baudrates, QStringList& dataBits,
                                          QStringList& parity, QStringList& stopBits,
                                           QStringList& flow);
    void updateInfoData(SerialDeviceEnumerator* enumerator,QString& name);


    void initSerialWidgetCloseState();
    void initSerialWidgetOpenState();

public slots:
    void procCtsChanged(bool val);
    void procDsrChanged(bool val);
    void procRingChanged(bool val);
protected:
    void changeEvent(QEvent *e);

private slots:
    //
    void procEnumerate(const QStringList &l);//Depreciated
    void procSerialMessages(const QString &msg, QDateTime dt);
    void procSerialDataReceive();
    void procSerialDataTransfer(const QByteArray &data);
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
    SerialWrapper* port;
    ConnectionController* control;



    bool initInfoWidget();
    void initOptionsWidget();
    void deinitOptionsWidget();
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
