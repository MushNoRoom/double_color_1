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
#include <QTableView>
#include <QAction>
#include <QMenuBar>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QHeaderView>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDebug>
#include <QScrollBar>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QScopedPointer>
#include <QHash>
#include <QStringList>
#include <QPluginLoader>
#include <QErrorMessage>
#include <QThread>
#include <QScopedPointer>
#include <QProgressBar>
#include <QSettings>
#include "dataimporter.h"
#include "dataimporterproxy.h"
#include "mainwindow.h"
#include "mymodel.h"
#include "newresultwindow.h"
#include "deleteresultwindow.h"
#include "lotterydockwidget.h"
#include "string.h"

char buffer[64];

inline void reOrderHeader(QHeaderView* header)
{
    int target_visual_index = 3;
    // reorder the ball number and gap columns
    for (int i = 9; i < 15; ++i)
    {
        int current_visual_index = i;
        header->moveSection(current_visual_index, target_visual_index);
        target_visual_index += 2;
    }
    // move the blue ball section
    header->moveSection(14, 2);
    header->moveSection(15, 3);

    QSettings app_settings;
    int num_of_sections = header->count();
    for (int i = 0; i < num_of_sections; ++i)
    {
        QString header_section_width = "GUI/hHeaderSection" + QString::number(i) + "Width";
        int section_width = app_settings.value(header_section_width).toInt();
        if (section_width != 0)
            header->resizeSection(i, section_width);
    }
}

inline void updateModelView(LotteryTableModel& model, QTableView& view)
{
    model.select();
    reOrderHeader(view.horizontalHeader());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_sqlModel(0),
      m_workerThread(new QThread),
      m_proxy(0)
{
    QCoreApplication::addLibraryPath("./sqldrivers");
    QPluginLoader p_loader(QCoreApplication::applicationDirPath() + "/sqldrivers/qsqlmysql4.dll");
    p_loader.load();
    qDebug() << p_loader.isLoaded();

    qDebug() << "Starting new window " << QThread::currentThread();
    tableView = new QTableView(this);
    tableView->horizontalHeader()->setMovable(true);
    QString newMenuTitle = QString::fromUtf8("编辑");

    QMenu* new_menu = QMainWindow::menuBar()->addMenu(newMenuTitle);
    QAction* new_result_act = new_menu->addAction(QString::fromUtf8("新建开奖结果"));
    QAction* delete_result_act = new_menu->addAction(QString::fromUtf8("删除结果"));
    // add a new separator
    new_menu->addSeparator();
    // the action use to trigger the data import functin
    QAction* import_data_act = new_menu->addAction(QString::fromUtf8("导入开奖结果"));



    connect(new_result_act, SIGNAL(triggered(bool)), SLOT(onNewActionClick()));
    connect(delete_result_act, SIGNAL(triggered(bool)), SLOT(onDeleteActionClicked()));
    connect(import_data_act, SIGNAL(triggered(bool)), SLOT(onImportDataClicked()));
    setCentralWidget(tableView);

    LotteryDockWidget *myDockWidget = new LotteryDockWidget(this);
    QWidget *title_bar = new QWidget(this);
    // trick to remove the title bar of dock widget
    myDockWidget->setTitleBarWidget(title_bar);
    myDockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, myDockWidget);

    // hash table for setting strings
    QHash<QString, QString> config_hash;
    // open the configuration file
    QFile config_file(QCoreApplication::applicationDirPath() + "/config.txt");
    config_file.open(QIODevice::ReadOnly);

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

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(config_hash.value("host_name"));
    db.setUserName(config_hash.value("user"));
    db.setPort(3306);
    db.setDatabaseName(config_hash.value("database"));
    db.setPassword(config_hash.value("password"));


    bool open_ok = db.open();


    if (!open_ok)
    {
        QErrorMessage err_msg;
        QString error_str = QString::fromUtf8("开启数据库失败.请重启程序") + "\n" + db.lastError().text() +
                "\n" + QString::number(db.lastError().type()) + "\n" + db.hostName() + db.password() + db.userName() + db.databaseName();
        if (!p_loader.isLoaded())
            error_str += "\Plugin not loaded";
        err_msg.showMessage(error_str);
        err_msg.exec();
        exit(-1);
    }

    m_sqlModel = new LotteryTableModel(0, db);

    connect(myDockWidget, SIGNAL(blueBallSelectChanged(int)),
            m_sqlModel, SLOT(setBlueBallSelect(int)));
    connect(myDockWidget, SIGNAL(redBlueSeparateChanged(bool)),
            m_sqlModel, SLOT(setRedBlueSeparated(bool)));
    // use the current selection from the dock widget to dertermine how the model selects data
    m_sqlModel->setRedBlueSeparated(myDockWidget->isRedBlueSeparated());
    // call this function to make sure that the right table is used
    // use previsouly set boolean value
    m_sqlModel->setRedBlueSeparated(m_sqlModel->isRedBlueSeparated());


    tableView->setModel(m_sqlModel);
    tableView->verticalHeader()->hide();

    m_sqlModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_sqlModel->select();
    m_sqlModel->query().first();

    connect(m_sqlModel, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(submitCheck(int,QSqlRecord&)));
    reOrderHeader(tableView->horizontalHeader());
}

MainWindow::~MainWindow()
{
    // Delete the sql model created
    QSettings settings;
    QHeaderView* horizontal_header = tableView->horizontalHeader();
    int num_of_sections = horizontal_header->count();
    for (int i = 0; i < num_of_sections; ++i)
    {
        QString header_section_width = "GUI/hHeaderSection" + QString::number(i) + "Width";
        settings.setValue(header_section_width, horizontal_header->sectionSize(i));
    }
    // close the database
    db.close();
    delete m_sqlModel;
}

void MainWindow::showWindowTitle(const QString & title)
{
    setWindowTitle(title);
}

void MainWindow::onNewActionClick()
{
    NewResultWindow *new_result_window = new NewResultWindow();
    new_result_window->show();
    if (m_sqlModel == 0)
        qDebug() << "invalid model";

    connect(new_result_window, SIGNAL(newResultConfirmed(int, QDate,QVector<quint32>,QVector<quint32>)),
            m_sqlModel, SLOT(acceptNewResultData(int, QDate,QVector<quint32>,QVector<quint32>)));
}

void MainWindow::onDeleteActionClicked()
{
    DeleteResultWindow *delete_result_window = new DeleteResultWindow();
    delete_result_window->show();

    connect(delete_result_window, SIGNAL(deleteIdEntered(int)), m_sqlModel,
            SLOT(deleteResult(int)));
}

void MainWindow::onImportDataClicked()
{
    // gets the file that contains the data
    QString file_name = QFileDialog::getOpenFileName(0, QString(), QString(), "*.txt");
    // clear the data from the database before importing starts

    m_workerThread.reset(new QThread);
    m_importer.reset(new DataImporter(file_name));
    // create the dataimporter wrapper
    m_proxy.reset(new DataImporterProxy(m_importer.data(), m_sqlModel));
    m_proxy->moveToThread(m_workerThread.data());
    connect(m_workerThread.data(), SIGNAL(started()), m_proxy.data(), SLOT(proxyStartWork()));
    connect(m_importer.data(), SIGNAL(doneImporting()), m_workerThread.data(), SLOT(quit()));


    if (m_importer->fileOk())
    {
        import_progress = new QProgressBar;
        import_progress->setAttribute(Qt::WA_DeleteOnClose);
        import_progress->setMaximum(34);
        import_progress->setValue(0);
        connect(m_sqlModel, SIGNAL(oneGapTableFinished()), this, SLOT(addProgress()));
        connect(m_importer.data(), SIGNAL(doneImporting()), import_progress, SLOT(close()));
        m_workerThread->start();
        import_progress->show();
    }
}


void MainWindow::addProgress()
{
    if (import_progress == 0)
        return;
    int currentVal = import_progress->value();
    import_progress->setValue(currentVal + 1);
}

void MainWindow::submitCheck(int row, QSqlRecord& rec)
{
    int serial2update = m_sqlModel->record(row).value("Serial").toInt();
    QString cal_string = rec.value("Calculation").toString();
    QString table_name = m_sqlModel->tableName();
    QString stmt = "UPDATE " + table_name + " SET Calculation = '" + cal_string + "' WHERE Serial = " + QString::number(serial2update);
    QSqlQuery query(m_sqlModel->database());
    query.prepare(stmt);
    query.exec();
    m_sqlModel->select();

}
