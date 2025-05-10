#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(MyUpdate()));
    timer->start(500);

    extensions.insert("bmp", true);
    extensions.insert("gif", false);
    extensions.insert("jpg", true);
    extensions.insert("jpeg", true);
    extensions.insert("png", true);
    extensions.insert("pbm", false);
    extensions.insert("pgm", false);
    extensions.insert("ppm", true);
    extensions.insert("xbm", true);
    extensions.insert("xpm", true);

    fillColor = QColor(QRgba64::fromArgb32(qRgba(0, 0, 0, 0)));

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm, *.xbm *.xpm)"));
    dialog.setViewMode(QFileDialog::Detail);

    QString aPath = QCoreApplication::applicationDirPath();
    aPath = trim(aPath);
    aPath = trim(aPath);
    aPath = trim(aPath) + "/images/";
    specImg.load(aPath + "spectrum.png");
    lrgCheck.load(aPath + "checker.png");
    smlCheck.load(aPath + "smlcheck.png");

    QString fileName = dialog.getOpenFileName();
    QImage samp;
    if(samp.load(fileName)){
        projName = fileName;
        projExt = projName.mid(projName.lastIndexOf(".") + 1, 4).toLower();
        ui->btn_sav->setEnabled(extensions[projExt]);
        projName = projName.left(projName.lastIndexOf("."));
        int slanum = projName.lastIndexOf("/");
        projPath = projName.left(slanum);
        projName = projName.mid(slanum + 1, projName.length() - (slanum + 1));
        workImage = samp.convertToFormat(QImage::Format_ARGB32);

        hasImage = true;
        if (samp.width() > 768){
            ui->horizontalScrollBar->setMaximum(samp.width() - 768);
        }else{
            ui->horizontalScrollBar->setMaximum(0);
        }
        if (samp.height() > 768){
            ui->verticalScrollBar->setMaximum(samp.height() - 768);
        }else{
            ui->verticalScrollBar->setMaximum(0);
        }
        smaxx = ui->horizontalScrollBar->maximum();
        smaxy = ui->verticalScrollBar->maximum();
        cutPart();
        QWidget::setWindowTitle("Pixel Edit - " + projName);
        QWidget::repaint();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MyUpdate()
{
    flash = !flash;
    QWidget::repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, lrgCheck);
    painter.drawImage(784, 0, smlCheck);
    painter.drawImage(912, 272, specImg);
    QPen drpen;
    QBrush drbrush;
    drbrush.setStyle(Qt::SolidPattern);
    drbrush.setColor(fillColor);
    drpen.setColor(fillColor);
    painter.setPen(drpen);
    painter.drawRect(912, 272, 128, 96);
    painter.fillRect(912, 272, 128, 96, drbrush);
    if (hasImage){

        painter.drawImage(0, 0, section);


        if (flash){
            drpen.setColor(QColor::fromRgb(255, 255, 0));
        }else{
            drpen.setColor(QColor::fromRgb(0, 0, 0));
        }
        painter.setPen(drpen);
        painter.drawRect(boxx - 1, boxy - 1, 34, 34);
        for (int i = 0;i < selection.size();i++){
            painter.drawPoint(selection[i].x - xfrm, selection[i].y - yfrm);
        }
        //painter.setBrush(Qt::SolidPattern);
        for (int y = 0;y < 32;y++){
            for (int x = 0;x < 32;x++){
                if (xfrm + boxx + x < workImage.width() && yfrm + boxy + y < workImage.height()){
                    QColor sample = workImage.pixelColor(xfrm + boxx + x, yfrm + boxy + y);

                    drpen.setColor(sample);
                    drbrush.setColor(sample);

                    painter.setPen(drpen);
                    painter.setBrush(drbrush);
                    painter.drawRect(x * 8 + 784, y * 8, 8, 8);
                    painter.fillRect(x * 8 + 784, y * 8, 8, 8, drbrush);
                }
            }
        }
        if (flash){
            drpen.setColor(QColor::fromRgb(255, 255, 0));
            drbrush.setColor(QColor::fromRgb(255, 255, 0));
            painter.setPen(drpen);
        }else{
            drpen.setColor(QColor::fromRgb(0, 0, 0));
            drbrush.setColor(QColor::fromRgb(0, 0, 0));
            painter.setPen(drpen);
        }
        for (int i = 0;i < selection.size();i++){
            int sx = selection[i].x - (xfrm + boxx);
            int sy = selection[i].y - (yfrm + boxy);
            if (sx >= 0 && sx < 32 && sy >= 0 && sy < 32){
                painter.drawRect(sx * 8 + 784, sy * 8, 8, 8);
                painter.fillRect(sx * 8 + 784, sy * 8, 8, 8, drbrush);
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *eventMove)
{
    if (mdown){
        boxx += eventMove->pos().x() - oldx;
        boxy += eventMove->pos().y() - oldy;
        oldx = eventMove->pos().x();
        oldy = eventMove->pos().y();
        if (boxx < 0) boxx = 0;
        if (boxy < 0) boxy = 0;
        if (boxx > 735) boxx = 735;
        if (boxy > 735) boxy = 735;
        QWidget::repaint();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *eventPress)
{
    if (eventPress->pos().x() >= boxx - 1 && eventPress->pos().x() < boxx + 34 && eventPress->pos().y() >= boxy - 1 && eventPress->pos().y() < boxy + 34){
        mdown = true;
        oldx = eventPress->pos().x();
        oldy = eventPress->pos().y();
    }else if (eventPress->pos().x() >= 784 && eventPress->pos().y() < 256){
        int x = (eventPress->pos().x() - 784) / 8 + boxx + xfrm;
        int y = eventPress->pos().y() / 8 + boxy + yfrm;
        if (x < workImage.width() && y < workImage.height()){
            if (selecting && fullShape){
                fillRegion(x, y);
            }else{
                myvec rep;
                rep.x = x;
                rep.y = y;
                if (selection.size() < 1){
                    selection.push_back(rep);
                }else{
                    selection[0] = rep;
                }
                QWidget::repaint();
            }
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
    mdown = false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (selecting){
        bool cont = false;
        myvec vec = selection[selection.size() - 1];
        switch(event->key()){
        case Qt::Key_Left:
            vec.x--;
            boxx--;
            cont = true;
            break;
        case Qt::Key_Right:
            vec.x++;
            boxx++;
            cont = true;
            break;
        case Qt::Key_Up:
            vec.y--;
            boxy--;
            cont = true;
            break;
        case Qt::Key_Down:
            vec.y++;
            boxy++;
            cont = true;
            break;
        }
        if (boxx < 0){
            xfrm--;
            boxx = 0;
            if (xfrm < 0){
                xfrm = 0;
            }
            ui->horizontalScrollBar->setValue(xfrm);
        }
        if (boxx > 735){
            xfrm++;
            boxx = 735;
            if (xfrm > smaxx){
                xfrm = ui->horizontalScrollBar->maximum();
            }
            ui->horizontalScrollBar->setValue(xfrm);
        }
        if (boxy < 0){
            yfrm--;
            boxy = 0;
            if (yfrm < 0){
                yfrm = 0;
            }
            ui->verticalScrollBar->setValue(yfrm);
        }
        if (boxy > 735){
            yfrm++;
            boxy = 735;
            if (yfrm > smaxy){
                yfrm = ui->verticalScrollBar->maximum();
            }
            ui->verticalScrollBar->setValue(yfrm);
        }
        if (cont && vec.x >= 0 && vec.x < workImage.width() && vec.y >= 0 && vec.y < workImage.height()){
            selection.push_back(vec);
            for (int i=0;i<selection.size() - 1;i++){
                if (selection[i].x == vec.x && selection[i].y == vec.y){
                    ui->dispLbl->setText("Filled");
                    fullShape = true;
                }
            }
            QWidget::repaint();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!isSaved){
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Pixel Edit",
                                                                   tr("Image has not been saved. Are you sure?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
        }
    }else{
        event->accept();
    }
}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    xfrm = value;
    cutPart();
    QWidget::repaint();
}


void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
    yfrm = value;
    cutPart();
    QWidget::repaint();
}

QString MainWindow::trim(QString inv)
{
    return inv.left(inv.lastIndexOf("/"));
}

void MainWindow::cutPart()
{
    QRect myreg(xfrm, yfrm, 768, 768);
    section = workImage.copy((myreg));
}


void MainWindow::on_btn_sel_clicked()
{
    if (selection.size() > 0){
        selecting = !selecting;
        if (!selecting){
            if (selection.size() > 1) selection.clear();
        }
    }
    ui->centralwidget->setFocus();
}

void MainWindow::fillRegion(int x, int y)
{
    myvec avec;
    avec.x = x;
    avec.y = y;
    if (selection.contains(avec) || workImage.pixelColor(avec.x, avec.y) == fillColor)
    {
        return;
    }
    for (int i=0;i<selection.size();i++){
        workImage.setPixelColor(selection[i].x, selection[i].y, fillColor);

    }
    selection.clear();
    int dirx[] = {1, 0, -1, 0};
    int diry[] = {0, 1, 0, -1};

    QVector<myvec> fillPars;
    fillPars.push_back(avec);
    myvec svec;
    myvec tvec;
    int wwidth = workImage.width();
    int wheight = workImage.height();
    while(fillPars.size() > 0){
        svec = fillPars[0];
        fillPars.pop_front();
        workImage.setPixelColor(svec.x, svec.y, fillColor);

        for (int i = 0;i<4;i++){
            tvec.x = svec.x + dirx[i];
            tvec.y = svec.y + diry[i];
            if (tvec.x >= 0 && tvec.x < wwidth && tvec.y >= 0 && tvec.y < wheight){
                if (workImage.pixelColor(tvec.x, tvec.y) != fillColor && !fillPars.contains(tvec)) fillPars.push_back(tvec);
            }
        }
    }
    ui->dispLbl->setText("");
    fullShape = false;
    selecting = false;
    isSaved = false;
    cutPart();
    QWidget::setWindowTitle("Pixel Edit - *" + projName);
    QWidget::repaint();
}


void MainWindow::on_spin_red_valueChanged(int arg1)
{
    red = arg1;
    setColor();
    QWidget::repaint();
}


void MainWindow::on_spin_grn_valueChanged(int arg1)
{
    grn = arg1;
    setColor();
    QWidget::repaint();
}

void MainWindow::on_spin_blu_valueChanged(int arg1)
{
    blu = arg1;
    setColor();
    QWidget::repaint();
}

void MainWindow::on_spin_alp_valueChanged(int arg1)
{
    alp = arg1;
    setColor();
    QWidget::repaint();
}

void MainWindow::on_spin_red_editingFinished()
{
    ui->centralwidget->setFocus();
}

void MainWindow::on_spin_grn_editingFinished()
{
    ui->centralwidget->setFocus();
}

void MainWindow::on_spin_blu_editingFinished()
{
    ui->centralwidget->setFocus();
}

void MainWindow::on_spin_alp_editingFinished()
{
    ui->centralwidget->setFocus();
}

void MainWindow::setColor()
{
    fillColor = QColor::fromRgba(qRgba(red, grn, blu, alp));
}

void MainWindow::on_btn_opn_clicked()
{
    QFileDialog dialog(this);
    if (!isSaved && hasImage){
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Pixel Edit",
                                                                   tr("Would you like to save before opening a file?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
        if (resBtn == QMessageBox::Cancel) return;

        if (resBtn == QMessageBox::Yes) {

            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setNameFilter(tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm, *.xbm *.xpm)"));
            dialog.setViewMode(QFileDialog::Detail);
            dialog.selectFile(projName);
            QString fileName = dialog.getSaveFileName(this, "Save As", projPath + "/" + projName, tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm, *.xbm *.xpm)"));
            isSaved = workImage.save(fileName);
            if (isSaved){
                ui->btn_sav->setEnabled(true);
                projName = fileName;
                projExt = projName.mid(projName.lastIndexOf(".") + 1, 4);

                projName = projName.left(projName.lastIndexOf(".")).toLower();
                int slanum = projName.lastIndexOf("/");
                projPath = projName.left(slanum);
                projName = projName.mid(slanum + 1, projName.length() - (slanum + 1));
                QWidget::setWindowTitle("Pixel Edit - " + projName);
            }
        }
    }

    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm, *.xbm *.xpm)"));
    dialog.setViewMode(QFileDialog::Detail);
    projName = dialog.getOpenFileName();
    QImage samp;
    if(samp.load(projName)){
        projExt = projName.mid(projName.lastIndexOf(".") + 1, 4).toLower();
        ui->btn_sav->setEnabled(extensions[projExt]);
        projName = projName.left(projName.lastIndexOf("."));
        int slanum = projName.lastIndexOf("/");
        projPath = projName.left(slanum);
        projName = projName.mid(slanum + 1, projName.length() - (slanum + 1));
        workImage = samp.convertToFormat(QImage::Format_ARGB32);

        hasImage = true;
        if (samp.width() > 768){
            ui->horizontalScrollBar->setMaximum(samp.width() - 768);
        }else{
            ui->horizontalScrollBar->setMaximum(0);
        }
        if (samp.height() > 768){
            ui->verticalScrollBar->setMaximum(samp.height() - 768);
        }else{
            ui->verticalScrollBar->setMaximum(0);
        }
        smaxx = ui->horizontalScrollBar->maximum();
        smaxy = ui->verticalScrollBar->maximum();
        fullShape = false;
        selecting = false;
        isSaved = true;
        cutPart();
        ui->spin_alp->setValue(0);
        ui->spin_blu->setValue(0);
        ui->spin_grn->setValue(0);
        ui->spin_red->setValue(0);
        isSaved = false;
        QWidget::setWindowTitle("Pixel Edit - " + projName);
        QWidget::repaint();
    }
    ui->centralwidget->setFocus();
}


void MainWindow::on_btn_sav_clicked()
{
    if (hasImage){
        isSaved = workImage.save(projPath + "/" + projName + "." + projExt);
        if (isSaved) QWidget::setWindowTitle("Pixel Edit - " + projName);
    }
    ui->centralwidget->setFocus();
}


void MainWindow::on_btn_saa_clicked()
{
    if (hasImage){
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setNameFilter(tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm, *.xbm *.xpm)"));
        dialog.setViewMode(QFileDialog::Detail);
        dialog.selectFile(projName);
        QString fileName = dialog.getSaveFileName(this, "Save As", projPath + "/" + projName, tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm, *.xbm *.xpm)"));
        isSaved = workImage.save(fileName);
        if (isSaved){
            ui->btn_sav->setEnabled(true);
            projName = fileName;
            projExt = projName.mid(projName.lastIndexOf(".") + 1, 4);

            projName = projName.left(projName.lastIndexOf(".")).toLower();
            int slanum = projName.lastIndexOf("/");
            projPath = projName.left(slanum);
            projName = projName.mid(slanum + 1, projName.length() - (slanum + 1));
            QWidget::setWindowTitle("Pixel Edit - " + projName);
        }
    }
    ui->centralwidget->setFocus();
}


void MainWindow::on_btn_lin_clicked()
{
    if (selection.size() > 0){
        for (int i=0;i<selection.size();i++){
            workImage.setPixelColor(selection[i].x, selection[i].y, fillColor);
        }
        selection.clear();
        ui->dispLbl->setText("");
        fullShape = false;
        selecting = false;
        isSaved = false;
        cutPart();
        QWidget::setWindowTitle("Pixel Edit - *" + projName);
        QWidget::repaint();
    }
    ui->centralwidget->setFocus();
}

