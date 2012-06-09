#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QDateTime>


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
//    AbstractSerial* getSerialPointer();

    void setConnectionController(ConnectionController* con);//May need checks in every method to see if set
    void clearConnectionController();

    void updateEnumeratedDevices(const QStringList &l,int index);
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
    void procSerialMessages(const QString &msg, QDateTime dt);
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
 //   AbstractSerial *serial;
    ConnectionController* control;



    bool initInfoWidget();
    void deinitOptionsWidget();
    bool initTraceWidget();
    void initButtonConnections();
    void initBoxConnections();
    void deinitWidgets();
    void setRtsDtrButtonsCaption(bool opened, bool rts, bool dtr);
    void setDefaultOptions();
};

#endif // SERIALWIDGET_H
