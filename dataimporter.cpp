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

void addDataFromLocalFile(QString fileName, MyModel* model)
{
    QSqlQuery delete_query("DELETE from result3", model->database());

    QSqlQuery reset_pk("alter table result3 auto_increment=1");
    QSqlQuery query(model->database());
    fileName.prepend("'");
    fileName.append("'");
    query.prepare("LOAD DATA INFILE " + fileName + " replace INTO table result3 fields terminated by ' '" );
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

    model->calculateGaps();

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


void DataImporter::startImporting(MyModel *model)
{
//    qDebug() << "Start importing" << QThread::currentThread();
//    // go to the beginning of the file
//    m_file_handler.seek(0);
//    // read one line of record
//    qint64  lineLength = m_file_handler.readLine(data_buffer,
//                                        64);
//    qint64 num_of_lines = m_file_handler.size() / lineLength;

//    qDebug() << num_of_lines;
//    if (import_progress != 0)
//        delete import_progress;


//    connect(this, SIGNAL(doneImporting()), import_progress, SLOT(close()));
//    import_progress->show();
//    qDebug() << import_progress->maximum();
//    while (lineLength != -1)
//    {
//        QString line_data(data_buffer);
//        // split the sections
//        QStringList list = line_data.split(" ");
//        qDebug() << list.size();
////        qDebug() << list;
//        QStringList date_list = list[1].split("-");

////        QStringList list1 = list[0].split("	");
////        qDebug() << list1;
////        for (int i = 0; i < list.size(); i++)
////        {
////            qDebug() << list.at(i);
////        }
//        qDebug() << date_list;
//        QDate date(date_list.at(0).toInt(), date_list.at(1).toInt(),
//                   date_list.at(2).toInt());
//        QVector<quint32> red_balls;
//        for (int i = 2; i < 8; ++i)
//        {
//            red_balls.append(static_cast<quint32>(list.at(i).toInt()));
//        }
//        QVector<quint32> blue_ball(1, static_cast<quint32>(list.at(8).toInt()));

////        qDebug() << QString::fromUtf8("正在输入") << list.at(0) << QString::fromUtf8("期");
//        model->acceptNewResultData(date, red_balls, blue_ball);
//        addProgress();
//        lineLength = m_file_handler.readLine(data_buffer, 64);
//    }

    addDataFromLocalFile(m_file_handler.fileName(), model);
    emit doneImporting();

}


