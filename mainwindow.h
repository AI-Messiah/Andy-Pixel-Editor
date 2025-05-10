#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QPen>
#include <QBrush>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QMap>
#include <QCloseEvent>
#include <QMessageBox>

#include <stdint.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct myvec{
    int16_t x, y;

    bool operator==(const myvec& other) const {
        return x == other.x && y == other.y;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void MyUpdate();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *eventMove);
    void mousePressEvent(QMouseEvent *eventPress);
    void mouseReleaseEvent(QMouseEvent *releaseEvent);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent (QCloseEvent *event);
private slots:
    void on_horizontalScrollBar_valueChanged(int value);
    void on_verticalScrollBar_valueChanged(int value);

    void on_btn_sel_clicked();

    void on_spin_red_valueChanged(int arg1);

    void on_spin_grn_valueChanged(int arg1);

    void on_spin_blu_valueChanged(int arg1);

    void on_spin_alp_valueChanged(int arg1);

    void on_spin_red_editingFinished();

    void on_spin_grn_editingFinished();

    void on_spin_blu_editingFinished();

    void on_spin_alp_editingFinished();

    void on_btn_opn_clicked();

    void on_btn_sav_clicked();

    void on_btn_saa_clicked();

    void on_btn_lin_clicked();

private:
    void setColor();
    void fillRegion(int x, int y);
    QString trim(QString inv);
    void cutPart();
    bool isSaved = true;
    QMap<QString, bool> extensions;
    QString projPath;
    QString projExt;
    QString projName;
    int red = 0, grn = 0, blu = 0, alp = 0;
    QColor fillColor;
    bool fullShape = false;
    QVector<myvec> selection;
    bool selecting = false;

    bool mdown = false;

    bool hasImage = false;
    bool flash = false;
    int oldx, oldy;
    int smaxx, smaxy;
    int boxx = 0, boxy = 0;
    int xfrm = 0, yfrm = 0;
    Ui::MainWindow *ui;
    QImage specImg;
    QImage lrgCheck;
    QImage smlCheck;
    QImage workImage;
    QImage section;
    QTimer *timer;
};
#endif // MAINWINDOW_H
