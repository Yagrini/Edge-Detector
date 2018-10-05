#ifndef MONCONTOUR_H
#define MONCONTOUR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class monContour;
}

class monContour : public QMainWindow
{
    Q_OBJECT

public:
    explicit monContour(QWidget *parent = 0);
    ~monContour();

    cv::Mat matImageSource;
    cv::Mat monImageGris,monImageContour,monImageSeuil;
    bool valid=false;
    int i=0;

private slots:
    void clickOuvrir();
    void clickEnregistrer();

    void clickGaussien();
    void clickMedian();

    void clickCanny();
    void clickLaplacien();
    void clickPrewitt();
    void clickRoberts();
    void clickSobel();

    void clickSliderSeuil(int);

private:
    Ui::monContour *ui;
};

#endif // MONCONTOUR_H
