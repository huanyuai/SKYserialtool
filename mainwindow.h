#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "qcustomplot.h"
#include "serialController.h"
#include "serialmonitor.h"
#include "serialplotter.h"
#include "serialsendmonitor.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
void onModeChanged(int state);  // Slot for handling mode change


// Slots for individual line edits
void on_lineEdit_windowSize_1_editingFinished();
void on_lineEdit_windowSize_2_editingFinished();
void on_lineEdit_windowSize_3_editingFinished();
void on_lineEdit_windowSize_4_editingFinished();
void on_lineEdit_windowSize_5_editingFinished();
void on_lineEdit_windowSize_6_editingFinished();

void on_lineEdit_channelname_1_editingFinished();
void on_lineEdit_channelname_2_editingFinished();
void on_lineEdit_channelname_3_editingFinished();
void on_lineEdit_channelname_4_editingFinished();
void on_lineEdit_channelname_5_editingFinished();
void on_lineEdit_channelname_6_editingFinished();

void on_lineEdit_channelpre_1_editingFinished();
void on_lineEdit_channelpre_2_editingFinished();
void on_lineEdit_channelpre_3_editingFinished();
void on_lineEdit_channelpre_4_editingFinished();
void on_lineEdit_channelpre_5_editingFinished();
void on_lineEdit_channelpre_6_editingFinished();

void on_lineEdit_channelsuf_1_editingFinished();
void on_lineEdit_channelsuf_2_editingFinished();
void on_lineEdit_channelsuf_3_editingFinished();
void on_lineEdit_channelsuf_4_editingFinished();
void on_lineEdit_channelsuf_5_editingFinished();
void on_lineEdit_channelsuf_6_editingFinished();

private:
    Ui::MainWindow *ui;

    SerialController *m_serial_controller;
    serialMonitor *m_serial_monitor;
    serialPlotter *m_serial_plotter;
    // parameterManager *m_parameter_manager;
    serialSendMonitor *m_serial_send_monitor;
};
#endif // MAINWINDOW_H
