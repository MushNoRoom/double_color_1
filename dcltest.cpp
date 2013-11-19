#include "dcltest.h"
#include "mymodel.h"
#include "dataimporter.h"
#include <QSqlQuery>
#include <QFile>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QString>

class LotteryTableModel;



DclTest* DclTest::test_instance = 0;

DclTest* DclTest::getTestInstance()
{
    if (test_instance == 0)
    {
        test_instance = new DclTest;
    }
    return test_instance;
}

DclTest::DclTest(){
    m_testModel = 0;

}

DclTest::~DclTest(){
    // test the LotteryTableModel object
    if (m_testModel != 0)
    {
        qDebug() << "Deleting Database model";
        m_testModel->database().close(); // close the database first
        delete m_testModel;
    }
}

void DclTest::startTest()
{
    qDebug() << "test starts";
    testDeleteTables();
    testCreateTables();
    testImportFile();
    qDebug() << "test ends";
}


void DclTest::testCreateTables()
{

    LotteryTableModel* model = getModel();
    model->database().open();
    QString createStr("CREATE TABLE result3("
            "Serial int not null,"
            "Date DATE,"
            "Red1 tinyint, Red2 tinyint, Red3 tinyint, Red4 tinyint, Red5 tinyint, Red6 tinyint,"
            "Blue1 tinyint,"
            "PRIMARY KEY(Serial))");
    qDebug() << createStr;
    QSqlQuery dropTableQuery;
    dropTableQuery.prepare("DROP TABLE result3");
    dropTableQuery.exec();

    QSqlQuery createResultTableQuery(model->database());
    createResultTableQuery.prepare(createStr);
    Q_ASSERT(createResultTableQuery.exec());

    QSqlQuery createGapTables(model->database());
    QString cstr1("("
                  "Serial int,"
                  "Red1Gap int, Red2Gap int, Red3Gap int, Red4Gap int, Red5Gap int, Red6Gap int,"
                  "Blue1Gap int,"
                  "Prediction TEXT,"
                  "Calculation TEXT)");
    qDebug() << cstr1;


    for (int blueBall = 0; blueBall <= 16; ++blueBall)
    {
        QString tb1, tb2;
        if (blueBall == 0)
        {
            tb1 = "gap_all_sp";
            tb2 = "gap_all_nsp";
        }
        else
        {
            tb1 = "gap_blue" + QString::number(blueBall) + "_sp";
            tb2 = "gap_blue" + QString::number(blueBall)+ "_nsp";
        }
        dropTableQuery.prepare("DROP TABLE " + tb1);
        dropTableQuery.exec();
        createGapTables.prepare("CREATE TABLE " + tb1 + cstr1);
        Q_ASSERT(createGapTables.exec());
        dropTableQuery.prepare("DROP TABLE " + tb2);
        dropTableQuery.exec();
        createGapTables.prepare("CREATE TABLE " + tb2 + cstr1);
        Q_ASSERT(createGapTables.exec());

    }

}

LotteryTableModel* DclTest::getModel()
{
    if (m_testModel == 0)
    {
        char buffer[64];

        QHash<QString, QString> config_hash;
        QFile config_file(QCoreApplication::applicationDirPath() + "/config.txt");
        qDebug() << (config_file.open(QIODevice::ReadOnly));


        memset(buffer, '\0', 64);
        int readLength = config_file.readLine(buffer, 64);
        while (readLength != -1)
        {
            QString line(buffer);
            QStringList list1 = line.split(" = ");

            // remove carriage return
            list1[1].remove(0x0D);
            // remove line feed
            list1[1].remove(0x0A);

            config_hash.insert(list1[0], list1[1]);
            memset(buffer, '\0', 64);
            readLength = config_file.readLine(buffer, 64);
        }
        config_file.close();
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(config_hash.value("host_name"));
        db.setUserName(config_hash.value("user"));
        db.setPort(3306);
        db.setDatabaseName(config_hash.value("database"));
        db.setPassword(config_hash.value("password"));
        db.open();
        m_testModel = new LotteryTableModel(0, db);
    }
    return m_testModel;
}



void DclTest::testDeleteTables()
{
    LotteryTableModel* model = getModel();
    model->database().open();
    QSqlQuery query2("delete from gap_all_sp", model->database());
    QSqlQuery query3("delete from gap_all_nsp", model->database());


    for (int blueBall = 1; blueBall <= 16; ++blueBall)
    {
        QString sp_table = "gap_blue" + QString::number(blueBall)+ "_sp";
        QString nsp_table = "gap_blue" + QString::number(blueBall)+ "_nsp";
        QSqlQuery gap_table_query1("DELETE FROM " + sp_table, model->database());
        QSqlQuery gap_table_query2("DELETE FROM " + nsp_table, model->database());
    }
}


void DclTest::testImportFile()
{
    DataImporter di(QCoreApplication::applicationDirPath() + "/dcl_file.txt");
    di.startImporting(getModel());

}
