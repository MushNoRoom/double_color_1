/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "mymodel.h"
#include "QtSql/qsqldatabase.h"
#include <QBrush>
#include <QFile>
#include <QPen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QSqlRecord>
#include <QThread>
#include <QMessageBox>
#include "QCoreApplication.h"
#include "qdebug.h"
#include <QSqlDriver>


QString table_name = "result3"; // the table used from sql database
QString determineTableName(int blueBall, bool separated)
{
    QString gap_table_name = "gap";
    if (blueBall == 0)
    {
        if (separated)
            gap_table_name += "_all_sp";
        else
            gap_table_name += "_all_nsp";
    }
    else // gap table name for specific blue ball
    {
        QString post_fix = "_blue" + QString::number(blueBall);
        if (separated)
            post_fix += "_sp";
        else
            post_fix += "_nsp";

        gap_table_name += post_fix;
    }
    return gap_table_name;
}

void insertionSort(QList<int>& gap_list) {
      int i, j, tmp;
      for (i = 1; i < gap_list.size(); i++) {
            j = i;
            while (j > 0 && gap_list[j - 1] < gap_list[j]) {
                  tmp = gap_list.at(j);
                  gap_list[j] = gap_list[j - 1];
                  gap_list[j - 1] = tmp;
                  j--;
            }
      }
}


LotteryTableModel::LotteryTableModel(QObject *parent, QSqlDatabase db)
    :QSqlTableModel(parent, db),
      m_selected_blue_ball(0),
      m_red_blue_separated(true)
{   
    setEditStrategy(QSqlTableModel::OnFieldChange);
}


LotteryTableModel::~LotteryTableModel()
{
    qDebug() << "My model closed.";
}


//-----------------------------------------------------------------
QVariant LotteryTableModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    int row = index.row();
    if (role == Qt::BackgroundColorRole)
    {
        if (column > 1 && column <8)
        {
            // red background color for red balls
            QColor red(210, 0 ,0);
            QBrush backgroundBrush(red);
            return backgroundBrush;
        }
        if (column == 8)
        {
            // blue background color for blue balls
            QColor blue(0, 0, 210);
            QBrush backgroundBrush(blue);
            return backgroundBrush;
        }
        // color for blue gap
        if (column == 15)
        {
            QColor yellow(210, 210, 0);
            QBrush backgroudBrush(yellow);
            return backgroudBrush;
        }
        if (column > 8 && column < 16)
        {
            QColor my_color(127,255,212);
            QBrush backgroundBrush(my_color);
            return backgroundBrush;
        }
    }
    if (role == Qt::ForegroundRole)
    {
        if (column == 8)
        {
            QBrush yellow_pen(Qt::yellow);
            return yellow_pen;
        }
    }

    if (role == Qt::TextAlignmentRole)
    {
        if (column > 1)
            return Qt::AlignHCenter;
    }
    if (role == Qt::DisplayRole)
    {
        if (column == 16)
            return prediction(row);
        else
            return QSqlTableModel::data(index, role);
    }

    return QSqlTableModel::data(index, role);
}

QVariant LotteryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return QString::fromUtf8("期号");
        else if (section == 1)
            return QString::fromUtf8("日期");
        else if (section >= 2 && section <= 7)
        {
            QString red_ball_str = QString::fromUtf8("红球") + QString::number(section - 1);
            return red_ball_str;
        }
        else if (section == 8)
        {
            QString blue_ball_str = QString::fromUtf8("篮球");
            return blue_ball_str;
        }
        else if (section >= 9 && section <= 14)
        {
            QString red_gap_str = QString::fromUtf8("红球") + QString::number(section - 8) + QString::fromUtf8("遗漏值");
            return red_gap_str;
        }
        else if (section == 15)
        {
            return QString::fromUtf8("篮球遗漏值");
        }
        else if (section == 16)
        {
            return QString::fromUtf8("遗漏值排序");
        }
        else if (section == 17)
        {
            return QString::fromUtf8("遗漏值计算");
        }
        else
            return QSqlTableModel::headerData(section, orientation, role);
    }
    else
        return QSqlTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags LotteryTableModel::flags(const QModelIndex &index) const
{
    int col = index.column();
    if (col >= 0 && col <= 16)
    {
        // read only
        return QSqlQueryModel::flags(index);
    }
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}

int LotteryTableModel::rowCount(const QModelIndex &parent)
{
    // fetch from more data from the buffer
    while (this->canFetchMore(parent))
        fetchMore();
    return QSqlTableModel::rowCount();
}

void LotteryTableModel::acceptNewResultData(const int serial, const QDate& resultDate, const QVector<quint32> &redBalls
                                  , const QVector<quint32> &blueBall)
{
    QSqlDatabase db = QSqlTableModel::database();
    QSqlQuery my_query(db);

    QString serial_check_str = "SELECT Serial from result3 where Serial = " + QString::number(serial);
    QSqlQuery serial_check_query(serial_check_str, db);

    //when result with the same serial is already in the database
    if (serial_check_query.first())
    {
        QMessageBox repeated_serial_box;
        repeated_serial_box.setText(QString::fromUtf8("期号输入重复"));
        repeated_serial_box.exec();
        return;
    }


    if (serial / 1000000 > 10 || serial / 1000000 < 1)
    {

        QMessageBox wrong_serial_box;
        wrong_serial_box.setText(QString::fromUtf8("期号输入错误"));
        wrong_serial_box.exec();
        return;
    }
    QString month_str = QString::number(resultDate.month());
//    if (resultDate.month() < 10)
//        month_str.prepend("0");

    QString day_str = QString::number(resultDate.day());
//    if (resultDate.day() < 10)
//        day_str.prepend("0");

    QString date = QString::number(resultDate.year()) + "/"
            + month_str + "/" + day_str;

    QString date_check_str = "Select Date from result3 where Date = '" + date + "'";

    QSqlQuery date_check_query(date_check_str, db);
    if (date_check_query.first())
    {
        QMessageBox date_serial_box;
        date_serial_box.setText(QString::fromUtf8("日期输入重复"));
        date_serial_box.exec();
        return;
    }

    // update the year result amount map
    const year_t cur_result_year = month_str.toInt();
    (m_year_amount_map.find(cur_result_year) == m_year_amount_map.end()) ? m_year_amount_map.insert(cur_result_year, 1) :
                                                                           m_year_amount_map[cur_result_year]++;
    int current_num_of_rows(0);
    int new_id = current_num_of_rows;
    if (this->database().driver()->hasFeature(QSqlDriver::QuerySize))
    {
        QSqlQuery query("SELECT * from result3", this->database());
        current_num_of_rows = query.size();
        int id_index = query.record().indexOf("Id");
        query.last();
        new_id = query.value(id_index).toInt() + 1;
    }
    else
    {
        current_num_of_rows = rowCount();
        new_id = this->record(current_num_of_rows - 1).value("Id").toInt() + 1;
    }


    if (current_num_of_rows == 0)
        new_id = 1;

    my_query.prepare(
                "INSERT INTO result3 (Serial, Date, Red1, Red2, Red3, Red4, Red5, Red6, Blue1) VALUES (:Serial, :Date, :Red1, :Red2, :Red3, :Red4, :Red5, :Red6, :Blue1)");
    // binds Id
    //my_query.bindValue(":Id", new_id);
    // binds Serial
    my_query.bindValue(":Serial", serial);


    my_query.bindValue(":Date", date);


    int numOfRedBalls = redBalls.size();
    // bind red ball numbers to field names
    for (int i = 0; i < numOfRedBalls; ++i)
    {
        QString field_name = ":Red" + QString::number((i+1), 10);
        my_query.bindValue(field_name, int(redBalls.at(i)));
    }

    // bind blue ball number to field name
    QString field_name = ":Blue1";
    my_query.bindValue(field_name, int(blueBall.at(0)));

    bool insert_OK = my_query.exec();

    if (!insert_OK)
    {
        QString error_msg = this->lastError().text();
        QMessageBox warning_box;
        warning_box.setText(QString::fromUtf8("加入新开奖结果出错. 重启程序再次尝试") + error_msg);
        warning_box.exec();
        return;
    }

    select();
    // create entry in the gap table with new result's serial number
    this->insertSerialIntoGapTable(serial, 0, false);
    this->insertSerialIntoGapTable(serial, 0, true);
    // for specific blue ball table
    this->insertSerialIntoGapTable(serial, blueBall.at(0), false);
    this->insertSerialIntoGapTable(serial, blueBall.at(0), true);

    // retrieve the last record of the SELECT which is the record we just insert
    QString last_record_query_str = "SELECT * from result3 where Serial = " + QString::number(serial);
    QSqlQuery last_record_query(last_record_query_str, database());   
    last_record_query.last();

    findGapsForRecord(last_record_query.record(), 0, false);
    findGapsForRecord(last_record_query.record(), 0, true);
    findGapsForRecord(last_record_query.record(), blueBall.at(0), true);
    findGapsForRecord(last_record_query.record(), blueBall.at(0), false);

    this->select();
    emit newResultInserted();

}


void LotteryTableModel::calculateGaps(int startingID)
{
    int num_of_rows;
    if (this->database().driver()->hasFeature(QSqlDriver::QuerySize))
    {
        QSqlQuery query(this->database());
        query.prepare("SELECT * from result3 WHERE Id >= " + QString::number(startingID));
        for (int blueBall = 0; blueBall <= 16; ++blueBall)
        {
            query.exec();
            query.first();
            while (query.next())
            {
                QSqlRecord rec1 = query.record();
                findGapsForRecord(rec1, blueBall, true);
                findGapsForRecord(rec1, blueBall, false);
            }
            emit oneGapTableFinished();
            emit oneGapTableFinished();
        }
    }
    else
        num_of_rows = this->rowCount();

}

void LotteryTableModel::deleteResult(int serial)
{
    QSqlQuery getBlueBallQuery(this->database());
    getBlueBallQuery.prepare("SELECT Blue1, Id from result3 WHERE Serial = :Serial");
    getBlueBallQuery.bindValue(":Serial", serial);
    getBlueBallQuery.exec();
    getBlueBallQuery.first();
    // place the query to right entry
    int buleBall = getBlueBallQuery.value(0).toInt();

    QSqlQuery delete_query("DELETE from result3 WHERE Serial = " + QString::number(serial));
    this->recalculateID();
    this->calculateGaps(getBlueBallQuery.value(1).toInt());
   // deleteResultPrivate(Id);
   // // for the sister table
   // bool current_separate = m_red_blue_separated;
   // setRedBlueSeparated(!isRedBlueSeparated());
   // deleteResultPrivate(Id);
   // // switch back to the current table
   // setRedBlueSeparated(current_separate);
}

void LotteryTableModel::deleteAllResults( )
{
    // delete from result3
    QSqlQuery delete_query1("delete from result3", this->database());
    // fetch all from the data base to get correct parameters
    while(canFetchMore())
        fetchMore();
    this->select();
}

void LotteryTableModel::setBlueBallSelect(int num)
{
    m_selected_blue_ball = num;
    setTable(determineTableName(num, m_red_blue_separated));
    this->select();
}

void LotteryTableModel::setRedBlueSeparated(bool separated)
{
    m_red_blue_separated = separated;
    setTable(determineTableName(m_selected_blue_ball, separated));
    // repopulate the table with correct values
    this->select();
}

const QMap<QString, int> LotteryTableModel::findGapsForRecord(const QSqlRecord &record, int blueBall, bool separated)
{
    int record_serial = record.value("Serial").toInt();
    bool found_flag = false;
    QMap<QString, int> gap_map;
    QString field_name;
    for(int i = 1; i <= 7; ++i)
    {
        // numbers in the record.
        QString ball_string = "Red" + QString::number(i);
        if (i == 7)
            ball_string = "Blue1";
        int ball_index = record.indexOf(ball_string);
        int number2check = record.value(ball_index).toInt();
        QSqlQuery comparand_query(this->database());
        QString comparand_query_str;
        if (blueBall == 0)// select all blue balls
        {
            comparand_query_str = "SELECT Serial Red1, Red2, Red3, Red4, Red5, Red6, Blue1 from result3 where Serail <= " + QString::number(record_serial) + " ORDER BY Serial";
        }
        else // for a specific blue ball
        {
            comparand_query_str = "SELECT Serial Red1, Red2, Red3, Red4, Red5, Red6, Blue1 from result3 where Serial <= " + QString::number(record_serial) + " AND Blue1 = " + QString::number(blueBall) + " ORDER BY Serial";
        }
        comparand_query.exec(comparand_query_str);
        comparand_query.last();
        int count = 0;
        while(comparand_query.previous()) // start from the previous record to the selected record
        {
            // when checking blue ball
            if (i == 7 && separated)
            {
                field_name = "Blue1";
                                // only compare with blue ball in the comparand query
                int comparand = comparand_query.value(7).toInt();
                if (number2check == comparand)
                {
                    // the number is found in the comparand record
                    found_flag = true;
                    gap_map.insert(field_name + "Gap", count);
                    //break;  // jump out of the for loop
                }

            }
            else
            {
                field_name = "Red" + QString::number(i - 1);
                // 6 means the check only goes to the ball of Red6
                int check_upper_limit = 6;
                if (!separated) // if the red and blue balls are treaded differently
                {
                    // the check goes to the ball of Blue1
                    check_upper_limit = 7;
                }
                if (i == 7)
                {
                    field_name = "Blue1";
                }
                for (int j = 1; j <= check_upper_limit; ++j)
                {
                    int comparand = comparand_query.value(j).toInt();
                    if (number2check == comparand)
                    {
                        found_flag = true;

                        gap_map.insert(field_name + "Gap", count);

                        break;  // jump out of the for loop
                    }
                }
            }
            if (found_flag == true) // has found number in comparand record
            {
                found_flag = false;
                break;
            }
            else // no number found in the comparand
            {

                // comparand is the first record already
                if (comparand_query.at() == 0)
                {
                    gap_map.insert(field_name + "Gap", -1); // -1 stands for the number has not showd up yet
                }
            }
            count++; // increase the record counter to indicate one more record has been scan
        }
    }


    // the table that is going to accept the gap values
    QString gap_table_name = determineTableName(blueBall, separated);
    QMap<QString, int>::const_iterator it = gap_map.begin();
    QSqlQuery gap_query;
    gap_query.prepare("UPDATE "  + gap_table_name + " SET Red1Gap=:Red1Gap, Red2Gap=:Red2Gap, Red3Gap=:Red3Gap, Red4Gap=:Red4Gap, Red5Gap=:Red5Gap, Red6Gap=:Red6Gap, Blue1Gap=:Blue1Gap, Prediction = :Prediction WHERE Serial = :Serial");

    gap_query.bindValue(":Serial", record_serial);
    while(it != gap_map.end()) // traverse through the map
    {
        gap_query.bindValue(":"+it.key(), QVariant(it.value()));
        it++;
    }
    gap_query.exec();
    return gap_map;
}


QString LotteryTableModel::selectStatement() const
{
    // 0 means display all
    if (m_selected_blue_ball == 0)
    {
        QString gap_table = "gap_all";
        m_red_blue_separated ? gap_table += "_sp" : gap_table += "_nsp";
        QString select_statement = "SELECT result3.Serial, result3.Date, result3.Red1, result3.Red2, result3.Red3, result3.Red4, result3.Red5, result3.Red6, result3.Blue1,"
                + gap_table + ".Red1Gap," + gap_table + ".Red2Gap," + gap_table + ".Red3Gap,"  + gap_table + ".Red4Gap," + gap_table + ".Red5Gap," + gap_table + ".Red6Gap,"  + gap_table + ".Blue1Gap," + gap_table + ".Prediction," + gap_table + ".Calculation "
               "FROM result3 INNER JOIN "  + gap_table +  " on result3.Serial = "  + gap_table + ".Serial ORDER BY Serial";
        return select_statement;
    }
    else
    {
        QString gap_table = "gap_blue" + QString::number(m_selected_blue_ball);
        m_red_blue_separated ? gap_table += "_sp" : gap_table += "_nsp";
        QString select_statement = "SELECT result3.Serial, result3.Date, result3.Red1, result3.Red2, result3.Red3, result3.Red4, result3.Red5, result3.Red6, result3.Blue1,"
                + gap_table + ".Red1Gap," + gap_table + ".Red2Gap," + gap_table + ".Red3Gap,"  + gap_table + ".Red4Gap," + gap_table + ".Red5Gap," + gap_table + ".Red6Gap,"  + gap_table + ".Blue1Gap," + gap_table + ".Prediction," + gap_table + ".Calculation "
               "FROM result3 INNER JOIN "  + gap_table +  " on result3.Serial = "  + gap_table + ".Serial ORDER BY Serial";
        return select_statement;
    }
}

QString LotteryTableModel::prediction(int row) const
{
    QSqlRecord record = this->record(row);
    int red1gap_index = record.indexOf("Red1Gap");
    int blue1gap_index = record.indexOf("Blue1Gap");
    QList<int> gap_list;
    for (int i = red1gap_index; i <= blue1gap_index; ++i)
    {
        gap_list.append(record.value(i).toInt());
    }
    insertionSort(gap_list);
    QString ret_str;
    for (int i = 0; i < gap_list.size(); ++i)
    {
        ret_str += (QString::number(gap_list.at(i)) + " ");
    }
    return ret_str;

}


void LotteryTableModel::deleteResultPrivate(int Id)
{
    int deleted_row = Id - 1; // The record's Id that is deleted

    // when a invalid Id is entered

    int num_of_remainning_row = rowCount() - 1;
    // the row number deleted
    QSqlDatabase db = QSqlTableModel::database();
    QSqlQuery my_query(db);

    my_query.prepare("DELETE FROM " + table_name + " WHERE Serial = :Serial");
    my_query.bindValue(
                ":Serial", QVariant(Id));

    bool delete_ok = my_query.exec();

    // if the query can not be executed against the database
    if (!delete_ok)
    {
        QMessageBox msgBox;
        msgBox.setText("删除开奖结果失败,请重试");
        msgBox.exec();
        return;
    }

    // recalculate the gaps
    for (int current_row = deleted_row; current_row < num_of_remainning_row; ++current_row)
    {
        QSqlRecord record = this->record(current_row);
        // update the Id field for the following records
        int new_id = record.value("Id").toInt() - 1;
        record.setValue("Id", new_id);
        //QMap<QString, int> gap_map = findGapsForNumbers(record, current_row);
        //fillRecord(&record, gap_map);
    }
}

void LotteryTableModel::recalculateID()
{
    int id = 1;
    QSqlQuery select_query("SELECT Serial, Id from result3 ORDER by Serial", this->database());
    select_query.first();
    QSqlQuery update_query("UPDATE result3 SET Id = :Id WHERE Serial = :Serial");

    // recalculate the IDs for the entries in serial order
    do
    {
        update_query.bindValue(":Id", id++);
        update_query.bindValue(":Serial", select_query.value(0));
        update_query.exec();
    }
    while(select_query.next());

}

void LotteryTableModel::insertSerialIntoGapTable(int serial, int blueBall, bool separated)
{
    QString table_name = determineTableName(blueBall, separated);
    QSqlQuery insert_query(this->database());
    insert_query.prepare("INSERT INTO " + table_name + " (Serial) VALUES (:Serial)");
    insert_query.bindValue(":Serial", serial);
    qDebug() << insert_query.exec();
}



//! [quoting mymodel_f]
