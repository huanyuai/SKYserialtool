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
    // connect(ui->windowSize_lineEdit, &QLineEdit::editingFinished, this, &MainWindow::on_windowSize_lineEdit_editingFinished);
    ui->tabWidget->setCurrentIndex(0);

    // Setup connections for individual window size line edits
    connect(ui->lineEdit_windowSize_1, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_1_editingFinished);
    connect(ui->lineEdit_windowSize_2, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_2_editingFinished);
    connect(ui->lineEdit_windowSize_3, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_3_editingFinished);
    connect(ui->lineEdit_windowSize_4, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_4_editingFinished);
    connect(ui->lineEdit_windowSize_5, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_5_editingFinished);
    connect(ui->lineEdit_windowSize_6, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_windowSize_6_editingFinished);

    connect(ui->lineEdit_channelname_1, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_1_editingFinished);
    connect(ui->lineEdit_channelname_2, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_2_editingFinished);
    connect(ui->lineEdit_channelname_3, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_3_editingFinished);
    connect(ui->lineEdit_channelname_4, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_4_editingFinished);
    connect(ui->lineEdit_channelname_5, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_5_editingFinished);
    connect(ui->lineEdit_channelname_6, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelname_6_editingFinished);

    connect(ui->lineEdit_channelpre_1, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_1_editingFinished);
    connect(ui->lineEdit_channelpre_2, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_2_editingFinished);
    connect(ui->lineEdit_channelpre_3, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_3_editingFinished);
    connect(ui->lineEdit_channelpre_4, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_4_editingFinished);
    connect(ui->lineEdit_channelpre_5, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_5_editingFinished);
    connect(ui->lineEdit_channelpre_6, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelpre_6_editingFinished);

    connect(ui->lineEdit_channelsuf_1, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_1_editingFinished);
    connect(ui->lineEdit_channelsuf_2, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_2_editingFinished);
    connect(ui->lineEdit_channelsuf_3, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_3_editingFinished);
    connect(ui->lineEdit_channelsuf_4, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_4_editingFinished);
    connect(ui->lineEdit_channelsuf_5, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_5_editingFinished);
    connect(ui->lineEdit_channelsuf_6, &QLineEdit::editingFinished, this, &MainWindow::on_lineEdit_channelsuf_6_editingFinished);
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

// Slot implementations for each line edit
void MainWindow::on_lineEdit_windowSize_1_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_1->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(1, newWindowSize);  // Assuming the serialPlotter class has this function
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_1->clear();
    }
}

void MainWindow::on_lineEdit_windowSize_2_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_2->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(2, newWindowSize);
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_2->clear();
    }
}

// Continue for other line edits in the same way...
void MainWindow::on_lineEdit_windowSize_3_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_3->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(3, newWindowSize);
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_3->clear();
    }
}

void MainWindow::on_lineEdit_windowSize_4_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_4->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(4, newWindowSize);
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_4->clear();
    }
}

void MainWindow::on_lineEdit_windowSize_5_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_5->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(5, newWindowSize);
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_5->clear();
    }
}

void MainWindow::on_lineEdit_windowSize_6_editingFinished()
{
    bool ok;
    int newWindowSize = ui->lineEdit_windowSize_6->text().toInt(&ok);
    if (ok && newWindowSize > 0) {
        m_serial_plotter->setWindowSize(6, newWindowSize);
    } else {
        qDebug() << "Invalid Input"<< "Please enter a valid positive number.";
        ui->lineEdit_windowSize_6->clear();
    }
}



void MainWindow::on_lineEdit_channelname_1_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_1->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(1, channelName);  // Assuming the serialPlotter class has this function
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_1->clear();
    }
}

void MainWindow::on_lineEdit_channelname_2_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_2->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(2, channelName);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_2->clear();
    }
}

// Continue similarly for other channel name line edits
void MainWindow::on_lineEdit_channelname_3_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_3->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(3, channelName);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_3->clear();
    }
}

void MainWindow::on_lineEdit_channelname_4_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_4->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(4, channelName);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_4->clear();
    }
}

void MainWindow::on_lineEdit_channelname_5_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_5->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(5, channelName);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_5->clear();
    }
}

void MainWindow::on_lineEdit_channelname_6_editingFinished()
{
    QString channelName = ui->lineEdit_channelname_6->text();
    if (!channelName.isEmpty()) {
        m_serial_plotter->setChannelName(6, channelName);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel name.";
        ui->lineEdit_channelname_6->clear();
    }
}

// Similarly for channel prefixes
void MainWindow::on_lineEdit_channelpre_1_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_1->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(1, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_1->clear();
    }
}

// Continue similarly for other channel prefixes
void MainWindow::on_lineEdit_channelpre_2_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_2->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(2, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_2->clear();
    }
}

void MainWindow::on_lineEdit_channelpre_3_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_3->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(3, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_3->clear();
    }
}

void MainWindow::on_lineEdit_channelpre_4_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_4->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(4, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_4->clear();
    }
}

void MainWindow::on_lineEdit_channelpre_5_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_5->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(5, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_5->clear();
    }
}

void MainWindow::on_lineEdit_channelpre_6_editingFinished()
{
    QString channelPrefix = ui->lineEdit_channelpre_6->text();
    if (!channelPrefix.isEmpty()) {
        m_serial_plotter->setChannelPrefix(6, channelPrefix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel prefix.";
        ui->lineEdit_channelpre_6->clear();
    }
}

// Similarly for channel suffixes
void MainWindow::on_lineEdit_channelsuf_1_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_1->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(1, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_1->clear();
    }
}

// Continue similarly for other channel suffixes
void MainWindow::on_lineEdit_channelsuf_2_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_2->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(2, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_2->clear();
    }
}

void MainWindow::on_lineEdit_channelsuf_3_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_3->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(3, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_3->clear();
    }
}

void MainWindow::on_lineEdit_channelsuf_4_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_4->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(4, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_4->clear();
    }
}

void MainWindow::on_lineEdit_channelsuf_5_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_5->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(5, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_5->clear();
    }
}

void MainWindow::on_lineEdit_channelsuf_6_editingFinished()
{
    QString channelSuffix = ui->lineEdit_channelsuf_6->text();
    if (!channelSuffix.isEmpty()) {
        m_serial_plotter->setChannelSuffix(6, channelSuffix);
    } else {
        qDebug() << "Invalid Input" << "Please enter a valid channel suffix.";
        ui->lineEdit_channelsuf_6->clear();
    }
}

