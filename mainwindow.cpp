#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    // Create multi-channel plot widgets
    QList<QCustomPlot*> multiChannelPlots;
    multiChannelPlots << ui->display_plot_1
                      << ui->display_plot_2
                      << ui->display_plot_3
                      << ui->display_plot_4
                      << ui->display_plot_5
                      << ui->display_plot_6;

    // Create serialPlotter instance with multi-channel support
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
                                         ui->display_horizontalScrollBar,
                                         multiChannelPlots);
    connect(m_serial_monitor, &serialMonitor::newLinesReceived, m_serial_plotter, &serialPlotter::onNewLinesReceived);

    ui->display_plot->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_1->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_1->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_2->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_2->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_3->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_3->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_4->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_4->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_5->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_5->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->display_plot_6->xAxis->setRange(0, 6, Qt::AlignLeft);
    ui->display_plot_6->yAxis->setRange(0, 10, Qt::AlignCenter);
    /* ----------------------- Serial send monitor --------------------------*/
    m_serial_send_monitor = new serialSendMonitor(ui->pushButton_serialSend, ui->pushButton_clear_send_history, ui->lineEdit_serialSend, ui->textBrowser_send_history, this);
    m_serial_send_monitor->connect_widgets();
    connect(m_serial_send_monitor, &serialSendMonitor::sendCommand, m_serial_controller, &SerialController::onSendCommand);
    connect(m_serial_send_monitor, &serialSendMonitor::sendMessage, m_serial_controller, &SerialController::onSendMessage);

    connect(ui->checkBox_multiChannelMode, &QCheckBox::stateChanged, this, &MainWindow::onModeChanged);

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

void MainWindow::onModeChanged(int state) {
    if (state == Qt::Checked) {
        m_serial_plotter->setMultiChannelMode(true);
        ui->display_plot->setVisible(false);  // Hide the main plot
        qDebug() << "setMultiChannelMode(true)";
        // Show multi-channel plots
        ui->tabWidget->setCurrentIndex(1);
        ui->display_plot_1->setVisible(true);
        ui->display_plot_2->setVisible(true);
        ui->display_plot_3->setVisible(true);
        ui->display_plot_4->setVisible(true);
        ui->display_plot_5->setVisible(true);
        ui->display_plot_6->setVisible(true);
        // Continue for other plots
    } else {
        m_serial_plotter->setMultiChannelMode(false);
        ui->display_plot->setVisible(true);   // Show the main plot
        // Hide multi-channel plots
        qDebug() << "setMultiChannelMode(false)";
        ui->tabWidget->setCurrentIndex(0);
        ui->display_plot_1->setVisible(false);
        ui->display_plot_2->setVisible(false);
        ui->display_plot_3->setVisible(false);
        ui->display_plot_4->setVisible(false);
        ui->display_plot_5->setVisible(false);
        ui->display_plot_6->setVisible(false);
        // Continue for other plots
    }
}
