#ifndef DELETERESULTWINDOW_H
#define DELETERESULTWINDOW_H

#include <QWidget>
#include <QLineEdit>

class DeleteResultWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DeleteResultWindow(QWidget *parent = 0);
    
signals:
    void deleteIdEntered(int);

public slots:

private slots:
    void onConfirmClick();
    void onCancelClick();

private:
    QLineEdit *id_edit;
    
};

#endif // DELETERESULTWINDOW_H
