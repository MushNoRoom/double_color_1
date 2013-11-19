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

#ifndef MYMODEL_H
#define MYMODEL_H

//! [Quoting ModelView Tutorial]
// mymodel.h
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QString>
#include <QMap>



const int COLS= 3;
const int ROWS= 2;

typedef int year_t;

/*!
 * \brief The DCLTableModel class is a class that represents the data of the lottery.
 *
 *  This is the model in the MVC architecture. Its backend is a MySQL database.
 *  Inherit QSqlTableModel.
 */
class LotteryTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    LotteryTableModel(QObject *parent, QSqlDatabase db = QSqlDatabase());
    ~LotteryTableModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    int rowCount(const QModelIndex& parent = QModelIndex());

signals:
    //! Emitted whenver a new result is inserted
    void newResultInserted();
    void oneGapTableFinished();

public slots:
    /*!
     * \brief acceptNewResultData accept a new lottery result
     * \param serial the serial number of the result
     * \param resultDate date of the result
     * \param redBalls vector of all the red ball numbers
     * \param blueBall vector of all the blue ball numbers (there is only one item in it)
     */
    void acceptNewResultData(const int serial, const QDate& resultDate, const QVector<quint32>& redBalls,
                             const QVector<quint32>& blueBall);
    /*!
     * \brief detele a entry with the serial number \e serial
     * \param serial the serial number of the entry to be deleted.
     */
    void deleteResult(int serial);
    /*!
     * \brief deleteAllResults delete all the results in the database (the .db files)
     */
    void deleteAllResults();
    /*!
     * \brief setBlueBallSelect set the displayed blue ball to \e num
     * \param num the number of blue ball that user want to view
     */
    void setBlueBallSelect(int num);
    /*!
     * \brief setRedBlueSeparated make red and blue balls be treated equally or not
     */
    void setRedBlueSeparated(bool separated);
    /*!
     * \brief calculateAllGaps calculate gaps for all the records in the database
     */
    void calculateAllGaps();


public:
    /*!
     * \brief findGapsForRecord find gaps for the balls in \e record
     * \param record the QSqlRecord whose gaps need to be found
     * \param blueBall specific for blue ball number
     * \param separated if the gap is be looked for with blue ball treated differently
     * \return
     */
    const QMap<QString, int> findGapsForRecord(const QSqlRecord& rec, int blueBall = 0, bool separated = false);

    /*!
     * \brief isRedBlueSeparated return if the model treats red and blue ball differently
     */
    bool isRedBlueSeparated()
    {
        return m_red_blue_separated;
    }

protected:
    /*!
     * \brief selectStatement reimplemented from QSqlTableModel::selectStatement()
     * \return the filter string
     */
    QString selectStatement() const;
    /*!
     * \brief prediction how the prediction section is displayed
     * \param row prediction string's row index
     * \return the content rendered in the form of text
     */
    virtual QString prediction(int row) const;

private:
    //! Private implementation of delete result
    void deleteResultPrivate(int id);
    void recalculateID();
    void insertSerialIntoGapTable(int serial, int blueBall, bool separated = true);

private:
    QSqlDatabase m_db;
    int m_selected_blue_ball; // the blue ball selected to view. 0 for all numbers
    bool m_red_blue_separated; // if the red and blue balls are treated separatly
    QMap<year_t, int> m_year_amount_map; // map contains the amount of results in each year

};
//! [Quoting ModelView Tutorial]

#endif // MYMODEL_H
