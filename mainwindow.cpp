#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    QPixmap pic;
    QSize size;//画框大小

    pic.load(":/icons/icons/sky.jpg");

    //设置图片显示尺寸
    size.setHeight(128);
    size.setWidth(305);

    //照片处理
    pic=pic.scaled(size,Qt::IgnoreAspectRatio);
    //显示
    ui->schoolimg->setPixmap(pic);

    /* --------------------  Serial port  --------------------------*/

    m_serial_controller = new SerialController( this,
                                                ui->label_connectStatus,
                                               ui->comboBox_COM,
                                                ui->comboBox_Baudrate,
                                                ui->comboBox_PartBit,
                                                ui->comboBox_DataBits,
                                                ui->comboBox_StopBits,
                                               ui->comboBox_FlowControl);

    connect(ui->pushButton_serialConnect, &QPushButton::clicked, m_serial_controller, &SerialController::onConnectClicked);
    connect(ui->pushButton_serialDisconnect, &QPushButton::clicked, m_serial_controller, &SerialController::onDisconnectClicked);

   //onnect(m_serial_controller, &SerialController::portListUpdate, this, &MainWindow::portListUpdate);

    /* ----------------------- Serial monitor --------------------------*/
    m_serial_monitor = new serialMonitor(this, ui->textBrowser_serial_monitor, ui->pushButton_serialBufferClear);
    connect(m_serial_controller, &SerialController::dataReceived, m_serial_monitor, &serialMonitor::onSerialDataReceived);

    /* ----------------------- Serial plotter --------------------------*/
    // TODO: added two undifined buttons
    m_serial_plotter = new serialPlotter(this,
                                         ui->pushButton_plotter_clear,
                                         ui->pushButton_plotter_save,
                                         ui->pushButton_plotter_stop,
                                         ui->pushButton_plotter_auto,
                                         ui->display_plot,
                                         ui->display_verticalScrollBar,
                                         ui->display_horizontalScrollBar);
    connect(m_serial_monitor, &serialMonitor::newLinesReceived, m_serial_plotter, &serialPlotter::onNewLinesReceived);

    ui->display_plot->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot->yAxis->setRange(0, 10, Qt::AlignCenter);

    /* ----------------------- Serial send monitor --------------------------*/
    m_serial_send_monitor = new serialSendMonitor(ui->pushButton_serialSend, ui->pushButton_clear_send_history, ui->lineEdit_serialSend, ui->textBrowser_send_history, this);
    m_serial_send_monitor->connect_widgets();
    connect(m_serial_send_monitor, &serialSendMonitor::sendCommand, m_serial_controller, &SerialController::onSendCommand);
    connect(m_serial_send_monitor, &serialSendMonitor::sendMessage, m_serial_controller, &SerialController::onSendMessage);


}

MainWindow::~MainWindow()
{
//    delete m_serial_controller;
//    delete m_serial_monitor;
//    delete m_serial_plotter;
    qDebug() <<  "start delete ui";
    delete ui;
    qDebug() <<  "delete ui done";

}
