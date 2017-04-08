#pragma once

#include <QtWidgets/QWidget.h>
#include <qlabel.h>
#include <qpushbutton.h>

class MyAboutDialogBox : public QWidget
{
	Q_OBJECT

private:

	QLabel *m_label1;
	QLabel *m_label2;
	QLabel *m_label3;
	QLabel *m_imageLabel;
	QPixmap *m_image;
	QPushButton *m_button;

public:

	MyAboutDialogBox(QWidget * parent = 0);
	~MyAboutDialogBox(void) {};

	public slots:
		void handleButton(void);
};