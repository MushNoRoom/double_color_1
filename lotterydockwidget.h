#ifndef LOTTERYDOCKWIDGET_H
#define LOTTERYDOCKWIDGET_H

#include <QDockWidget>
#include <QComboBox>

class LotteryDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit LotteryDockWidget(QWidget *parent = 0);
    
signals:
    void blueBallSelectChanged(int ball_number);
    /*!
     * \brief redBlueSeparateChanged emitted when user selects to change the separation mode
     * \param isSeparated true for calculated seapartely false for calculated equally
     */
    void redBlueSeparateChanged(bool isSeparated);

public:
    /*!
     * \brief isRedBlueSeparted return if the user's selection for the current separation mode
     * \return true as the red and blue are treated separately otherwise false
     */
    bool isRedBlueSeparated();
public slots:

private slots:
    void onBlueBallSelectChanged(const QString& text);
    void onRedBlueSeparateSelectChanged(const QString& text);

private:
    QComboBox* m_blue_ball_number_view_select;
    QComboBox* m_red_blue_separate_select;
    
};

#endif // LOTTERYDOCKWIDGET_H
