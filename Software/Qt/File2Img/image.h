#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMenu>

// Default zoom factors
#define DEFAULT_ZOOM_FACTOR      1.15
#define DEFAULT_ZOOM_CTRL_FACTOR 1.01

class GraphicBin : public QGraphicsView
{
    Q_OBJECT

public:

    GraphicBin(bool isContextualMenu = true, QWidget * parent=0);
    ~GraphicBin();

    void setImage(const QImage & image);
    void setImageFromRawData(const uchar * data, int width, int height, bool mirrorHorizontally = false, bool mirrorVertically = false);
    void setZoomFactor(const double factor) {zoomFactor=factor;}
    void setZoomCtrlFactor(const double factor) {zoomCtrlFactor=factor;}
    void setSize(int width, int height);
    QPixmap getPixmap(void);
    QSize getImageSize(void);

public slots:

    void fitImage();

protected:

    virtual void drawOnImage(QPainter* painter, QSize imageSize);
    virtual void drawInViewPort(QPainter* painter, QSize portSize);
    virtual void drawForeground(QPainter *painter, const QRectF &rect);
    virtual QString setToolTipText(QPoint imageCoordinates);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

protected slots:

    virtual void showContextMenu(const QPoint & pos);

private:

    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem;
    QSize imageSize;
    double zoomFactor;
    double zoomCtrlFactor;
    QPixmap pixmap;

};

#endif // IMAGE_H
