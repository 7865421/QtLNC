#ifndef SAVEEXCELTHEAD_H
#define SAVEEXCELTHEAD_H


#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QObject>
#include <QThread>

#include "excel/ExcelBase.h"

class saveExcelThead : public QThread
{
    Q_OBJECT
signals:
    void SendProgress(const int& percent);
    void SaveDone(const QString& filename);
public:
    saveExcelThead(QObject* par);
    ~saveExcelThead();
    void setModel(void *model);

private:
    void run();
    void saveExcelSqlmodel();
    void saveExcelStdmodel();

public:
    QSqlQueryModel *sqlmodel=NULL;
    QStandardItemModel *stdmodel=NULL;
    QString saveFileName;
    QString xlsFile;//保存文件名
    QString sheetTabName;
    int type;
    bool is_running=false;

private:
    QScopedPointer<ExcelBase> m_xls;
};

#endif // SAVEEXCELTHEAD_H
