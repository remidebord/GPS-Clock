#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(TimeZones* tz, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Fixed size
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    // TimeZones object
    m_timezones = tz;

    // Grid object
    m_grid = new Grid();

    // GridThread object
    m_gridThread = new GridThread();

    // Average size
    m_average = new Average(100);

    QString msg = "© 2016 Rémi Debord - (";
    msg += "Polygons: " + QString::number(tz->PolygonsCount()) + ", ";
    msg += "Timezones: " + QString::number(tz->TimezonesCount()) + "/";
    msg += QString::number((tz->TimezonesCount() + tz->MissingTimezonesCount())) + ")";

    ui->statusBar->showMessage(msg);

    // Slots
    connect(ui->pushButtonFind, SIGNAL(clicked()), this, SLOT(UTCOffset()));
    connect(ui->lineEditEarthRadius, SIGNAL(textChanged(QString)), this, SLOT(GridParametersUpdate()));
    connect(ui->spinBoxTiles, SIGNAL(valueChanged(int)), this, SLOT(GridParametersUpdate()));
    connect(ui->pushButtonGenerate, SIGNAL(clicked(bool)), this, SLOT(GenerateGrid()));
    connect(m_gridThread, SIGNAL(Processing(int,int,int)), this, SLOT(UpdateProgressBar(int,int,int)));
    connect(m_gridThread, SIGNAL(Processed(int,int,int,int)), this, SLOT(GridGenerated(int,int,int,int)));
    connect(this, SIGNAL(StartGridThread(gridParameters*, TimeZones*)), m_gridThread, SLOT(startc(gridParameters*, TimeZones*)));
    connect(this, SIGNAL(StopGridThread()), m_gridThread, SLOT(stop()));

    // Set validator on earth radius line edit
    ui->lineEditEarthRadius->setValidator(new QIntValidator(1, 1000000, this));

    // Display default values
    this->GridParametersUpdate();

    // Init value(s)
    m_state = 0;
}

void MainWindow::GenerateGrid(void)
{
    if(m_state)
    {
        ui->pushButtonGenerate->setText("Start");
        emit StopGridThread();

        // Unprotect shared variables
        ui->groupBoxTest->setEnabled(1);
    }
    else
    {
        ui->pushButtonGenerate->setText("Stop");
        emit StartGridThread(&m_parameters, m_timezones);

        // Protect shared variables
        ui->groupBoxTest->setEnabled(0);
    }

    m_state = (m_state == 0) ? 1 : 0;
}

void MainWindow::GridParametersUpdate(void)
{
    char result = 0;

    m_parameters.earthRadius = ui->lineEditEarthRadius->text().toInt();
    m_parameters.tiles = ui->spinBoxTiles->value();

    result = m_grid->update(&m_parameters);

    if(result)
    {
        ui->lineEditEarthSurface->setText(QString::number(m_parameters.earthSurface));
        ui->lineEditTilesX->setText(QString::number(m_parameters.tilesX));
        ui->lineEditTilesY->setText(QString::number(m_parameters.tilesY));
        ui->lineEditPrecision->setText(QString::number(m_parameters.precision, 'f', 3));
    }
    else
    {
        ui->lineEditEarthSurface->clear();
        ui->lineEditTilesX->clear();
        ui->lineEditTilesY->clear();
        ui->lineEditPrecision->clear();
    }
}

void MainWindow::UTCOffset(void)
{
    timezone tz;

    double latitude = ui->doubleSpinBoxLatitude->value();
    double longitude = ui->doubleSpinBoxLongitude->value();

    //m_timezones->FindTimezone(latitude, longitude, &tz);
    m_timezones->FindTimezoneFast(latitude, longitude, &tz);

    ui->lineEditTZID->setText(tz.tzid);
    ui->lineEditUTCOffset->setText(QString::number(tz.offset));
}

void MainWindow::UpdateProgressBar(int counter, int tiles, int refreshTime)
{
    QString tmp = "";
    static int oldCounter = 0;
    int remainingTime = 0;
    float speed = 0;
    s_time t;

    // Compute speed (tiles / second)
    speed = (counter - oldCounter) * ((float) refreshTime / 1000);

    // Compute remaining time in seconds
    remainingTime = (tiles - counter) / speed;

    // Insert value into in an array
    m_average->insert(remainingTime);

    // Get average value
    remainingTime = m_average->get();

    // Compute time (h:m:s)
    t.hour = (remainingTime / HOUR_SEC);
    t.minute = (remainingTime % HOUR_SEC) / 60;
    t.second = remainingTime % 60;

    // Construct display string
    tmp.sprintf("%d / %d (Time remaining: %02d:%02d:%02d)", counter, tiles, t.hour, t.minute, t.second);

    // Display string
    ui->labelProcessing->setText(tmp);

    // Update progress bar value
    ui->progressBar->setValue(counter * ui->progressBar->maximum() / tiles);

    oldCounter = (oldCounter <= counter) ? counter : 0;
}

void MainWindow::GridGenerated(int elapsed, int counter, int tiles, int refreshTime)
{
    QMessageBox msgBox;
    QString tmp = "";
    s_time t;

    // Information icon
    msgBox.setIcon(QMessageBox::Information);

    // Convert in seconds
    elapsed = elapsed * ((float) refreshTime / 1000);

    // Compute time (h:m:s)
    t.hour = (elapsed / HOUR_SEC);
    t.minute = (elapsed % HOUR_SEC) / 60;
    t.second = elapsed % 60;

    // Update progress bar value
    ui->progressBar->setValue(counter * ui->progressBar->maximum() / tiles);

    tmp.sprintf("%d / %d (Time elapsed: %02d:%02d:%02d)", counter, tiles, t.hour, t.minute, t.second);

    // Set window title
    msgBox.setWindowTitle("Processing done !");

    // Add text (html)
    msgBox.setText(tmp);

    msgBox.exec();

    // Reset progress bar
    ui->progressBar->setValue(0);

    // Reset button
    ui->pushButtonGenerate->setText("Start");

    // Reset state value
    m_state = 0;

    // Reset label
    ui->labelProcessing->setText("-");

    // Unprotect shared variables
    ui->groupBoxTest->setEnabled(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
