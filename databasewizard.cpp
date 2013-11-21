#include "databasewizard.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <iostream>
#include <fstream>

DataBaseWizard::DataBaseWizard(QObject *parent) :
    QObject(parent)
{
}


QString DataBaseWizard::dbLocation()
{
    return QString();
}

bool DataBaseWizard::createDB(QString &name)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setPort(3306);
    QString create_db_str = "CREATE DATABASE " + name;
    QSqlQuery create_query(db);
    return create_query.exec(create_db_str);
}


void DataBaseWizard::setLoggingInfo(QString& userName, QString& password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setPort(3306);
    db.setUserName(userName);
    db.setPassword(password);
    // create string to write to file
    std::string userName_str = "username = " + userName.toStdString() + "\n";
    std::string password_str = "password = " + password.toStdString() + "\n";
    std::ofstream output_file;
    if (output_file.is_open())
        output_file.close();
    std::string ofile_name = "conf.txt";
    output_file.open(ofile_name);
    if (!output_file.is_open()) // create new file if not existing
        output_file.open(ofile_name, std::ios_base::app);
    output_file.write(userName_str.c_str(), userName_str.size());
    output_file.write(password_str.c_str(), password_str.size());
}

void DataBaseWizard::setSpecialNum(int num)
{
    return;
}
