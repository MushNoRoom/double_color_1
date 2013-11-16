#ifndef DCLTEST_H
#define DCLTEST_H

#include <QSqlDatabase>

class LotteryTableModel;


class DclTest
{

public:
    static LotteryTableModel* m_testModel;


public:
    static void createDataBaseAndTables();
    static void startInsertTest();
    static LotteryTableModel* getModel();
    static void stopTesting();

private:
    DclTest();

};

#endif // DCLTEST_H
