#include "modelfactory.h"
#include "mymodel.h"
#include "databaseworker.h"
#include <QDebug>
#include <QWaitCondition>
#include <QCoreApplication>


ModelFactory::ModelFactory(QObject *parent) :
    QObject(parent),
    m_sqlModel(0)
{
}

MyModel* ModelFactory::produceModel()
{
    // create the connection in a seperate thread
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString db_path = QCoreApplication::applicationDirPath()
            + "/database/lottery.db";

    qDebug() << "The database is created on " << QThread::currentThread();

    m_db.setDatabaseName(db_path);
    bool open_ok = m_db.open();

    if (m_sqlModel != 0)
        delete m_sqlModel;

    m_sqlModel = new MyModel(0, m_db);

    m_mutex->lock();
//    emit modelReady(open_ok);
    //m_dbWorker->setWorkerThreadWait(true);
    m_waitCondition->wakeAll();
    m_mutex->unlock();
    return m_sqlModel;
}
