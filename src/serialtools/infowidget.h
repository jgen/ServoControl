#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QWidget>

namespace Ui {
    class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT
public:
    //
    class InfoData
    {
    public:
        QString name;
        QString shortName;
        QString systemPath;
        QString subSystem;
        QString locationInfo;
        QString driver;
        QString friendlyName;
        QString description;
        QStringList hardwareID;
        QString vendorID;
        QString productID;
        QString manufacturer;
        QString service;
        QString bus;
        QString revision;
        bool exists;
        bool busy;
    };

    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();

    void updateInfo(const InfoData &data);

private slots:
    void on_btnClose_pressed();

private:
    Ui::InfoWidget *ui;
};

#endif // INFOWIDGET_H
