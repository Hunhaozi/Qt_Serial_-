#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>

class mycomboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit mycomboBox(QWidget *parent = nullptr);

    void showPopup();

signals:

};

#endif // MYCOMBOBOX_H
