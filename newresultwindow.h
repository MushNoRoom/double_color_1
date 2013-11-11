#ifndef NEWRESULTWINDOW_H
#define NEWRESULTWINDOW_H

#include <QWidget>
#include <QVector>
#include <QDate>

class QLineEdit;

/*!
 * \brief The newresultwindow class is the window for adding new result of lottery
 */
class NewResultWindow : public QWidget
{
    Q_OBJECT
public:
    explicit NewResultWindow(QWidget *parent = 0);
    
public:
    /*!
     * \brief redBalls returns the red balls currently shown in the editing widget
     * \return QVector that with red ball numbers as elements
     */
    const QVector<quint32>& redBalls()
    {
        return m_red_balls;
    }

    const QVector<quint32>& blueBall()
    {
        return m_blue_ball;
    }



signals:
    void newResultConfirmed(const int, const QDate&, const QVector<quint32>&, const QVector<quint32>&);
    
public slots:

private slots:
    void onConfirmClick();
    void onCancelClick();

private:
    // date of lottery date
    QDate m_result_date;
    QVector<QLineEdit*> m_red_edit_vec;
    QVector<quint32> m_red_balls, m_blue_ball;
    QLineEdit *m_serial_edit;
    QLineEdit *m_year_edit, *m_month_edit,*m_day_edit;
    QLineEdit *m_red_edit1, *m_red_edit2, *m_red_edit3, *m_red_edit4, *m_red_edit5, *m_red_edit6;
    QLineEdit *m_blue_edit1;
    
};

#endif // NEWRESULTWINDOW_H
