#ifndef DATAIMPORTERPROXY_H
#define DATAIMPORTERPROXY_H

#include <QObject>
#include <QThread>

class DataImporter;
class LotteryTableModel;
/*!
 * \brief The DataImporterProxy class is a convenient class for making DataImporter work in multi-threaded environment
 */
class DataImporterProxy : public QThread
{
    Q_OBJECT
public:
    /*!
     * \brief DataImporterProxy constructor
     * \param di the DataImporter object pointer to the DataImporter
     * \param model the model accepting the data importing
     */
    explicit DataImporterProxy(DataImporter* di, LotteryTableModel* model,
                               QObject *parent = 0);
    
signals:
    void startImport(LotteryTableModel* model);
    //! emitted when the proxy has done its job
    void proxyDoneWork();

public slots:
    /*!
     * \brief proxyStartWork the proxy start the importing work
     */
    void proxyStartWork();

protected:
    void run();

private slots:
    void deleteImporter();

private:
    DataImporter* m_di;
    LotteryTableModel* m_model;
    
};

#endif // DATAIMPORTERPROXY_H
