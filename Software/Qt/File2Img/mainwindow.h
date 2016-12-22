#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtPrintSupport>
#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool LoadFile(const QString &);
    ~MainWindow();

private slots:

    void Open(void);
    void Save(void);
    void Print(void);
    void WidthChanged(void);
    void About(void);

private:

    Ui::MainWindow *ui;
    GraphicBin *m_graphicBinWidget;
    QLabel *m_fileLabel;
    QSpinBox *m_width;
    QSpinBox *m_height;
    double m_scaleFactor;

    QPrinter printer;
};

#endif // MAINWINDOW_H
