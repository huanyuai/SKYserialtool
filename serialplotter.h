#ifndef SERIALPLOTTER_H
#define SERIALPLOTTER_H

#include <QObject>
#include "qcustomplot.h"
#include <QDebug>
#include <QThread>
#include <atomic>
#include "plotdatahandlerthread.h"

#define MAX_GRAPH_NUM 5 // plot max 5 curves
#define GRAPH_PEN_WIDTH 2

class serialPlotter : public QObject
{
    Q_OBJECT
public:
    serialPlotter(QObject *parent,
                  QPushButton *clear,
                  QPushButton *save,
                  QPushButton *stop,
                  QPushButton *_auto,
                  QCustomPlot *display_plot,
                  QScrollBar *display_verticalScrollBar,
                  QScrollBar *display_horizontalScrollBar,
                  QList<QCustomPlot*> multiChannelPlots);  // Constructor updated to include multi-channel plots

    ~serialPlotter();

    void setMultiChannelMode(bool enabled);  // Method to toggle between original and multi-channel modes

signals:
    void newLinesReceived(const QStringList lines, QCPRange xRange, bool auto_mode = true);
    void newLinesReceivedForChannel(int channelIndex, const QVector<double> &yValues, QCPRange xRange, bool auto_mode);
    void axisRangeChanged(QCPRange xRange);
    void clearPlotData();
    // void clearChannelPlotsData();
    void savePlotDataToCSV(const QString &fileName);

public slots:
    void onNewLinesReceived(const QStringList &lines);

    void xAxisChanged(QCPRange range);
    void yAxisChanged(QCPRange range);

    void onSaveButtonClicked();
    void onClearButtonClicked();
    void onStopButtonClicked();
    void onAutoButtonClicked();

    void onCurveNumChanged(int new_num);
    void onReadyForPlot(PlotDataPtrList data, QCPRange xRange, QCPRange yRange, bool auto_mode);

private:
    void setupDisplayPlot(int numGraphs);
    void setupSingleChannelPlot();
    int getChannelIndex(const QString &channel_id);
    void setXAxis(QCPRange range) { m_x_axis_range = range; };
    void setYAxis(QCPRange range) { m_y_axis_range = range; };
    void handleSingleChannelData(const QString &line);
    void updatePlotData(const QVector<double> &yData, QCustomPlot *plot);
    QVector<double> performDataSampling(const QVector<double> &yData, int currentDataCount);
    QCPRange getXAxis() { return m_x_axis_range; };
    QCPRange getYAxis() { return m_y_axis_range; };

    // void updatePlotData(const QVector<double>& values, PlotDataPtrList& plot_data, QCustomPlot* plot); // Helper function to update plot data
    QVector<double> extractChannelData(const QString& line, const QString& prefix, const QString& suffix); // Helper function to extract multi-channel data
    QVector<double> extractOriginalData(const QString& line); // Helper function to extract data for the original protocol

    QList<QColor> m_pen_colors = {Qt::red,
                                  Qt::green,
                                  Qt::blue,
                                  Qt::cyan,
                                  Qt::magenta,
                                  Qt::black,
                                  Qt::darkGray,
                                  Qt::gray,
                                  Qt::lightGray,
                                  Qt::yellow,
                                  Qt::darkRed,
                                  Qt::darkGreen,
                                  Qt::darkBlue,
                                  Qt::darkCyan,
                                  Qt::darkMagenta,
                                  Qt::darkYellow,
                                  Qt::transparent};
    QCustomPlot *m_display_plot;
    QScrollBar *m_display_verticalScrollBar;
    QScrollBar *m_display_horizontalScrollBar;

    QList<QCustomPlot*> m_multiChannelPlots;  // List of plots for multi-channel mode

    QPushButton *m_button_clear;
    QPushButton *m_button_save;
    QPushButton *m_button_stop;
    QPushButton *m_button_auto;

    bool m_stop = false;
    bool m_auto = true;
    bool m_multiChannelMode = false;  // Variable to track the current mode

    QCPRange m_x_axis_range;
    QCPRange m_y_axis_range;

    QCPRange m_x_axis_range_temp;
    QCPRange m_y_axis_range_temp;

    plotDataHandlerThread m_plot_thread;
    QThread *m_thread;

    std::atomic_bool m_plot_data_finished = true;
    QStringList m_qeued_lines;

};

#endif // SERIALPLOTTER_H
