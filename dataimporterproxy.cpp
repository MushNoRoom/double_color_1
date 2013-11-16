#include "dataimporterproxy.h"
#include "mymodel.h"
#include "dataimporter.h"

DataImporterProxy::DataImporterProxy(DataImporter* di, LotteryTableModel* model, QObject* parent):
    QThread(parent),
    m_di(di),
    m_model(model)
{
    connect(this, SIGNAL(startImport(LotteryTableModel*)), m_di, SLOT(startImporting(LotteryTableModel*)), Qt::DirectConnection);
    connect(m_di, SIGNAL(doneImporting()), SLOT(deleteImporter()));
}

void DataImporterProxy::proxyStartWork()
{
    emit startImport(m_model);
}


void DataImporterProxy::deleteImporter()
{
//    delete m_di;
    emit proxyDoneWork();;
}

void DataImporterProxy::run()
{
    exec();
}
