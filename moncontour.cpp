#include "moncontour.h"
#include "ui_moncontour.h"

using namespace cv;

//les Masques

int SobelX[3][3] ={{-1,0,1},{-2,0,2},{-1,0,1}} ;
int SobelY[3][3] ={{1,2,1},{0,0,0},{-1,-2,-1}} ;
int PrewitX[3][3] ={{-1,0,1},{-1,0,1},{-1,0,1}} ;
int PrewitY[3][3] ={{1,1,1},{0,0,0},{-1,-1,-1}} ;
int RobertX[3][3] ={{1,0,0},{0,-1,0},{0,0,0}} ;
int RobertY[3][3] ={{0,1,0},{-1,0,0},{0,0,0}} ;
int laplace[3][3] ={{0,1,0},{1,-4,1},{0,1,0}} ;

//Calcul du noyau gaussien
double** calculNoyau(int s=1)
{
    if(s<=0) s=1;
    double** noyeau = new double* [2*s+1];
    for(int i=0 ; i<2*s+1 ; i++) noyeau[i] = new double [2*s+1];
    double sum(0.0);
    for(int x=-s ; x<=s ; x++)
    {
        for(int y=-s ; y<=s ; y++)
        {
            double radius = sqrt(x*x+y*y);
            noyeau[x+s][y+s] = (1/sqrt(2*M_PI))*exp(-(radius*radius)/2);
            sum +=noyeau[x+s][y+s];
        }
    }
    for(int i=0 ; i<s*2+1 ; i++)
    {
        for(int j=0 ; j<s*2+1 ; j++)
        {
            noyeau[i][j] /= sum;
        }
    }
    return noyeau;
}

//Gaussien
Mat lissageGaussien(Mat &img ,int  s=1)
{
    int ligne = img.rows;
    int colonne = img.cols;
    Mat imageLissee(ligne,colonne,CV_8UC3);
    if(s<=0) s=1;
    double** Filtre = calculNoyau(s);
    for(int i=0 ; i<ligne ; i++)
    {
        for(int j=0 ; j<colonne ; j++)
        {
            if( (j-s<0) || (j+s>=colonne) || (i-s<0) || (i+s>=ligne) ) imageLissee.at<Vec3b>(i,j) = img.at<Vec3b>(i,j);
            else
            {
                imageLissee.at<Vec3b>(i,j)=0;
                for(int x=-s ; x<=s ; x++)
                {
                    for(int y=-s ; y<=s ; y++)
                        imageLissee.at<Vec3b>(i,j) += img.at<Vec3b>(i+x,j+y) * Filtre[x+s][y+s];
                }
            }
        }
    }
    return imageLissee;
}

//Médiane
Mat lissageMedian(Mat &img)
{
    char tab[9];
    Mat imageLissee=img.clone();
    for(int x=1; x < img.rows-1; x++)
        {
            for(int y=1; y < img.cols-1; y++)
            {
                tab[0]=img.at<uchar>(x-1,y-1);
                tab[1]=img.at<uchar>(x-1,y);
                tab[2]=img.at<uchar>(x-1,y+1);
                tab[3]=img.at<uchar>(x,y-1);
                tab[4]=img.at<uchar>(x,y);
                tab[5]=img.at<uchar>(x,y+1);
                tab[6]=img.at<uchar>(x+1,y-1);
                tab[7]=img.at<uchar>(x+1,y);
                tab[8]=img.at<uchar>(x+1,y+1);
                qSort(tab,tab+9);
                imageLissee.at<uchar>(x,y)=tab[4];
            }
        }
    return imageLissee;
}

//Canny
Mat contourCanny(Mat &img,Mat &Grad_x,Mat &Grad_y)
{
    int ligne   = img.rows;
    int colonne = img.cols;

    for(int i=1 ; i<ligne-1 ;i++)
    {
        for(int j =1; j<colonne-1 ; j++)
        {
            double theta;
            theta = atan2(Grad_y.at<uchar>(i,j), Grad_x.at<uchar>(i,j))*(180/3.14);

            if(((-22.5 < theta) && (theta <= 22.5)) || ((157.5 < theta) && (theta <= -157.5)))
            {
                if ((img.at<uchar>(i,j) < img.at<uchar>(i,j+1)) || (img.at<uchar>(i,j) < img.at<uchar>(i,j-1)))
                    img.at<uchar>(i, j) = 0;
            }

            if (((-112.5 < theta) && (theta <= -67.5)) || ((67.5 < theta) && (theta <= 112.5)))
            {
                if ((img.at<uchar>(i,j) < img.at<uchar>(i+1,j)) || (img.at<uchar>(i,j) < img.at<uchar>(i-1,j)))
                    img.at<uchar>(i, j) = 0;
            }

            if (((-67.5 < theta) && (theta <= -22.5)) || ((112.5 < theta) && (theta <= 157.5)))
            {
                if ((img.at<uchar>(i,j) < img.at<uchar>(i+1,j+1)) || (img.at<uchar>(i,j) < img.at<uchar>(i-1,j-1)))
                    img.at<uchar>(i, j) = 0;
            }

            if (((-157.5 < theta) && (theta <= -112.5)) || ((22.5 < theta) && (theta <= 67.5)))
            {
                if ((img.at<uchar>(i,j) < img.at<uchar>(i-1,j+1)) || (img.at<uchar>(i,j) < img.at<uchar>(i+1,j-1)))
                    img.at<uchar>(i, j) = 0;
            }
        }
    }
     return img;
}

//Calcul Masque * Zone

int Calcul(Mat& img, int x , int y , int Masque[3][3])
{
    return  img.at<uchar>(x-1,y-1)*Masque[0][0] +  img.at<uchar>(x-1,y)*Masque[0][1] +  img.at<uchar>(x-1,y+1)*Masque[0][2] + img.at<uchar>(x,y-1)*Masque[1][0] +  img.at<uchar>(x,y)*Masque[1][1] +  img.at<uchar>(x,y+1)*Masque[1][2] + img.at<uchar>(x+1,y-1)*Masque[2][0] +  img.at<uchar>(x+1,y)*Masque[2][1] +  img.at<uchar>(x+1,y+1)*Masque[2][2];
}

//Gradient
Mat contourGradient(Mat &img , int MasqueX[3][3] , int MasqueY[3][3])
{
    Mat contourImage = img.clone();
    for(int i=1; i < img.rows-1; i++)
    {
        for(int j=1; j < img.cols-1; j++)
        {
            int GradientX = Calcul(img, i , j ,MasqueX);
            int GradientY = Calcul(img, i , j ,MasqueY);
            
            contourImage.at<uchar>(i,j) = sqrt(abs(GradientX)*abs(GradientX) + abs(GradientY)*abs(GradientY));
        }
    }
    return contourImage;
}


//Laplacien
Mat contourLaplacien(Mat &img , int S=120)
{
    Mat contourImage = img.clone();
    
    for(int i = 1; i < img.rows - 1; i++)
    {
        for(int j = 1; j< img.cols - 1; j++)
        {
            contourImage.at<uchar>(i,j) = Calcul(img, i, j, laplace);
        }
    }
    for(int i = 1; i < img.rows - 1; i++)
    {
        for(int j = 1; j< img.cols - 1; j++)
        {
            if(contourImage.at<uchar>(i,j)==0 &&
               (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i-1,j-1)<S) &&
                    (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i-1,j)<S)   &&
                        (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i-1,j+1)<S) &&
                            (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i,j+1)<S) &&
                                (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i,j-1)<S) &&
                                    (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i+1,j-1)<S) &&
                                        (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i+1,j)<S)   &&
                                            (contourImage.at<uchar>(i,j)-contourImage.at<uchar>(i+1,j+1)<S))
                contourImage.at<uchar>(i,j)=0;
        }
    }

    return contourImage;
}


//Seuillage
Mat SliderSeuil(Mat img , int seuil)
{
    int ligne   = img.rows;
    int colonne = img.cols;
    Mat imageSeuil(ligne,colonne,CV_8UC1);

    for(int i=0 ; i<ligne ; i++)
    {
        for(int j=0 ; j<colonne ; j++)
            imageSeuil.at<uchar>(i,j)=img.at<uchar>(i,j);
    }

    for(int i=0 ; i<ligne ; i++)
    {
        for(int j=0 ; j<colonne ; j++)
        {
            if(imageSeuil.at<uchar>(i,j)<seuil)
            {
                if( (imageSeuil.at<uchar>(i-1,j-1)<=seuil) && (imageSeuil.at<uchar>(i-1,j)<=seuil)
                   && (imageSeuil.at<uchar>(i-1,j+1)<=seuil) && (imageSeuil.at<uchar>(i,j-1)<=seuil)
                   && (imageSeuil.at<uchar>(i,j+1)<=seuil) && (imageSeuil.at<uchar>(i+1,j-1)<=seuil)
                   && (imageSeuil.at<uchar>(i+1,j)<=seuil) && (imageSeuil.at<uchar>(i+1,j+1)<=seuil))
                     imageSeuil.at<uchar>(i,j) = 0;
            }
        }
    }
    return imageSeuil;
}


//Connexion
monContour::monContour(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::monContour)
{
    ui->setupUi(this);

    QObject::connect(ui->Ouvrir,SIGNAL(clicked(bool)),this,SLOT(clickOuvrir()));                     //bouton Ouvrir
    QObject::connect(ui->Enregistrer,SIGNAL(clicked(bool)),this,SLOT(clickEnregistrer()));           //bouton Enregistrer

    QObject::connect(ui->Gaussien,SIGNAL(clicked(bool)),this,SLOT(clickGaussien()));                 //Gaussien
    QObject::connect(ui->Mediane,SIGNAL(clicked(bool)),this,SLOT(clickMedian()));                    //Médiane

    QObject::connect(ui->Canny,SIGNAL(clicked(bool)),this,SLOT(clickCanny()));                       //Canny
    QObject::connect(ui->Laplacien,SIGNAL(clicked(bool)),this,SLOT(clickLaplacien()));               //Laplacian
    QObject::connect(ui->Prewitt,SIGNAL(clicked(bool)),this,SLOT(clickPrewitt()));                   //Prewitt
    QObject::connect(ui->Roberts,SIGNAL(clicked(bool)),this,SLOT(clickRoberts()));                   //Roberts
    QObject::connect(ui->Sobel,SIGNAL(clicked(bool)),this,SLOT(clickSobel()));                       //Sobel

    QObject::connect(ui->slideSeuillage,SIGNAL(valueChanged(int)),this,SLOT(clickSliderSeuil(int))); //Seuillage

    setFixedSize(1090,570);
    setWindowTitle("Detection du contour");
}

monContour::~monContour()
{
    delete ui;
}

//Creation du slot qui permet d'insérer une image
void monContour::clickOuvrir()
{
    QString chemin = QFileDialog::getOpenFileName(
                this,"Choisissez une image","Image",tr("Image Files(*.png *.gif *.jpg *.jpeg)"));
    QImage imageSource;
    valid=imageSource.load(chemin);
    if(valid)
    {
        matImageSource = imread(chemin.toUtf8().constData(),1);
        cv::resize(matImageSource,matImageSource,cv::Size(390,390));
        cv::cvtColor(matImageSource,matImageSource,CV_BGR2RGB);
        ui->imageSource->setPixmap(
                    QPixmap::fromImage(QImage(matImageSource.data,390,390,matImageSource.step,QImage::Format_RGB888)));
    }
    else
    {
        QMessageBox::warning(this,"Attention","Image non chargée !");
    }
}

//Création du slot qui permet d'enregistrer une image
void monContour::clickEnregistrer()
{
    i++;
    ui->imageContour->grab().save("imageContour"+QString::number(i)+".png");
}

//Création du slot qui applique le lissage Gaussien
void monContour::clickGaussien()
{
    if(valid)
    {
        matImageSource=lissageGaussien(matImageSource);
        ui->imageSource->setPixmap(
                    QPixmap::fromImage(QImage(matImageSource.data,390,390,matImageSource.step,QImage::Format_RGB888)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le lissage Gaussien !");
    }
}

//Création du slot qui applique le lissage Médian
void monContour::clickMedian()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);
        matImageSource=lissageMedian(monImageGris);
        cvtColor(matImageSource,matImageSource,CV_GRAY2RGB);
        ui->imageSource->setPixmap(
                    QPixmap::fromImage(QImage(matImageSource.data,390,390,matImageSource.step,QImage::Format_RGB888)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le lissage Médiane !");
    }
}

//Création du slot qui applique le filtre Canny
void monContour::clickCanny()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);

        int scale = 1;
        int delta = 0;
        int ddepth = CV_16S;
        Mat grad;
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;

        Sobel(monImageGris,grad_x,ddepth,1,0,3,scale,delta,BORDER_DEFAULT);
        convertScaleAbs(grad_x,abs_grad_x);

        Sobel(monImageGris,grad_y,ddepth,0,1,3,scale,delta,BORDER_DEFAULT);
        convertScaleAbs(grad_y,abs_grad_y);

        addWeighted( abs_grad_x,0.5,abs_grad_y,0.5,0,grad);
        monImageContour=contourCanny(grad,abs_grad_x,abs_grad_y);
        ui->imageContour->setPixmap(
                    QPixmap::fromImage(QImage(monImageContour.data,390,390,monImageContour.step,QImage::Format_Grayscale8)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le filtre Canny !");
    }
}

//Création du slot qui applique le filtre Sobel
void monContour::clickSobel()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);
        monImageContour = contourGradient(monImageGris, SobelX, SobelY);
        ui->imageContour->setPixmap(
                    QPixmap::fromImage(QImage(monImageContour.data,390,390,monImageContour.step,QImage::Format_Grayscale8)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le filtre Sobel !");
    }
}

//Création du slot qui applique le filtre Prewitt
void monContour::clickPrewitt()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);
        monImageContour = contourGradient(monImageGris, PrewitX, PrewitY);
        ui->imageContour->setPixmap(
                    QPixmap::fromImage(QImage(monImageContour.data,390,390,monImageContour.step,QImage::Format_Grayscale8)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le filtre Prewitt !");
    }
}

//Création du slot qui applique le filtre Roberts
void monContour::clickRoberts()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);
        monImageContour = contourGradient(monImageGris, RobertX, RobertY);
        ui->imageContour->setPixmap(
                    QPixmap::fromImage(QImage(monImageContour.data,390,390,monImageContour.step,QImage::Format_Grayscale8)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le filtre Roberts !");
    }
}

//Création du slot qui applique le filtre Laplacien
void monContour::clickLaplacien()
{
    if(valid)
    {
        cvtColor(matImageSource,monImageGris,CV_RGB2GRAY);
        monImageContour=contourLaplacien(monImageGris);
        ui->imageContour->setPixmap(
                    QPixmap::fromImage(QImage(monImageContour.data,390,390,monImageContour.step,QImage::Format_Grayscale8)));
    }
    else
    {
        QMessageBox::critical(this,"Erreur critique","Veuillez charger une image avant d'appliquer le filtre Laplacien !");
    }
}

//Création du slot qui permet de modifier le seuil
void monContour::clickSliderSeuil(int s)
{
    monImageSeuil=SliderSeuil(monImageContour,s);
    ui->imageContour->setPixmap(
                QPixmap::fromImage(QImage(monImageSeuil.data,390,390,monImageSeuil.step,QImage::Format_Grayscale8)));
}
