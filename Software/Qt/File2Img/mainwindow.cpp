#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a the image widget
    m_graphicBinWidget = new GraphicBin();
    m_graphicBinWidget->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));
    setCentralWidget(m_graphicBinWidget);

    // Create filename label
    m_fileLabel = new QLabel;
    ui->statusBar->insertWidget(0, m_fileLabel, 2);

    // Create width and height spin box
    m_width = new QSpinBox();
    m_height = new QSpinBox();
    ui->statusBar->insertWidget(1, m_width);
    ui->statusBar->insertWidget(2, m_height);

    m_width->setRange(1, INT_MAX);
    m_height->setRange(1, INT_MAX);

    // Set shortcuts
    ui->actionOpen->setShortcut(tr("Ctrl+O"));
    ui->actionPrint->setShortcut(tr("Ctrl+P"));
    ui->actionExit->setShortcut(tr("Ctrl+Q"));

    // Set actions default state
    ui->actionSave->setEnabled(false);
    ui->actionPrint->setEnabled(false);

    // Configure slots
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(Print()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(Open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(Save()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(About()));
    connect(m_width, SIGNAL(valueChanged(int)), this, SLOT(WidthChanged()));

    // Resize the main window
    this->resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool MainWindow::LoadFile(const QString &fileName)
{
    QByteArray buffer;
    int width = 0;
    int height = 0;
    int size = 0;

    // Create file
    QFile file(fileName);

    // Open file
    if(file.open(QIODevice::ReadOnly) == 0)
    {
        return false;
    }

    // Get file size
    size = file.size();

    // Get default width and height
    GetDefaultWidthHeight(size, &width, &height);

    // Read data
    buffer = file.readAll();

    // Buffer not empty ?
    if(buffer.isEmpty())
    {
       return false;
    }

    // Create image from raw data
    m_graphicBinWidget->setImageFromRawData((uchar*) buffer.constData(), width, height);

    // Fit to window
    m_graphicBinWidget->fitImage();

    // Update line edit
    m_width->setValue(width);
    m_height->setValue(height);

    // Enable actions
    ui->actionSave->setEnabled(true);
    ui->actionPrint->setEnabled(true);
    ui->actionFitToWindow->setEnabled(true);

    // Show filename in status bar
    m_fileLabel->setText(fileName);

    return true;
}

void MainWindow::Open(void)
{
    QFileDialog dialog(this, tr("Open File"));

    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !LoadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::Save(void)
{
    QString filter = "PNG (*.png);;Bitmap (*.bmp);;JPEG (*.jpeg)";

    // Open dialog window
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "/", filter);

    // Filename and format are correct ?
    if(fileName.isEmpty() == 0)
    {
        QFile file(fileName);

        // Open file
        file.open(QIODevice::WriteOnly);

        // Split string
        QStringList list = fileName.split(".");

        // Get last string
        QString extension = list.at(list.count()-1).toUpper();

        qDebug("%s | %s", fileName.toLocal8Bit().constData(), extension.toLocal8Bit().constData());

        // Set quality only for JPEG !
        int quality = (extension == "JPEG") ? 100 : -1;

        // Save file
        if(m_graphicBinWidget->getPixmap().save(&file, extension.toLocal8Bit().constData(), quality) == 0)
        {
            qDebug("Save failed !");
        }
    }
}

void MainWindow::Print(void)
{
    QPrintDialog dialog(&printer, this);

    // Show dialog
    if(dialog.exec())
    {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = m_graphicBinWidget->getPixmap().size();

        size.scale(rect.size(), Qt::KeepAspectRatio);

        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(m_graphicBinWidget->getPixmap().rect());
        painter.drawPixmap(0, 0, m_graphicBinWidget->getPixmap());
    }
}

void MainWindow::WidthChanged(void)
{
    int size =  0;
    int width = m_width->value();
    int height = 0;

    // Get size in bytes
    size = m_graphicBinWidget->getImageSize().width() * m_graphicBinWidget->getImageSize().height();

    height = size / width;

    qDebug("Resize! %d : %d", width, height);

    m_graphicBinWidget->setSize(width, height);

    m_width->setValue(width);
    m_height->setValue(height);
}

void MainWindow::About(void)
{
    QMessageBox::about(this, tr("About Image Viewer"), tr("Lorem ipsum"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
