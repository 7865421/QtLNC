#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QInputDialog>
void MainWindow::FtpInit()
{
    qDebug()<<"Ftp init start In ftpinit fuction";

//#ifdef _WIN32
    ui->btFtpGotoParent->setIcon(QPixmap(":/images/up.png"));
//#endif

    qDebug()<<"Ftp init Iocn In ftpinit fuction";
/*
    ui->btFtpDownload->setEnabled(false);
    ui->treeWidget->setEnabled(false);
    ui->btFtpGotoParent->setEnabled(false);
    ui->btMkdir->setEnabled(false);
    ui->btFtpUpload->setEnabled(false);
*/
    qDebug()<<"Ftp init enable In ftpinit fuction";

    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->setHeaderLabels(QStringList() << "文件" << "文件大小" << "修改时间");

  //  ui->treeWidget->header()->setMinimumSectionSize(100);
  //  ui->treeWidget->header()->resizeSection(0, 400);
  //  ui->treeWidget->header()->setStretchLastSection(true);
    ui->treeWidget->setSortingEnabled(true);


  //  ui->leFtpPort->setValidator(new QIntValidator(1, 65535, this));


    isAlreadyConnected = false;

    isProcessed = false;

    qDebug()<<"Ftp init connections In ftpinit fuction";
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(enableDownload()));
    connect(ui->btFtpConnect, SIGNAL(clicked()), this, SLOT(connectDisconnect()));
    connect(ui->btFtpDownload, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(ui->btFtpGotoParent, SIGNAL(clicked()), this, SLOT(goToParent()));
    connect(ui->btFtpUpload, SIGNAL(clicked()), this, SLOT(uploadFile()));
    connect(ui->btMkdir, SIGNAL(clicked()), this, SLOT(FtpMkdir()));

    qDebug()<<"Ftp init done In ftpinit fuction";

}



QString MainWindow::_FromSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return  QString::fromLocal8Bit(InputStr.toLatin1());
#else
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    if (codec)
    {
        return codec->toUnicode(InputStr.toLatin1());
    }
    else
    {
        return QString("");
    }
#endif
}

QString MainWindow::_ToSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return QString::fromLatin1(InputStr.toLocal8Bit());
#else
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
#endif
}


void MainWindow::connectDisconnect()
{
    if (isAlreadyConnected)
        disconnectFTP();
    else
        connectToFTP();
}


void MainWindow::connectToFTP()
{
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(processItem(QTreeWidgetItem*, int)));

    ui->treeWidget->clear();
    currentPath.clear();
    isDirectory.clear();

    bool isAddress = true, isUsername = true, isPort = true;


    QString address(ui->leFtpServerIP->text());
    if (address == "") {
        QString msg="FTP登录时，IP地址为空";
        const_msg(msg);
        isAddress = false;
    }

    QString port_str = ui->leFtpPort->text();
    int port = 21;
    if (port_str != "")
        port = port_str.toInt();
    if (port == 0 || port > 65535)
        isPort = false;


    QString username = ui->leFtpUserName->text();
    if (username == "") {
        QString msg="FTP登录时，用户名为空";
        const_msg(msg);
        isUsername = false;
    }

    QString password = ui->leFtpPassword->text();

    if (isAddress && isPort && isUsername) {
        ftp->connectToHost(address, port);
        ftp->login(username, password);
    } else {
        QString message("登录不正确:");
        if (!isAddress)
            message += "<br><br><b>地址不对.</b>";
        if (!isPort)
            message += "<br><br><b>端口不对.</b>";
        if (!isUsername)
            message += "<br><br><b>用户名不对.</b>";
        QMessageBox::warning(this, "Qt FTP 客户端出错", message);
    }
}


void MainWindow::disconnectFTP()
{
    if (ftp) {
        ftp->abort();
        ftp->deleteLater();
        ftp->close();
        ftp = 0;

        QString msg="断开FTP链接";
        const_msg(msg);
        isAlreadyConnected = false;
        isDirectory.clear();
        ui->treeWidget->clear();
        ui->treeWidget->setEnabled(false);


        ui->btFtpGotoParent->setEnabled(false);
        ui->btFtpDownload->setEnabled(false);
        ui->btFtpConnect->setText("链接");
    }
}

//下载文件列表
void MainWindow::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    //item->setText(0, urlInfo.name());
    item->setText(0, _FromSpecialEncoding(urlInfo.name()));
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.lastModified().toString("yyyy-MM-dd HH:mm:ss"));


    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(0, pixmap);

    isDirectory[urlInfo.name()] = urlInfo.isDir();
    ui->treeWidget->addTopLevelItem(item);
    if (!ui->treeWidget->currentItem()) {
        ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
        ui->treeWidget->setEnabled(true);
    }
}

// 处理各项
void MainWindow::processItem(QTreeWidgetItem *item, int)
{

    if (!isProcessed) {
        QString name = _ToSpecialEncoding(item->text(0));
        if (isDirectory.value(name)) {
            ui->treeWidget->clear();
            isDirectory.clear();
            currentPath += '/';
            currentPath += name;
            ftp->cd(name);
            ftp->list();
            ui->btFtpGotoParent->setEnabled(true);
            isProcessed = true;
        }
    }
}

// 返回上一级
void MainWindow::goToParent()
{
    ui->treeWidget->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    if (currentPath.isEmpty()) {
        ui->btFtpGotoParent->setEnabled(false);
        ftp->cd("/");
    } else {
        ftp->cd(currentPath);
    }
    ftp->list();
    isProcessed = false;
}

// FTP操作完成
void MainWindow::ftpCommandFinished(int, bool error)
{
    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QString msg="链接到FTP服务器失败！";
            const_msg(msg);
           // QMessageBox::warning(this,"ftp链接失败", "链接到主机错误.");
        } else
        {
            QString msg="链接到FTP服务器成功！";
            const_msg(msg);
        }
    } else if (ftp->currentCommand() == QFtp::Login) {
        if (error) {
            QString msg="登录到FTP服务器失败！";
            const_msg(msg);
        } else {
            isAlreadyConnected = true;
            ui->btFtpConnect->setText("断开");
            ui->btFtpConnect->setEnabled(true);

            ui->treeWidget->setEnabled(true);
            ui->btMkdir->setEnabled(true);
            ui->btFtpUpload->setEnabled(true);
            ftp->list();
            QString msg="登录到FTP服务器成功！";
            const_msg(msg);
        }
    } else if (ftp->currentCommand() == QFtp::Get) {
        if (error) {
           // ui->statusbar->showMessage(tr("下载文件%1失败.").arg(downloadfile->fileName()));
            downloadfile->close();
            downloadfile->remove();
            QString msg=tr("下载文件%1失败.").arg(downloadfile->fileName());
            const_msg(msg);
           // disconnectFTP();
           // connectToFTP();

        } else {
           //ui->statusbar->showMessage(tr("成功下载文件 %1.").arg(downloadfile->fileName()));
            downloadfile->close();
            QString msg=tr("下载文件%1成功.").arg(downloadfile->fileName());
            const_msg(msg);
        }
        delete downloadfile;
        enableDownload();
    } else if (ftp->currentCommand() == QFtp::List) {
        if (isDirectory.isEmpty()) {
            ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << "<到达顶部>"));
            ui->treeWidget->setEnabled(false);
        } else
            isProcessed = false;
    } else if (ftp->currentCommand() == QFtp::Close) {
       // ui->statusbar->showMessage("断开链接.");
        QString msg="FTP断开练级";
        const_msg(msg);
        ui->btMkdir->setEnabled(false);
        ui->btFtpUpload->setEnabled(false);
    }
    else if (ftp->currentCommand() == QFtp::Put) {
        if(error)
        {
           // ui->statusbar->showMessage(tr("文件%1上传失败").arg(uploadfile->fileName()));
            QString msg=tr("文件%1上传失败").arg(uploadfile->fileName());
            const_msg(msg);
           // disconnectFTP();
           // connectToFTP();
        }
        else
        {
           // ui->statusbar->showMessage(tr("文件%1上传完成").arg(uploadfile->fileName()));
            QString msg=tr("文件%1上传完成！").arg(uploadfile->fileName());
            const_msg(msg);
            uploadfile->close();
           // uploadfile->remove();
        }

        }//put
    else if (ftp->currentCommand() == QFtp::Mkdir) {
        if(error)
        {
           // ui->statusbar->showMessage(tr("创建文件夹失败"));
            QString msg="FTP服务器创建文件夹失败!";
            const_msg(msg);
        }
        else
        {
            //ui->statusbar->showMessage(tr("创建文件夹成功"));
            QString msg="FTP服务器创建文件夹成功!";
            const_msg(msg);
            ui->treeWidget->clear();
            ftp->cd(currentPath);
            ftp->list();
        }

        }//mkdir
}

// 使能下载按钮
void MainWindow::enableDownload()
{
    QTreeWidgetItem *current = ui->treeWidget->currentItem();
    if (current) {
        QString currentFile = current->text(0);
        ui->btFtpDownload->setEnabled(!isDirectory.value(currentFile));
    } else {
        ui->btFtpDownload->setEnabled(false);
    }
}



// 下载文件
void MainWindow::downloadFile()
{

    QString fileName = ui->treeWidget->currentItem()->text(0);
    QString fileToDownload = QFileDialog::getSaveFileName(this, "选择保存文件位置...", fileName);

    downloadfile = new QFile(fileToDownload);
    if (!downloadfile->open(QIODevice::WriteOnly)) {
        ui->statusbar->showMessage(tr("打开文件失败 %1.").arg(fileName));
        delete downloadfile;
        return;
    }
    if(isAlreadyConnected)
        ftp->get(_ToSpecialEncoding(ui->treeWidget->currentItem()->text(0)), downloadfile);
}

// 上传文件
void MainWindow::uploadFile()
{
    QString fileName=QFileDialog::getOpenFileName(this,"选择需要上传的文件","C:","所有文件(*.*)");

    uploadfile = new QFile(fileName);
    QFileInfo fileinfo = QFileInfo(fileName);
  //  qDebug()<<"选择文件"<<fileName<<"文件名"<<fileinfo.fileName();

    if(!uploadfile->open(QIODevice::ReadOnly)){
        ui->statusbar->showMessage(tr("打开文件失败 %1.").arg(fileName));
        delete uploadfile;
        return;
    }
    else
    {
        if(isAlreadyConnected){
        ftp->setTransferMode(QFtp::Active);
        ftp->put(uploadfile->readAll(),_ToSpecialEncoding(fileinfo.fileName()));
        //刷新列表
        ui->treeWidget->clear();
        ftp->cd(currentPath);
        ftp->list();
        }
        else
         delete uploadfile;
    }

  //  QString srcfile=m_IniFile->value("mesFormalFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
  //  QString despath=m_IniFile->value("mesFtpFormalHostDir").toString();
  //  FtpUploadFileToFixPath(srcfile,despath);

}

//根据文件名上传到对应目录
void MainWindow::FtpUploadFileToFixPath(QString srcfile,QString desPath)
{

    if(isAlreadyConnected)
    {
        uploadfile = new QFile(srcfile);
        QFileInfo fileinfo = QFileInfo(srcfile);

       // qDebug()<<"选择文件"<<srcfile<<"文件名"<<fileinfo.fileName();

        if(!uploadfile->open(QIODevice::ReadOnly)){
            ui->statusbar->showMessage(tr("打开文件失败 %1.").arg(srcfile));
            delete uploadfile;
            return;
        }
        else
        {
            if(isAlreadyConnected){
            ftp->setTransferMode(QFtp::Active);
            ftp->put(uploadfile->readAll(),_ToSpecialEncoding(desPath+"/"+fileinfo.fileName()));
            }
            else
            delete uploadfile;
        }
    }
}


//根据文件名上传到对应目录
void MainWindow::FtpMkdir()
{
    bool isOK=false;
    QString text = QInputDialog::getText(NULL, "新建文件夹对话框",
                                                       "请输入新建文件夹名称",
                                                       QLineEdit::Normal,
                                                       "新建文件夹",
                                                       &isOK);
    if(isOK) {

           QString dir=currentPath+"/"+_ToSpecialEncoding(text);
           ftp->mkdir(dir);

    }

}
