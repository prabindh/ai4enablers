/*
 * Part of ai4enablers project. 2023
 *
*/

#include "Vision.h"


using namespace cv;

std::string FILE_NAME = "C:/Users/psundareson/TOOLS/braille/test7-tricky.png";
QString PDF_NAME = "C:/Users/psundareson/TOOLS/braille/tables.pdf";
std::string PDF_PNG_NAME = "C:/Users/psundareson/TOOLS/braille/rocke-1.png";


static constexpr int MIN_LINE_WIDTH   = 50;
static constexpr int MIN_CONSIDERED_LINE_COUNT_METHOD1 = 10;
static constexpr int MIN_CONSIDERED_BOX_COUNT_METHOD2 = 5;
static constexpr int MIN_BOUNDS_WIDTH = 50;
static constexpr int MIN_BOUNDS_HEIGHT = 20;


Vision::Vision()
  : m_document(new QPdfDocument(this))
  , m_ImageLabel(new QLabel(this))
  , m_vBoxLayout(new QVBoxLayout(this))
  , m_next(new QPushButton("&Next", this))
  , m_prev(new QPushButton("&Prev",this))
{
    connect(m_next, &QPushButton::clicked, this, &Vision::handleNextButton);
    connect(m_prev, &QPushButton::clicked, this, &Vision::handlePrevButton);

    m_ImageLabel->setScaledContents( true );
    m_ImageLabel->minimumSizeHint();

    m_vBoxLayout->addWidget(m_ImageLabel);
    m_vBoxLayout->addWidget(m_next);
    m_vBoxLayout->addWidget(m_prev);

    m_currPage = m_pageCount = 0;
}

void Vision::handleNextButton(bool checked)
{
    qDebug() << "next" << m_currPage;

    bool found = false;
    while(!found)
    {
        m_currPage ++;
        if (m_currPage > m_pageCount)
        {
            QMessageBox msgError;
            msgError.setText("Max page count reached!");
            msgError.setIcon(QMessageBox::Information);
            msgError.setWindowTitle("Info");
            msgError.exec();

            break;
        }
        found = RunAlg(m_currPage);       
    }
}
void Vision::handlePrevButton(bool checked)
{
    qDebug() << "prev " << m_currPage;

    bool found = false;
    while(!found)
    {
        if (m_currPage > 0)
        {
            m_currPage --;
            if (m_currPage <= 0)
            {
                m_currPage = 0;
                QMessageBox msgError;
                msgError.setText("At first page!");
                msgError.setIcon(QMessageBox::Information);
                msgError.setWindowTitle("Info");
                msgError.exec();

                break;
            }
            found = RunAlg(m_currPage);
        }
        else
            break;
    }

}

void Vision::getBounds(int x1,int y1, int x2,int y2, cv::Rect & bound)
{
    if (x1 < bound.x)
    {
        bound.x = x1;
    }
    if (y1 < bound.y)
    {
        bound.y = y1;
    }
    if (x2 > (bound.x + bound.width))
    {
        bound.width = x2-bound.x;
    }
    if (y2 > (bound.y + bound.height))
    {
        bound.height = y2-bound.y;
    }
}
bool Vision::EdgeMethod2(std::string filename, std::vector<cv::Rect> & cells)
{
    cv::Mat image;

    image = imread(filename, IMREAD_GRAYSCALE);
    if (image.empty())
    {
        qDebug() << "Error reading image";
        return false;
    }
    bool ret = EdgeMethod2 (image, cells);
    return ret;
}

bool Vision::EdgeMethod1(std::string filename, std::vector<Rect> & cells)
{
    cv::Mat image;

    image = cv::imread(filename, IMREAD_GRAYSCALE);
    if (image.empty())
    {
        qDebug() << "Error reading image";
        return false;
    }

    bool ret = EdgeMethod1(image, cells);
    return ret;
}

bool Vision::EdgeMethod1(cv::Mat & image, std::vector<cv::Rect> & cells)
{
    cv::Mat edges, dst;

    // Canny

    cv::Canny(image, edges, 50,150);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(
                edges, //Input edge image
                lines,
                1, //Distance resolution in pixels
                (CV_PI/180), //Angle resolution in radians
                100, //# Min number of votes for valid line
                250, //# Min allowed length of line
                10 //# Max allowed gap between line for joining them
                );

    cv::Mat cdstP = image.clone();
    cdstP = Scalar::all(0);

    qDebug() << "# lines - Method1 " << lines.size();

    int validCount = 0;

    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::Rect cell(Point(100000, 100000), Size());
        Vec4i l = lines[i];
        if (l[2]-l[0] > MIN_LINE_WIDTH)
        {
            line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
            validCount ++;
            //qDebug() << l[0] << l[1] << l[2] << l[3];
            getBounds(l[0], l[1], l[2], l[3], cell);
            cells.emplace_back(cell);
        }
    }
    if (cells.empty())
    {
        qDebug() << "Method1 count [empty]";
    }
    else
    {
        qDebug() << "Method1 bounds " << cells[0].x << cells[0].y << cells[0].width << cells[0].height;
        qDebug() << "Method1 count " << validCount;
    }
    //imshow("lines", cdstP);

    if (validCount > MIN_CONSIDERED_LINE_COUNT_METHOD1)
    {
        return true;
    }
    return false;
}
bool Vision::EdgeMethod2(cv::Mat & image, std::vector<cv::Rect> & cells)
{
    cv::Mat edges, thresholded;
    threshold(image, thresholded, 128,255,THRESH_BINARY);

    Mat img_bin, img_bin1, img_bin2, img_bin_otsu;
    img_bin = 255-thresholded;
    img_bin1 = 255-image;
    img_bin2 = 255 - image;

    img_bin_otsu = img_bin2.clone();
    threshold(img_bin2,img_bin_otsu,128,255,THRESH_OTSU | THRESH_BINARY);
    Mat kernel, vertical_kernel;
    kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
    vertical_kernel = getStructuringElement(MORPH_RECT, Size(1, int((float)image.size().height/100.0)));

    Mat eroded_image;
    eroded_image = img_bin_otsu.clone();
    erode(img_bin_otsu, eroded_image, vertical_kernel, Point(-1,-1), 3);

    Mat vertical_lines;
    dilate(eroded_image, vertical_lines, vertical_kernel, Point(-1,-1), 3);

    Mat hor_kernel;
    hor_kernel = getStructuringElement(MORPH_RECT, Size(int((float)image.size().width/100.0), 1));
    Mat horizontal_lines;
    erode(img_bin, horizontal_lines, hor_kernel, Point(-1,-1), 15);
    dilate(horizontal_lines, horizontal_lines, hor_kernel, Point(-1,-1), 15);


    Mat vertical_horizontal_lines;
    addWeighted(vertical_lines, 0.5, horizontal_lines, 0.5, 0.0,vertical_horizontal_lines);

    erode(~vertical_horizontal_lines, vertical_horizontal_lines, kernel,Point(-1,-1), 15);
    threshold(vertical_horizontal_lines, vertical_horizontal_lines, 128,255, THRESH_BINARY | THRESH_OTSU);

    Mat bitxor;
    bitwise_xor(image, vertical_horizontal_lines, bitxor);

    Mat bitnot;
    bitwise_not(bitxor, bitnot);

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(vertical_horizontal_lines, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    std::vector<Rect> boxes;
    for(size_t k = 0;k < contours.size(); k ++)
    {
        cv::Rect cell(Point(100000, 100000), Size());
        Rect r = boundingRect(contours[k]);
        if (r.width < 1000 && r.height < 500)
        {
            boxes.emplace_back(r);
            getBounds(r.x, r.y, r.x+r.width, r.y+r.height, cell);
            cells.emplace_back(cell);
        }
    }
    if (cells.empty())
    {
        qDebug() << "Method2 count [empty]";
    }
    else
    {
        qDebug() << "Boxes Method2 count " << boxes.size();
        qDebug() << "Method2 bounds " << cells[0].x << cells[0].y << cells[0].width << cells[0].height;
    }
    if (boxes.size() > MIN_CONSIDERED_BOX_COUNT_METHOD2)
    {
        return true;
    }
    return false;
}

bool Vision::LoadDoc(QString & path)
{
    // open a PDF and render
    if (!m_document) return false;

    m_document->load(path);
    m_pageCount = m_document->pageCount();

    if (m_pageCount) return true;

    return false;
}

bool Vision::RunAlg(int page)
{
    m_image = m_document->render(page, QSize(720, 1080));

    if (m_image.isNull())
    {
        return false;
    }

    QString finalname = QString::fromStdString(PDF_PNG_NAME) + QString::number(page);
    //img.save(finalname);

    qDebug() << "format=" << m_image.format();
    QImage gray = m_image.convertToFormat(QImage::Format::Format_Grayscale8);
    qDebug() << "format=" << gray.format();
    cv::Mat currImage(gray.height(), gray.width(), CV_8UC1, (cv::Scalar*)gray.scanLine(0));

    std::vector<cv::Rect> bounds1, bounds2;

#if 0 //Do it only on image buffer, not on file
    EdgeMethod1(PDF_PNG_NAME, bounds1);
    EdgeMethod2(PDF_PNG_NAME, bounds2);

    bounds1.clear();
    bounds2.clear();
#endif
    bool ret = EdgeMethod1(currImage, bounds1);
    ret = EdgeMethod2(currImage, bounds2);

    if (ret)
    {
        QPainter painter(&m_image);
        for (cv::Rect r : bounds2)
        {
            // Draw bounds
            QPen pen;
            pen.setWidth(5);
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.drawRect(QRect(r.x, r.y, r.width, r.height));
        }
        painter.end();
        m_ImageLabel->setPixmap(
                    QPixmap::fromImage(m_image).scaled(
                        QSize(480,640), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        this->setWindowTitle("Tabler (" + QString::number(page) + ") (" + QString::number(m_pageCount) + ")");
        return ret;
    }

    return false;
}
