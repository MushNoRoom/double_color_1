#ifndef DATAIMPORTER_H
#define DATAIMPORTER_H

#include <QObject>
#include <QString>
#include <QFile>

class MyModel;
class QProgressBar;

class DataImporter : public QObject
{
    Q_OBJECT
public:
    explicit DataImporter(const QString& fileName, QObject *parent = 0);
    virtual ~DataImporter();
    bool fileOk() const
    {
        return m_fileOk;
    }

    public slots:
        /*!
         * \brief startImporting start import data into model's data base
         * \param model the pointer to the model object
         */
        void startImporting(MyModel* model);

    signals:
        void doneImporting();



    private:
        QProgressBar* import_progress;
        QFile m_file_handler;
        int m_blue_ball;
        bool m_fileOk;
    
};

#endif // DATAIMPORTER_H
