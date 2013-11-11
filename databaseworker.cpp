#include "databaseworker.h"
#include "mymodel.h"
#include <QCoreApplication>

DataBaseWorker::DataBaseWorker(QObject *parent) :
    QThread(parent)
{
}

void DataBaseWorker::initializeWorker()
{
    start();
}

void DataBaseWorker::run()
{
    // create the connection in a seperate thread
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString db_path = QCoreApplication::applicationDirPath()
            + "/database/lottery.db";


    m_db.setDatabaseName(db_path);
    bool open_ok = m_db.open();

    emit databaseReady(open_ok);

    exec();

}
