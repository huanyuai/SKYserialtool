#include "serialplotter.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

QVector<double> extractNumbers(const QString &input);

serialPlotter::serialPlotter(QObject *parent,
                             QPushButton *clear,
                             QPushButton *save,
                             QPushButton *stop,
                             QPushButton *_auto,
                             QCustomPlot *display_plot,
                             QScrollBar *display_verticalScrollBar,
                             QScrollBar *display_horizontalScrollBar,
                             QList<QCustomPlot*> multiChannelPlots)
    : QObject(parent),
    m_button_clear(clear),
    m_button_save(save),
    m_button_stop(stop),
    m_button_auto(_auto),
    m_display_plot(display_plot),
    m_display_verticalScrollBar(display_verticalScrollBar),
    m_display_horizontalScrollBar(display_horizontalScrollBar),
    m_multiChannelPlots(multiChannelPlots),  // Initialize multi-channel plots
    m_thread(new QThread(this)),
    m_multiChannelMode(false)  // Initialize to false
     // Initialize m_tabWidget

{
    //connect(m_display_horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(display_horzScrollBarChanged(int)));
    //connect(m_display_verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(display_vertScrollBarChanged(int)));
    connect(m_display_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    connect(m_display_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
    connect(m_button_clear, &QPushButton::clicked, this, &serialPlotter::onClearButtonClicked);
    connect(m_button_save, &QPushButton::clicked, this, &serialPlotter::onSaveButtonClicked);
    connect(m_button_stop, &QPushButton::clicked, this, &serialPlotter::onStopButtonClicked);
    connect(m_button_auto, &QPushButton::clicked, this, &serialPlotter::onAutoButtonClicked);


    qRegisterMetaType<PlotDataPtrList>("PlotDataPtrList");

    m_plot_thread.moveToThread(m_thread);

    connect(this, &serialPlotter::newLinesReceived, &m_plot_thread, &plotDataHandlerThread::onNewDataReceived);
    connect(this, &serialPlotter::axisRangeChanged, &m_plot_thread, &plotDataHandlerThread::onAxisChanged);
    connect(&m_plot_thread, &plotDataHandlerThread::curveNumChanged, this, &serialPlotter::onCurveNumChanged);
    connect(&m_plot_thread, &plotDataHandlerThread::readyForPlot, this, &serialPlotter::onReadyForPlot);
    connect(this, &serialPlotter::clearPlotData, &m_plot_thread, &plotDataHandlerThread::onClearPlotData);
    // connect(this, &serialPlotter::clearChannelPlotsData, &m_plot_thread, &plotDataHandlerThread::onClearPlotData);
    connect(this, &serialPlotter::savePlotDataToCSV, &m_plot_thread, &plotDataHandlerThread::onSavePlotDataToCSV);

    connect(m_thread, &QThread::finished, &m_plot_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    m_thread->start();
    //setupDisplayPlot(MAX_GRAPH_NUM);
    setupSingleChannelPlot();

    // configure scroll bars:
    // Since scroll bars only support integer values, we'll set a high default range of -500..500 and
    // divide scroll bar position values by 100 to provide a scroll range -5..5 in floating point
    // axis coordinates. if you want to dynamically grow the range accessible with the scroll bar,
    // just increase the minimum/maximum values of the scroll bars as needed.
    m_display_horizontalScrollBar->setHidden(true);
    m_display_verticalScrollBar->setHidden(true);

    m_button_stop->setText("Stop");
}

serialPlotter::~serialPlotter() {
    qDebug() <<  "deconstruct serial plotter";

    disconnect(m_thread, &QThread::finished, &m_plot_thread, &QObject::deleteLater);
    disconnect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    if (m_thread->isRunning()) {
        m_thread->quit();
        m_thread->wait();
    }
}
//设置多通道独立解析模式
void serialPlotter::setMultiChannelMode(bool enabled) {
    m_multiChannelMode = enabled;
    // Optionally initialize or clear multi-channel plots here
}
void serialPlotter::onNewLinesReceived(const QStringList &lines) {
    if (m_stop) {
        return;
    }
    // store lines when plot data not finished yet.
    m_qeued_lines.append(lines);
    qDebug() <<  "开始判断模式";
    if (m_multiChannelMode) {
        qDebug() <<  "单通道独立绘制模式";
        for (const QString &line : lines) {
            handleSingleChannelData(line);  // 处理单通道数据
        }
    } else {
        qDebug() <<  "多通道混合绘制模式";
        if (m_plot_data_finished) {
            m_x_axis_range_temp = getXAxis();
            m_y_axis_range_temp = getYAxis();
            m_plot_data_finished = false;
            emit newLinesReceived(m_qeued_lines, m_x_axis_range_temp, m_auto);

        }
    }
    m_qeued_lines.clear();

    }


int serialPlotter::getChannelIndex(const QString &channel_id) {
    // Implement logic to map channel identifier to index (0 to 5) corresponding to display_plot_1 to display_plot_6
    if (channel_id.contains("channel1")) return 0;
    if (channel_id.contains("channel2")) return 1;
    if (channel_id.contains("channel3")) return 2;
    if (channel_id.contains("channel4")) return 3;
    if (channel_id.contains("channel5")) return 4;
    if (channel_id.contains("channel6")) return 5;
    // Return -1 if the channel_id is not recognized
    return -1;
}

// Implement helper methods if needed
void serialPlotter::onCurveNumChanged(int new_num) {
    setupDisplayPlot(new_num);
}

void serialPlotter::onReadyForPlot(PlotDataPtrList data, QCPRange xRange, QCPRange yRange, bool auto_mode) {
    //qDebug() << "plot: set data";
    for (int i = 0; i < data.size(); i++) {
        m_display_plot->graph(i)->setData(data[i]);
    }
    //qDebug() << "plot: set axis";
    if (auto_mode) {
        m_display_plot->xAxis->setRange(xRange);
        m_display_plot->yAxis->setRange(yRange);
    }
    else {
        m_display_plot->xAxis->setRange(m_x_axis_range_temp);
        m_display_plot->yAxis->setRange(m_y_axis_range_temp);
    }
    //QElapsedTimer timer;
    //timer.start();
    m_display_plot->replot();
    //qDebug() << "raw plot timer: " << timer.elapsed() << "ms";
    m_plot_data_finished = true;
}

void serialPlotter::xAxisChanged(QCPRange range)
{
  setXAxis(range);
    // make zooming possible when plot stopped
  if (m_x_axis_range_temp != range && m_plot_data_finished && m_stop) {
      m_x_axis_range_temp = getXAxis();
      m_y_axis_range_temp = getYAxis();
      m_plot_data_finished = false;
      emit axisRangeChanged(m_x_axis_range);
  }
}

void serialPlotter::yAxisChanged(QCPRange range)
{
  //qDebug() << "y axis change: " << range;
  setYAxis(range);
}


void serialPlotter::setupDisplayPlot(int numGraphs)
{
    m_display_plot->clearGraphs(); // 清除现有的图表

    // 动态创建图表和数据容器
    for (int i = 0; i < numGraphs; ++i) {
        m_display_plot->addGraph();
        QPen pen(m_pen_colors[i]);
        pen.setWidth(GRAPH_PEN_WIDTH); // 设置线的粗细
        m_display_plot->graph()->setPen(pen);
        //m_display_plot->graph()->setPen(m_pen_colors[i]);
        m_display_plot->legend->setVisible(true);
        // 关闭抗锯齿
        m_display_plot->graph()->setName("通道 "+QString::number(i));
        m_display_plot->graph()->setAntialiased(false);
    }

    m_display_plot->axisRect()->setupFullAxesBox(true);
    m_display_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_display_plot->setNoAntialiasingOnDrag(true);

}
void serialPlotter::setupSingleChannelPlot() {
    // 这里假设每个通道图表都已经在 m_multiChannelPlots 中初始化好了
    qDebug() << "每个通道图表都已经初始化好了 " ;
    for (int i = 0; i < m_multiChannelPlots.size(); ++i) {
        QCustomPlot *plot = m_multiChannelPlots[i];

        // 清除现有的图表数据
        plot->clearGraphs();
        plot->clearItems();

        // 添加一条曲线到每个图表
        plot->addGraph();
        QPen pen(m_pen_colors[i]);
        pen.setWidth(GRAPH_PEN_WIDTH); // 设置线的粗细
        plot->graph(0)->setPen(pen);
        // 设置每条曲线的独特名称
        plot->graph(0)->setName("通道 " + QString::number(i));
        plot->graph(0)->setAntialiased(false);

        // 设置图表的基本配置
        plot->axisRect()->setupFullAxesBox(true);
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        plot->setNoAntialiasingOnDrag(true);


        // 图例设置
        plot->legend->setVisible(true);  // 确保图例可见
        plot->legend->setFont(QFont("Arial", 8));  // 设置图例的字体及大小
        plot->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));  // 设置图例的背景色及透明度
        plot->legend->setBorderPen(QPen(Qt::black));  // 设置图例边框
        plot->legend->setIconSize(QSize(10, 10));  // 设置图例图标的大小

    }

    // 可选：隐藏其他图表（如多通道合并图表）或者进行其他初始化
}
void serialPlotter::handleSingleChannelData(const QString &line) {
    qDebug() << "Received line: " << line;

    QStringList parts = line.split(",");
    if (parts.size() < 3) {
        qDebug() << "Invalid data format, skipping";
        return;
    }

    QString channelPrefix = parts.first();
    QString channelSuffix = parts.last();
    qDebug() << "channelPrefix: " << channelPrefix;
    qDebug() << "channelSuffix: " << channelSuffix;
    if (channelPrefix != channelSuffix) {
        qDebug() << "Prefix and suffix do not match, skipping";
        return;
    }

    QStringList values = parts.mid(1, parts.size() - 2);
    QVector<double> yData;
    for (const QString &val : values) {
        bool ok;
        double value = val.toDouble(&ok);
        if (ok) {
            yData.append(value);
        } else {
            qDebug() << "Invalid value: " << val;
        }
    }

    int channelIndex = getChannelIndex(channelPrefix);
    if (channelIndex >= 0 && channelIndex < m_multiChannelPlots.size()) {
        QCustomPlot *plot = m_multiChannelPlots[channelIndex];
        if (!yData.isEmpty()) {
            qDebug() << "Updating plot for channel: " << channelIndex << " with data: " << yData;
            updatePlotData(yData, plot);
        }
    }else{
        qDebug() << "Invalid channel: " << channelIndex;
    }

}

void serialPlotter::updatePlotData(const QVector<double> &yData, QCustomPlot *plot) {
    if (!plot || yData.isEmpty()) {
        return;
    }

    // 设置固定显示的窗口长度（例如100个数据点）
    const int windowSize = 10;  // 调整这个值来控制显示周期的长度

    // 获取当前数据点的 x 坐标起点
    double startX = plot->graph(0)->dataCount();  // 获取当前数据点数量作为 x 起点
    // 动态计算总数据点数
    int currentDataCount = plot->graph(0)->dataCount() + yData.size();

    // 判断是否需要采样输出
    if (currentDataCount > 2 * PLOT_BUFFER_SIZE) {
        // 进行采样处理
        QVector<double> sampledYData = performDataSampling(yData, currentDataCount);
        for (int i = 0; i < sampledYData.size(); ++i) {
            plot->graph(0)->addData(startX + i, sampledYData[i]);  // 将 x 坐标设为起点 + i
        }
    } else {
        // 不需要采样，直接绘制所有数据
        for (int i = 0; i < yData.size(); ++i) {
            plot->graph(0)->addData(startX + i, yData[i]);  // 将 x 坐标设为起点 + i
        }
    }


    // 计算新的 x 轴范围
    double xAxisUpperBound = plot->graph(0)->dataCount();  // x 轴上限为当前数据点数量
    double xAxisLowerBound = qMax(0.0, xAxisUpperBound - windowSize);  // x 轴下限为上限减去窗口大小

    // 设置 x 轴的范围为滑动窗口范围
    plot->xAxis->setRange(xAxisLowerBound, xAxisUpperBound);

    // 计算滑动窗口内的 y 轴范围
    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::lowest();

    for (int i = 0; i < plot->graph(0)->dataCount(); ++i) {
        double x = plot->graph(0)->data()->at(i)->key;
        if (x >= xAxisLowerBound && x <= xAxisUpperBound) {
            double y = plot->graph(0)->data()->at(i)->value;
            yMin = qMin(yMin, y);
            yMax = qMax(yMax, y);
        }
    }

    // 设定 y 轴范围
    plot->yAxis->setRange(yMin, yMax);

    // 重新绘制图表以显示更新后的数据
    plot->replot();
}

QVector<double> serialPlotter::performDataSampling(const QVector<double> &yData, int currentDataCount) {
    int samplingRate = currentDataCount / (2 * PLOT_BUFFER_SIZE);  // 计算采样率
    QVector<double> sampledYData;

    // 采样数据
    for (int i = 0; i < yData.size(); i += samplingRate) {
        sampledYData.append(yData[i]);
    }

    return sampledYData;
}


/*
Auto Mode:
xAxis 最新的1000组数据 range: (n-1000, n)
yAxis range: (min, max) of y(n-1000, n)

*/

void serialPlotter::onSaveButtonClicked() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Plot Data", "", "CSV files (*.csv)");
    if (!fileName.isEmpty()) {
        emit savePlotDataToCSV(fileName);
    }
}

void serialPlotter::onClearButtonClicked() {
    // quit plot thread
    if(m_multiChannelMode)
    {for (int i = 0; i < m_multiChannelPlots.size(); ++i) {
        QCustomPlot *plot = m_multiChannelPlots[i];
        plot->clearGraphs();
        // emit clearPlotData();
        plot->replot();
        }
        setupSingleChannelPlot();
    }else{
        // 清除现有的图表数据
        m_display_plot->clearGraphs();
        emit clearPlotData();
        m_display_plot->replot();
    }

}

void serialPlotter::onStopButtonClicked() {
    m_stop = !m_stop;
    if (m_stop) {
        m_button_stop->setText("Run");
        m_button_save->setEnabled(true);
    }
    else {
        m_button_stop->setText("Stop");
        m_button_save->setEnabled(false);
    }
}

void serialPlotter::onAutoButtonClicked() {
    m_auto = !m_auto;
}
