/*
 * Part of ai4enablers project. 2023
 *
*/
#ifndef CV_H
#define CV_H

#include <QDebug>
#include <QWidget>
#include <QPdfDocument>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QVBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QSizePolicy>
#include <QMessageBox>
#include <QResizeEvent>


#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

class Vision : public QWidget
{
Q_OBJECT
public:
//explicit MyClass(QObject *parent = 0) : QObject(parent) {}
Vision();
bool RunAlg(int page);
bool LoadDoc(QString & path);

signals:

public slots:

private:
    QPdfDocument *m_document;
    bool EdgeMethod1(std::string filename, std::vector<cv::Rect> & cells);
    bool EdgeMethod2(std::string filename, std::vector<cv::Rect> & cells);

    bool EdgeMethod1(cv::Mat & image, std::vector<cv::Rect> & cells);
    bool EdgeMethod2(cv::Mat & image, std::vector<cv::Rect> & cells);

    void getBounds(int x1,int y1, int x2,int y2, cv::Rect & bound);

    QImage m_image;
    QLabel* m_ImageLabel;
    QVBoxLayout* m_vBoxLayout;
    QPushButton* m_next;
    QPushButton* m_prev;
    int m_currPage;
    int m_pageCount;

private slots:
  void handleNextButton(bool checked);
  void handlePrevButton(bool checked);
};

#endif // CV_H
