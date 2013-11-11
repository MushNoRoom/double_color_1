#include "newresultwindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVector>
#include <QPushButton>
#include <QMessageBox>

NewResultWindow::NewResultWindow(QWidget *parent) :
    QWidget(parent),
    m_blue_ball(1)
{
    QGridLayout *layout = new QGridLayout();

    QLabel* serial_label = new QLabel(QString::fromUtf8("输入开奖期号"), this);
    QLabel* date_label = new QLabel(QString::fromUtf8("输入开奖日期"), this);
    QLabel* red_ball_label = new QLabel(QString::fromUtf8("输入红球号码"), this);
    QLabel* blue_ball_lable = new QLabel(QString::fromUtf8("输入篮球号码"), this);

    m_serial_edit = new QLineEdit(this);
    m_serial_edit->setPlaceholderText(QString::fromUtf8("开奖期号"));
    m_year_edit = new QLineEdit(this);
    m_year_edit->setPlaceholderText(QString::fromUtf8("开奖年份"));
    m_month_edit = new QLineEdit(this);
    m_month_edit->setPlaceholderText(QString::fromUtf8("开奖月份"));
    m_day_edit = new QLineEdit(this);
    m_day_edit->setPlaceholderText(QString::fromUtf8("开奖日"));

    QString default_num_str("00");

    m_red_edit1 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit1);
    m_red_edit2 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit2);
    m_red_edit3 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit3);
    m_red_edit4 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit4);
    m_red_edit5 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit5);
    m_red_edit6 = new QLineEdit(this); m_red_edit_vec.append(m_red_edit6);


    layout->addWidget(serial_label, 0, 0);
    layout->addWidget(m_serial_edit, 0, 1);
    layout->addWidget(date_label, 1, 0);
    layout->addWidget(m_year_edit, 1, 1);
    layout->addWidget(m_month_edit, 1 ,2);
    layout->addWidget(m_day_edit, 1 ,3);
    layout->addWidget(red_ball_label, 2, 0);
    for (int i = 0; i < m_red_edit_vec.size(); ++i)
    {
        layout->addWidget(m_red_edit_vec.at(i), 2, i+1);
        m_red_edit_vec.at(i)->setPlaceholderText(default_num_str);
    }
    m_blue_edit1 = new QLineEdit(this); m_blue_edit1->setPlaceholderText(default_num_str);
    layout->addWidget(blue_ball_lable, 3, 0);
    layout->addWidget(m_blue_edit1, 3, 1);
    QPushButton* confirm = new QPushButton(QString::fromUtf8("确定"), this);
    connect(confirm, SIGNAL(clicked(bool)), SLOT(onConfirmClick()));

    QPushButton* cancel = new QPushButton(QString::fromUtf8("取消"), this);
    connect(cancel, SIGNAL(clicked(bool)), SLOT(onCancelClick()));
    layout->addWidget(confirm, 3, 5);
    layout->addWidget(cancel, 3, 6);
    this->setLayout(layout);
    setAttribute(Qt::WA_DeleteOnClose);
}

void NewResultWindow::onConfirmClick()
{
    if (!m_red_balls.isEmpty())
    {
        // remove all the previous stored red balls
        m_red_balls.clear();
    }
    for(int i = 0; i < m_red_edit_vec.size(); ++i)
    {
        m_red_balls.append(quint32(m_red_edit_vec.at(i)->text().toUInt()));
    }

    int serial = m_serial_edit->text().toInt();
    if (serial == 0)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("期号输入错误"));
        msgBox.exec();
        return;
    }

    int year_ok = m_year_edit->text().toInt();
    int month_ok = m_month_edit->text().toInt();
    int day_ok = m_day_edit->text().toInt();
    // check if the date entered are valid numbers
    if (year_ok == 0 || month_ok == 0 || day_ok == 0)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("日期输入错误"));
        msgBox.exec();
        return;
    }

    bool date_ok = m_result_date.setDate(m_year_edit->text().toInt(),
                          m_month_edit->text().toInt(),
                          m_day_edit->text().toInt());
    // if valid number ranges are entered
    if (!date_ok)
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("日期输入错误"));
        msgBox.exec();
        return;
    }

    m_blue_ball[0] = quint32(m_blue_edit1->text().toUInt());

    emit newResultConfirmed(serial, m_result_date, m_red_balls, m_blue_ball);
}

void NewResultWindow::onCancelClick()
{
    // close the new result window
    this->close();
}
