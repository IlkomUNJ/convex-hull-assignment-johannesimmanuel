 #include "canvaswidget.h"
#include <algorithm> // Untuk std::sort
#include <QDebug>

// Titik referensi global untuk sorting pada Graham Scan
static QPoint globalPivot;

// Fungsi komparator untuk sorting titik berdasarkan sudut polar
bool comparePoints(const QPoint &a, const QPoint &b) {
    CanvasWidget tempWidget; // Objek sementara untuk memanggil getOrientation
    int order = tempWidget.getOrientation(globalPivot, a, b);

    if (order == 0) { // Jika kolinear
        return tempWidget.distSq(globalPivot, a) < tempWidget.distSq(globalPivot, b);
    }
    return (order == 2); // 2 berarti counter-clockwise (kiri)
}


CanvasWidget::CanvasWidget(QWidget *parent) : QWidget(parent)
{
    // Set background agar terlihat jelas
    this->setStyleSheet("background-color: white; border: 1px solid black;");
    slowIterations = 0;
    grahamIterations = 0;
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Gambar semua titik
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::NoPen);
    for (const QPoint &point : points) {
        painter.drawEllipse(point, 5, 5);
    }

    // 2. Gambar Convex Hull jika sudah dihitung
    if (!hullPoints.isEmpty()) {
        painter.setPen(QPen(Qt::red, 2));
        painter.drawPolygon(QPolygon(hullPoints));
    }

    // 3. Tulis jumlah iterasi
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(10, 20, QString("Slow Hull Iterations: %1").arg(slowIterations));
    painter.drawText(10, 40, QString("Graham Scan Iterations: %1").arg(grahamIterations));
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        points.append(event->pos());
        // Minta widget untuk digambar ulang
        update();
    }
}

void CanvasWidget::clearCanvas()
{
    points.clear();
    hullPoints.clear();
    slowIterations = 0;
    grahamIterations = 0;
    update(); // Gambar ulang canvas yang kosong
}

void CanvasWidget::runConvexHull()
{
    if (points.size() < 3) return; // Butuh minimal 3 titik

    // Jalankan kedua algoritma untuk perbandingan
    slowConvexHull(); // Ini akan mengisi hullPoints untuk digambar
    grahamScanConvexHull(); // Ini hanya menghitung iterasi untuk perbandingan

    update(); // Gambar ulang untuk menampilkan hull
}

void CanvasWidget::slowConvexHull()
{
    if (points.size() < 3) return;

    QVector<QPoint> localHull;
    slowIterations = 0;

    for (int i = 0; i < points.size(); ++i) {
        for (int j = 0; j < points.size(); ++j) {
            if (i == j) continue;

            bool isEdge = true;
            for (int k = 0; k < points.size(); ++k) {
                if (k == i || k == j) continue;
                slowIterations++; // Hitung iterasi di loop terdalam
                if (getOrientation(points[i], points[j], points[k]) == 2) { // Titik k di sebelah kiri garis ij
                    isEdge = false;
                    break;
                }
            }
            if (isEdge) {
                // Untuk memastikan tidak ada duplikat
                if(!localHull.contains(points[i])) localHull.append(points[i]);
                if(!localHull.contains(points[j])) localHull.append(points[j]);
            }
        }
    }

    // Sort hull points untuk menggambar polygon dengan benar
    globalPivot = getLowestPoint();
    std::sort(localHull.begin(), localHull.end(), comparePoints);

    hullPoints = localHull;
}


void CanvasWidget::grahamScanConvexHull() {
    if (points.size() < 3) return;

    grahamIterations = 0;

    // 1. Cari titik dengan y terendah
    QPoint pivot = getLowestPoint();
    globalPivot = pivot; // Set pivot global untuk komparator

    // 2. Sort titik berdasarkan sudut polar relatif terhadap pivot
    QVector<QPoint> sortedPoints = points;
    std::sort(sortedPoints.begin(), sortedPoints.end(), comparePoints);

    // 3. Buat stack dan push 3 titik pertama
    QVector<QPoint> localHull;
    localHull.push_back(sortedPoints[0]);
    localHull.push_back(sortedPoints[1]);
    localHull.push_back(sortedPoints[2]);

    // 4. Proses sisa titik
    for (int i = 3; i < sortedPoints.size(); ++i) {
        grahamIterations++; // Hitung iterasi
        // Terus hapus titik teratas dari stack selama titik tersebut membuat belokan kanan (clockwise)
        while (localHull.size() > 1 && getOrientation(localHull[localHull.size()-2], localHull.back(), sortedPoints[i]) != 2) {
            localHull.pop_back();
            grahamIterations++; // Iterasi tambahan untuk pop
        }
        localHull.push_back(sortedPoints[i]);
    }
    // Tidak mengubah hullPoints utama agar hasil dari slow hull yang digambar
    // Jika ingin menggambar hasil Graham, uncomment baris di bawah:
    // hullPoints = localHull;
}


int CanvasWidget::getOrientation(const QPoint &p, const QPoint &q, const QPoint &r)
{
    // (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)
    long long val = (long long)(q.y() - p.y()) * (r.x() - q.x()) -
                    (long long)(q.x() - p.x()) * (r.y() - q.y());
    if (val == 0) return 0;  // Collinear
    return (val > 0) ? 1 : 2; // Clockwise or Counter-clockwise
}

int CanvasWidget::distSq(const QPoint &p1, const QPoint &p2)
{
    return (p1.x() - p2.x())*(p1.x() - p2.x()) +
           (p1.y() - p2.y())*(p1.y() - p2.y());
}

QPoint CanvasWidget::getLowestPoint() {
    QPoint lowest = points[0];
    for (int i = 1; i < points.size(); ++i) {
        if (points[i].y() < lowest.y() || (points[i].y() == lowest.y() && points[i].x() < lowest.x())) {
            lowest = points[i];
        }
    }
    return lowest;
}
