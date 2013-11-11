#include "lotterydockwidget.h"
#include <QHBoxLayout>

const int num_of_blue_balls = 16;

LotteryDockWidget::LotteryDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_blue_ball_number_view_select(new QComboBox(this)),
    m_red_blue_separate_select(new QComboBox(this))
{
    m_blue_ball_number_view_select->addItem(QString::fromUtf8("显示所有蓝球"));
    for (int i = 1; i <= num_of_blue_balls; ++i)
    {
        QString item_string = QString::fromUtf8("显示") + QString::number(i) + QString::fromUtf8("号篮球");
        m_blue_ball_number_view_select->addItem(item_string);
    }


    connect(m_blue_ball_number_view_select, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onBlueBallSelectChanged(const QString&)));


    m_red_blue_separate_select->addItem(QString::fromUtf8("红蓝球区分计算"));
    m_red_blue_separate_select->addItem(QString::fromUtf8("红蓝球合并计算"));
    connect(m_red_blue_separate_select, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onRedBlueSeparateSelectChanged(const QString&)));

    QWidget* dock_widget_content = new QWidget(this);
    QHBoxLayout *dock_widget_content_layout = new QHBoxLayout;
    dock_widget_content_layout->addWidget(m_red_blue_separate_select);
    dock_widget_content_layout->addWidget(m_blue_ball_number_view_select);
    dock_widget_content->setLayout(dock_widget_content_layout);
    this->setWidget(dock_widget_content);
}

bool LotteryDockWidget::isRedBlueSeparated()
{
    if (m_red_blue_separate_select->currentText() == QString::fromUtf8("红蓝球区分计算"))
    {
        return true;
    }
    else if (m_red_blue_separate_select->currentText() == QString::fromUtf8("红蓝球合并计算"))
    {
        return false;
    }
}

void LotteryDockWidget::onBlueBallSelectChanged(const QString& text)
{
    if (text == QString::fromUtf8("显示所有蓝球"))
    {
        // 0 stands for all blue balls
        emit blueBallSelectChanged(0);
    }
    else
    {
        for (int i = 1; i <= num_of_blue_balls; ++i)
        {
             QString item_string = QString::fromUtf8("显示") + QString::number(i) + QString::fromUtf8("号篮球");
             if (item_string == text)
             {
                 emit blueBallSelectChanged(i);
             }
        }
    }
}

void LotteryDockWidget::onRedBlueSeparateSelectChanged(const QString& text)
{
    // red and blue balls are treated separately
    if (text == QString::fromUtf8("红蓝球区分计算"))
    {
        emit redBlueSeparateChanged(true);
    }
    else if (text == QString::fromUtf8("红蓝球合并计算"))
    {
        emit redBlueSeparateChanged(false);
    }
}
