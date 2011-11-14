#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QDateTime>

namespace Ui {
    class MainWidget;
}

class SerialDeviceEnumerator;
class AbstractSerial;
class InfoWidget;
class TraceWidget;
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

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
    Ui::MainWidget *ui;
    InfoWidget *infoWidget;
    TraceWidget *traceWidget;

    SerialDeviceEnumerator *enumerator;
    AbstractSerial *serial;

    void initMainWidgetCloseState();
    void initMainWidgetOpenState();
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

#endif // MAINWIDGET_H
