#include "mycombobox.h"
#include "mainwindow.h"
#include <QDebug>
#include <QComboBox>

mycomboBox::mycomboBox(QWidget *parent) : QComboBox(parent)
{

}

//重写下拉后的操作
void mycomboBox::showPopup()
{
    qDebug()<<"qqq";

    QString current_text = this->currentText();
    QStringList my_serialPortName;
    my_serialPortName.clear();
    QComboBox::clear();

    auto ports  = QSerialPortInfo::availablePorts();
    if(ports.isEmpty())
    {
        qDebug()<<"没有串口连接";
    }
    else
    {
        for(const QSerialPortInfo &info : ports)
        {
            my_serialPortName << info.portName();
        }
    }

    QComboBox::addItems(my_serialPortName);
    setCurrentText(current_text);

    QComboBox::showPopup();//转给父类调用
}
