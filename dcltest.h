#ifndef DCLTEST_H
#define DCLTEST_H

#include <QSqlDatabase>
#include <QScopedPointer>
#include <QTest>
class LotteryTableModel;


/*!
 * \brief The DclTest class singleton class for testing purposes
 */
class DclTest
{

public:
    static DclTest* getTestInstance();
    ~DclTest();


public:
    void startTest();
    void testCreateTables();
    void testDeleteTables();
    void testImportFile();
private:
    LotteryTableModel* getModel();
private:
    static DclTest* test_instance;
    LotteryTableModel* m_testModel;
    DclTest();

};

#endif // DCLTEST_H
