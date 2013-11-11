#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QThread>
#include <QSqlDatabase>
#include <QScopedPointer>
#include <QMutex>

class MyModel;

class DataBaseWorker : public QThread
{
    Q_OBJECT
public:
    explicit DataBaseWorker(QObject *parent = 0);
    const QSqlDatabase& database() const
    {
        return const_cast<const QSqlDatabase&>(database());
    }
    QSqlDatabase& database()
    {
        return m_db;
    }
    MyModel* modelOfWorker()
    {
        return m_sqlModel;
    }
    QMutex& mutexLock()
    {
        return m_mutex;
    }
    void initializeWorker();

signals:
    void databaseReady(bool);
    
public slots:

protected:
    void run();

private:
    QMutex m_mutex;
    MyModel* m_sqlModel;
    QSqlDatabase m_db;
    
};

#endif // DATABASEWORKER_H
