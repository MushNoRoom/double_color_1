#ifndef DCLTEST_H
#define DCLTEST_H

#include <QSqlDatabase>

class MyModel;


class DclTest
{

public:
    static MyModel* m_testModel;


public:
    static void createDataBaseAndTables();
    static void startInsertTest();
    static MyModel* getModel();
    static void stopTesting();

private:
    DclTest();

};

#endif // DCLTEST_H
