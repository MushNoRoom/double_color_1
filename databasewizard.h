#ifndef DATABASEWIZARD_H
#define DATABASEWIZARD_H

#include <QObject>


class DataBaseWizard : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseWizard(QObject *parent = 0);
    /*!
     * \brief dbLocation location of the database, usually a string indicating a machine within a LAN
     * \return the IP address of the server
     */
    QString dbLocation();


signals:

public slots:
    /*!
     * \brief createDB create a database whose name is \e name
     * \param name the name of database to create, usually a name indicating the type of the lottery. e.g. 双色球,七乐彩等. however, use only letters
     * \return true if created successfully otherwise false
     */
    bool createDB(QString &name);
    /*!
     * \brief setLoggingInfo set the logging information
     * \param userName logging username, should be root for the purpose now
     * \param password password, fixed for now. maybe adding changable password?
     */
    void setLoggingInfo(QString &userName, QString &password);
    /*!
     * \brief setSpecialNum set the special number for this lottery to be num. 0 or negative number for no special number
     * \param num number to be set to special number
     */
    void setSpecialNum(int num);
};

#endif // DATABASEWIZARD_H
