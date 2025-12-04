#ifndef SAVEASCSV_H
#define SAVEASCSV_H

#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QObject>
#include <QThread>
#include <QString>

class saveAsCsv : public QThread
{
    Q_OBJECT
signals:
    void SaveDone(const QString& filename);
public:
    saveAsCsv(QObject* par);
    ~saveAsCsv();

private:
    void run();
    void ReadDataFromSqlWriteToCSV();//将表导出为csv格式表格

public:
    QSqlQueryModel *sqlmodel=NULL;
    QString saveFileName;//保存文件名
    QString logtype;
    bool is_running=false;
    void setModel(void *model);

};

#endif // SAVEASCSV_H
