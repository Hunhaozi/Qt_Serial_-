#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QTextCodec>
#include <QRadioButton>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    resize(910,642);

    //设置应用图片
    this->setWindowIcon(QPixmap(":/soure/vegetable_mouse.png"));
    //设置窗口标题
    this->setWindowTitle("串口调试助手");


    //创建串口
    my_serialPort  = new QSerialPort;

    //创建获取串口号列表
    QStringList my_serialPortName = getPortNameList();

    //清空下拉栏
    ui->com_combox->clear();

    //添加查询到的串口到窗口中
    ui->com_combox->addItems(my_serialPortName);

    //"发送"默认按钮为关闭
    ui->send_but->setEnabled(false);

    //监听“打开串口”按钮
    connect(ui->open_radiobut, &QRadioButton::clicked,[=](){
        open_SerialCom(my_serialPortName);
    });

    //监听"发送"按钮
    connect(ui->send_but,&QPushButton::clicked,[=](){
        send_data();
    });

    //监听"接收"数据
    connect(my_serialPort,&QSerialPort::readyRead,[=]() {
        receive_data();
    });

    //监听"清空"按钮
    connect(ui->clear_re_but,&QPushButton::clicked,[=]{
        ui->re_text->clear();
    });
    connect(ui->clear_send_but,&QPushButton::clicked,[=]{
        ui->send_text->clear();
    });

    //当hex模式时，禁止选择gbk等编码
    connect(ui->hex_re_cheak, &QPushButton::clicked, [=]() {
        toggleHexSettings();
    });
    connect(ui->hex_send_cheak, &QPushButton::clicked, [=]() {
        toggleHexSettings();
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}


//获取串口号
QStringList MainWindow::getPortNameList()
{
    QStringList my_serialPortName;
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
    return my_serialPortName;
}


//打开串口
void MainWindow::open_SerialCom(QStringList my_serialPortName)
{
    auto ports  = QSerialPortInfo::availablePorts();
    if (ui->open_radiobut->text()=="打开串口")
    {
        //再次检查串口是否有效
        if(ui->com_combox->count() == 0||ports.isEmpty()||my_serialPort->isOpen()) //当没有串口来连接
        {
            err_Open_Serial();
            return;
        }
        if(my_serialPort->isOpen())
        {
            my_serialPort->clear();
            my_serialPort->close();
        }

        my_serialPort->setPortName(my_serialPortName[ui->com_combox->currentIndex()]);

        if(!my_serialPort->open(QIODevice::ReadWrite))
        {
            err_Open_Serial();
            return;
        }

        //打开成功
        my_serialPort->setBaudRate(ui->btl_combox->currentText().toInt(),QSerialPort::AllDirections);//设置波特率和读写方向

        switch (ui->data_combox->currentIndex()) {
        case 0 :     my_serialPort->setDataBits(QSerialPort::Data8); break;            //数据位为8位
        case 1 :     my_serialPort->setDataBits(QSerialPort::Data7); break;
        case 2 :     my_serialPort->setDataBits(QSerialPort::Data6); break;
        case 3 :     my_serialPort->setDataBits(QSerialPort::Data5); break;
        default: qDebug()<<"数据位错误";
        }

        switch (ui->stop_combox->currentIndex()) {
        case 0 :     my_serialPort->setStopBits(QSerialPort::OneStop);break;            //一位停止位
        case 1 :     my_serialPort->setStopBits(QSerialPort::OneAndHalfStop);break;
        case 2 :     my_serialPort->setStopBits(QSerialPort::TwoStop);break;
        default: qDebug()<<"停止位错误";
        }

        switch (ui->jiaoyan_combox->currentIndex()) {
        case 0 :     my_serialPort->setParity(QSerialPort::NoParity);break;             //无校验位
        case 1 :     my_serialPort->setParity(QSerialPort::OddParity);break;
        case 2 :     my_serialPort->setParity(QSerialPort::EvenParity);break;
        default: qDebug()<<"校验位错误";
        }

        my_serialPort->setFlowControl(QSerialPort::NoFlowControl);   //无流控制

        connect(my_serialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

        //禁用部分按钮等
        my_setEnabled(false);
        ui->open_radiobut->setText("关闭串口");
    }
    else
    {
        my_setEnabled(true);
        my_serialPort->close();
        ui->open_radiobut->setText("打开串口");
    }
}

//串口打开失败界面
void MainWindow::err_Open_Serial()
{
    QMessageBox::critical(this, "警告", "串口打开失败,请检查串口");
    ui->open_radiobut->setChecked(false);
    my_setEnabled(true);
}

//接收数据
void MainWindow::receive_data()
{
    QByteArray info = my_serialPort->readAll();

    QString strReceiveData = "";
    if(ui->hex_re_cheak->isChecked())
    {
        QByteArray hexData = info.toHex();
        strReceiveData = hexData.toUpper();

        qDebug()<<"接收到串口数据: "<<strReceiveData;

        for(int i=0; i<strReceiveData.size(); i+=2+1)
            strReceiveData.insert(i, QLatin1String(" "));
        strReceiveData.remove(0, 1);

        qDebug()<<"处理后的串口数据: "<<strReceiveData;

        ui->re_text->append(strReceiveData);
    }
    else
    {
        strReceiveData = info;

        if (ui->gbk_re->isChecked())
        {
            QTextCodec *tc = QTextCodec::codecForName("GBK");
            QString tmpQStr = tc->toUnicode(info);
            ui->re_text->append(tmpQStr);
        }
        else
        {
            QString tmpQStr = QString::fromUtf8(info);
            ui->re_text->append(tmpQStr);
        }
    }
}

//发送数据
void MainWindow::send_data()
{
    //检查串口打开情况
    if (!my_serialPort->isOpen())
    {
        err_Open_Serial();
        return;
    }

    QString my_strSendData = ui->send_text->toPlainText();

    if(ui->hex_send_cheak->isChecked())
    {
        my_strSendData.remove(" "); // 去掉空格

        QByteArray sendBuf;

        if (!isHexString(my_strSendData))
        {
            QMessageBox::critical(this, "警告", "输入内容非16进制");
            return;
        }

        convertStringToHex(my_strSendData, sendBuf);             //把QString 转换 为 hex

        my_serialPort->write(sendBuf);

    }
    else
    {
        if(ui->gbk_send->isChecked())
        {
            my_serialPort->write(my_strSendData.toLocal8Bit());//发送GBK编码数据
        }
        else my_serialPort->write(my_strSendData.toUtf8());  // 发送UTF-8编码数据

    }
    my_serialPort->write("\r\n");

}

void MainWindow::convertStringToHex(const QString &str, QByteArray &sendBuf)
{
    sendBuf.clear();
    QString hexStr = str.simplified().replace(" ", ""); // 去掉空格

    for (int i = 0; i < hexStr.length(); i += 2) {
        bool ok;
        char byte = hexStr.mid(i, 2).toUShort(&ok, 16);
        if (ok) {
            sendBuf.append(byte);
        } else {
            sendBuf.clear(); // 如果有无效字符，清空结果
            break;
        }
    }
}


bool MainWindow::isHexString(const QString &str)
{
    // 正则表达式匹配 0-9、a-f、A-F 和空格
    QRegularExpression re("^[0-9a-fA-F ]+$");
    return re.match(str).hasMatch();
}

void MainWindow::my_setEnabled(bool flag)
{
    ui->com_combox->setEnabled(flag);
    ui->btl_combox->setEnabled(flag);
    ui->stop_combox->setEnabled(flag);
    ui->data_combox->setEnabled(flag);
    ui->jiaoyan_combox->setEnabled(flag);
    ui->send_but->setEnabled(!flag);
}

void MainWindow::toggleHexSettings()
{
    if (ui->hex_re_cheak->isChecked() || ui->hex_send_cheak->isChecked()) {
        // 当 hex_re_cheak 或 hex_send_cheak 被勾选时，禁用相关控件
        ui->gbk_re->setEnabled(false);
        ui->gbk_send->setEnabled(false);
    } else {
        // 当都没有勾选时，恢复相关控件
        ui->gbk_re->setEnabled(true);
        ui->gbk_send->setEnabled(true);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (my_serialPort->isOpen()) {
        my_serialPort->close();  // 关闭串口
    }

    event->accept();  // 允许窗口关闭
}



