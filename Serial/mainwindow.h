#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    //定义串口
    QSerialPort *my_serialPort;

    //获取串口列表
    QStringList getPortNameList();

    //打开串口
    void open_SerialCom(QStringList my_serialPortName);

    //串口打开失败界面
    void err_Open_Serial();

    //接受数据
    void receive_data();

    //string 转 hex
    void convertStringToHex(const QString &str, QByteArray &sendBuf);

    //判断是否为16进制
    bool isHexString(const QString &str);

    //发送数据
    void send_data();

    //禁用部分按钮
    void my_setEnabled(bool flag);
    void toggleHexSettings();

    void closeEvent(QCloseEvent *event);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
