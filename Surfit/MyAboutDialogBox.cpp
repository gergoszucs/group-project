#include "MyAboutDialogBox.h"
#include <QGridLayout>
#include <QLabel.h>
#include <QPixmap.h>

// Dom's includes.
#include "global.h"
#include "ITProject.h"

MyAboutDialogBox::MyAboutDialogBox( QWidget * parent) : QWidget(parent)
{

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside MyAboutDialogBox constructor");

	QGridLayout *layout = new QGridLayout;

	m_label1 = new QLabel(QString("Flexit version: %1").arg(PROGRAM_VERSION));
	m_label1->setStyleSheet("QLabel { background-color : black; color : white; }");
	layout->addWidget(m_label1,0,0);

	m_label2 = new QLabel();
	m_label2->setTextFormat(Qt::RichText);
	m_label2->setText("by Dr Dominique Fleischmann &copy; 2016");
	m_label2->setStyleSheet("QLabel { background-color : black; color : white; }");
	layout->addWidget(m_label2,1,0);

	m_label3 = new QLabel();
	m_label3->setTextFormat(Qt::RichText);
	m_label3->setText("d.s.fleischmann@cranfield.ac.uk");
	m_label3->setStyleSheet("QLabel { background-color : black; color : white; }");
	layout->addWidget(m_label3,2,0);

	QPixmap image("E:/WorkingFolder/SWDevelopment/PCMAC/C++/Projects/Surfit/Surfit/Resources/about.png", "png");
	
	QString imagePath("");

	m_image = new QPixmap("E:/WorkingFolder/SWDevelopment/PCMAC/C++/Projects/Surfit/Surfit/Resources/about.png", "png");
	m_imageLabel = new QLabel();
	m_imageLabel->setPixmap(*m_image);
	layout->addWidget(m_imageLabel,3,0);


	// Create Close button.
	m_button = new QPushButton("Close", this);
	QPalette* p = new QPalette();
	p->setColor(QPalette::Background, Qt::white);
	p->setColor(QPalette::ButtonText,Qt::blue);
	m_button->setPalette(*p);
    m_button->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));

    // Connect button signal to appropriate slot
    connect(m_button, SIGNAL (released()), this, SLOT (handleButton()));
	layout->addWidget(m_button, 4, 0);

	setLayout(layout);

	// Change the background colour.
	QPalette Pal(palette());
	// set white background
	Pal.setColor(QPalette::Background, Qt::white);
	setAutoFillBackground(true);
	setPalette(Pal);

	setWindowTitle("Surfit");

	this->setStyleSheet("background-color: black;");

    setFixedSize(203, 320);
}


MyAboutDialogBox::~MyAboutDialogBox(void)
{
}


void MyAboutDialogBox::handleButton()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "button");

	close();
}