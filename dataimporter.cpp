#include "dataimporter.h"
#include "mymodel.h"
#include <QStringList>
#include <QDebug>
#include <QThread>
#include <QErrorMessage>
#include <QDate>
#include <QProgressBar>
#include <QSqlQuery>
#include <QSqlDatabase>


char data_buffer[64];

void addDataFromLocalFile(QString fileName, LotteryTableModel* model)
{
    QSqlQuery delete_query("DELETE from result3", model->database());

    QSqlQuery reset_pk("alter table result3 auto_increment=1");
    QSqlQuery query(model->database());
    fileName.prepend("'");
    fileName.append("'");
    QString import_string = "LOAD DATA INFILE " + fileName + " replace INTO table result3 fields terminated by ' '";
    qDebug() << import_string;
    query.prepare(import_string);
    qDebug() << "Start importing on thread " <<  QThread::currentThread();
    qDebug() << query.exec();

    QSqlQuery query2("delete from gap_all_sp", model->database());
    QSqlQuery query3("delete from gap_all_nsp", model->database());


    QSqlQuery query4("INSERT INTO gap_all_sp (Serial) SELECT Serial from result3", model->database());
    QSqlQuery query5("INSERT INTO gap_all_nsp (Serial) SELECT Serial from result3", model->database());

    for (int blueBall = 1; blueBall <= 16; ++blueBall)
    {
        QString sp_table = "gap_blue" + QString::number(blueBall)+ "_sp";
        QString nsp_table = "gap_blue" + QString::number(blueBall)+ "_nsp";
        QSqlQuery gap_table_query1("DELETE FROM " + sp_table, model->database());
        QSqlQuery gap_table_query2("DELETE FROM " + nsp_table, model->database());

        QSqlQuery insert_query_sp;
        insert_query_sp.prepare("INSERT INTO " + sp_table + " (Serial) SELECT Serial from result3 where Blue1 = :Blue1");
        insert_query_sp.bindValue(":Blue1", blueBall);
        insert_query_sp.exec();

        QSqlQuery insert_query_nsp;
        insert_query_nsp.prepare("INSERT INTO " + nsp_table + " (Serial) SELECT Serial from result3 where Blue1 = :Blue1");
        insert_query_nsp.bindValue(":Blue1", blueBall);
        insert_query_nsp.exec();

    }

    // calculate all the gaps after importing.
    model->calculateAllGaps();

    model->select();
}

DataImporter::DataImporter(const QString& fileName, QObject *parent)
    :QObject(parent),
      import_progress(0),
      m_file_handler(fileName)
{
    if (!m_file_handler.exists())
        m_fileOk = false;
    else
    {
        bool ok = m_file_handler.open(QIODevice::ReadOnly);
        if (!ok)
        {
            m_fileOk = false;
            QErrorMessage err_msg;
            err_msg.showMessage("File cannot be open");
            err_msg.exec();
        }
        else
            m_fileOk = true;
    }
}

DataImporter::~DataImporter()
{
    m_file_handler.close();
}


void DataImporter::startImporting(LotteryTableModel *model)
{
    addDataFromLocalFile(m_file_handler.fileName(), model);
    emit doneImporting();

}


