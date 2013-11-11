#include "deleteresultwindow.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

DeleteResultWindow::DeleteResultWindow(QWidget *parent) :
    QWidget(parent)
{
    QLabel *delete_result_label = new QLabel(QString::fromUtf8("输入要删除的开奖结果编号"), this);
    id_edit = new QLineEdit(this);
    id_edit->setPlaceholderText("1");
    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addWidget(delete_result_label);
    layout1->addWidget(id_edit);
    QWidget *edit_widget = new QWidget(this);
    edit_widget->setLayout(layout1);


    setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* confirm = new QPushButton(QString::fromUtf8("确定"), this);
    connect(confirm, SIGNAL(clicked(bool)), SLOT(onConfirmClick()));

    QPushButton* cancel = new QPushButton(QString::fromUtf8("取消"), this);
    connect(cancel, SIGNAL(clicked(bool)), SLOT(onCancelClick()));
    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(confirm);
    layout2->addWidget(cancel);
    QWidget *button_widget = new QWidget(this);
    button_widget->setLayout(layout2);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(edit_widget);
    layout->addWidget(button_widget);

    this->setLayout(layout);
}

void DeleteResultWindow::onConfirmClick()
{
    bool ok;
    int id = id_edit->text().toInt(&ok, 10);

    if (!ok)
        return;

    emit deleteIdEntered(id);
}

void DeleteResultWindow::onCancelClick()
{
    this->close();
}
