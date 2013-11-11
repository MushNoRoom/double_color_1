#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
class QWaitCondition;
class DataBaseWorker;
class MyModel;

class ModelFactory : public QObject
{
    Q_OBJECT
public:
    explicit ModelFactory(QObject *parent = 0);
    const QSqlDatabase& database() const
    {
        return const_cast<const QSqlDatabase&>(database());
    }
    QSqlDatabase& database()
    {
        return m_db;
    }
    MyModel* model()
    {
        return m_sqlModel;
    }
    void setMutexLock(QMutex* mutex)
    {
        m_mutex = mutex;
    }

    void setWaiCondition(QWaitCondition* waitCondition)
    {
        m_waitCondition = waitCondition;
    }
    void setWorker(DataBaseWorker* dbWorker)
    {
        m_dbWorker = dbWorker;
    }

signals:
    void modelReady(bool);

public slots:
    MyModel* produceModel();

private:
    DataBaseWorker* m_dbWorker;
    QMutex* m_mutex;
    MyModel* m_sqlModel;
    QSqlDatabase m_db;
    QWaitCondition* m_waitCondition;
};

#endif // MODELFACTORY_H
