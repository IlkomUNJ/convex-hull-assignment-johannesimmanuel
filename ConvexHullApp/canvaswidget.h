#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QMouseEvent>
#include <QPainter>

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Helper functions moved to public so the global comparePoints can access them
    int getOrientation(const QPoint &p, const QPoint &q, const QPoint &r);
    int distSq(const QPoint &p1, const QPoint &p2);

protected:
    // Event handler yang dipanggil saat widget perlu digambar ulang
    void paintEvent(QPaintEvent *event) override;
    // Event handler yang dipanggil saat tombol mouse ditekan
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    // Slot untuk membersihkan canvas
    void clearCanvas();
    // Slot untuk menjalankan algoritma convex hull
    void runConvexHull();

private:
    // Fungsi untuk implementasi algoritma Slow Convex Hull (Brute Force)
    void slowConvexHull();
    // Fungsi untuk implementasi algoritma Graham Scan
    void grahamScanConvexHull();

    // Helper untuk Graham Scan: mencari titik dengan y terendah
    QPoint getLowestPoint();

    // Menyimpan semua titik yang ditambahkan oleh pengguna
    QVector<QPoint> points;
    // Menyimpan titik-titik yang membentuk convex hull
    QVector<QPoint> hullPoints;

    // Menyimpan jumlah iterasi untuk perbandingan
    long long slowIterations;
    long long grahamIterations;
};

#endif // CANVASWIDGET_H

