#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "average.h"
#include "timezones.h"
#include "grid.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(TimeZones *tz, QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    TimeZones* m_timezones;
    Grid* m_grid;
    GridThread* m_gridThread;
    gridParameters m_parameters;
    Average* m_average;
    char m_state;

signals :
    void StartGridThread(gridParameters* p, TimeZones* tz);
    void StopGridThread(void);

private slots:
    void UTCOffset(void);
    void GridParametersUpdate(void);
    void GenerateGrid(void);
    void UpdateProgressBar(int counter, int tiles, int refreshTime);
    void GridGenerated(int elapsed, int counter, int tiles, int refreshTime);
};

#endif // MAINWINDOW_H
