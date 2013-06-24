#include "widget.h"
#include "ui_widget.h"
#include <QtAddOnSerialPort/serialportinfo.h>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    mySerialPort=new SerialPort(this);
    myTimer=new QTimer(this);
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(refreshAvSerialPort()));
    connect(mySerialPort, SIGNAL(readyRead()), this , SLOT(readToPlainText()));
    //ui->groupBox->setHidden(ui->toolButton_3->isChecked());
    //ui->groupBox->hide();
    // fill data bits
    ui->comboBox_3->addItem(QLatin1String("8"), SerialPort::Data8);
    ui->comboBox_3->addItem(QLatin1String("7"), SerialPort::Data7);
    ui->comboBox_3->addItem(QLatin1String("6"), SerialPort::Data6);
    ui->comboBox_3->addItem(QLatin1String("5"), SerialPort::Data5);
    ui->comboBox_3->setCurrentIndex(0);

    // fill stop bits
    ui->comboBox_4->addItem(QLatin1String("2"), SerialPort::TwoStop);
    #ifdef Q_OS_WIN
    ui->comboBox_4->addItem(QLatin1String("1.5"), SerialPort::OneAndHalfStop);
    #endif
    ui->comboBox_4->addItem(QLatin1String("1"), SerialPort::OneStop);
    ui->comboBox_4->setCurrentIndex(ui->comboBox_4->count()-1);

    // fill parity
    ui->comboBox_5->addItem(QLatin1String("None"), SerialPort::NoParity);
    ui->comboBox_5->addItem(QLatin1String("Even"), SerialPort::EvenParity);
    ui->comboBox_5->addItem(QLatin1String("Odd"), SerialPort::OddParity);
    ui->comboBox_5->addItem(QLatin1String("Mark"), SerialPort::MarkParity);
    ui->comboBox_5->addItem(QLatin1String("Space"), SerialPort::SpaceParity);
    ui->comboBox_5->setCurrentIndex(0);

    // fill flow control
    ui->comboBox_6->addItem(QLatin1String("None"), SerialPort::NoFlowControl);
    ui->comboBox_6->addItem(QLatin1String("RTS/CTS"), SerialPort::HardwareControl);
    ui->comboBox_6->addItem(QLatin1String("XON/XOFF"), SerialPort::SoftwareControl);
    ui->comboBox_6->setCurrentIndex(0);
    refreshAvSerialPort();
    //currPortName="";
    loadSettings();
    if(complList.isEmpty())
        complList<<"ATID"<<"ATCH"<<"ATPL"<<"ATRE"<<"ATCN"<<"ATAC"<<"ATFR"<<"ATAD"<<"ATVR"<<"ATSH"<<"ATSL";

    myCompl=new QCompleter(complList, this);
        myCompl->setCaseSensitivity(Qt::CaseInsensitive);

    ui->lineEdit->setCompleter(myCompl);
    for(qint32 i=1; i<ui->comboBox_2->count(); i++){
        if(ui->comboBox_2->itemText(i)==currPortName){
            ui->comboBox_2->setCurrentIndex(i);
            break;
        }

    }
    dozvilNaOnovlPortiv=1;
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(qint32)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(qint32)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(qint32)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(qint32)));
    connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndxToPushButton(qint32)));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_7, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->comboBox_8, SIGNAL(currentIndexChanged(int)), this, SLOT(focusLineEdit(qint32)));
    connect(ui->toolButton_2,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));
    connect(ui->toolButton_3,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));
    connect(ui->toolButton_4,SIGNAL(clicked()), this , SLOT(focusLineEdit2()));

    _name = "";
       _sok = new QTcpSocket(this);
       connect(_sok, SIGNAL(readyRead()), this, SLOT(onSokReadyRead()));
       connect(_sok, SIGNAL(connected()), this, SLOT(onSokConnected()));
       connect(_sok, SIGNAL(disconnected()), this, SLOT(onSokDisconnected()));
       connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSokDisplayError(QAbstractSocket::SocketError)));

}

Widget::~Widget()
{
    saveSettings();
    delete ui;
}

void Widget::on_lineEdit_returnPressed()
{
    QByteArray data=ui->lineEdit->text().toLocal8Bit();
    if(!data.isEmpty() && data.left(3)!="+++" && data.left(3)!="&&&") {

        QString str=ui->lineEdit->text().toUpper();
        for(qint32 i=0; i<complList.count(); i++){
            if(str==complList[i].toUpper()){
                complList.removeAt(i);
                break;
            }

        }
    }


    ui->plainTextEdit->moveCursor(QTextCursor::End);
    QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
    textCharFormat.setFontItalic(0);
    textCharFormat.setFontWeight(QFont::Bold);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);

    ui->plainTextEdit->insertPlainText(data+"\r\n");
    ui->plainTextEdit->moveCursor(QTextCursor::End);


    switch (ui->comboBox_7->currentIndex()) {
    case 0: { data+="\r\n"; break; }
    case 1: { data+="\r" ; break; }
    case 2: { data+="\n"; break; }
    }
    mySerialPort->write(data);
    selPosition=0;
    QTimer::singleShot(50, this, SLOT(clearLine()));
}

void Widget::on_toolButton_3_clicked(bool checked)
{
    ui->groupBox->setHidden(!checked);
    ui->groupBox_2->setHidden(!checked);
    ui->groupBox_3->setHidden(!checked);
    if(checked) {

        if(ui->pushButton->text()=="Cl&ose"){
            ui->comboBox->setEnabled(1);

        }
        ui->plainTextEdit->moveCursor(QTextCursor::End);

    }
    else {
        if(ui->pushButton->text()=="Cl&ose")
            ui->comboBox->setEnabled(0);
    }
}

void Widget::on_toolButton_4_clicked()
{
    QMessageBox::about(this, tr("About Terminalka"),
                       tr("<b>Simple serial port terminal.</b><br>"
                          "build date 2013-04-29<br>")+QString(QString::fromUtf8("©"))+tr(" Bogdan Zikranets, 2013"));
}

//-----------------------------------------------------------------------------------------------------------

bool Widget::openSerialPort()
{
    dozvilNaOnovlPortiv=1;
    currPortName=ui->comboBox_2->currentText();
    SerialPort::DataBits dataBits=static_cast<SerialPort::DataBits>(ui->comboBox_3->itemData((ui->comboBox_3->currentIndex())).toInt()); //currentText().toInt());
    SerialPort::Parity parity=static_cast<SerialPort::Parity>(ui->comboBox_5->itemData(ui->comboBox_5->currentIndex()).toInt());
    SerialPort::StopBits stopBits=static_cast<SerialPort::StopBits>(ui->comboBox_4->itemData(ui->comboBox_4->currentIndex()).toInt());
    SerialPort::FlowControl flowControl=static_cast<SerialPort::FlowControl>(ui->comboBox_6->itemData(ui->comboBox_6->currentIndex()).toInt());
    mySerialPort->setPort(ui->comboBox_2->currentText());
    if(mySerialPort->open(QIODevice::ReadWrite)) {
        if(mySerialPort->setRate(ui->comboBox->currentText().toInt()) && mySerialPort->setParity(parity) && mySerialPort->setDataBits(dataBits) && mySerialPort->setStopBits(stopBits) && mySerialPort->setFlowControl(flowControl)){

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(1);
            textCharFormat.setFontWeight(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->appendPlainText(tr("Opened <---> %0, %1\r\n").arg(ui->comboBox_2->currentText()).arg(ui->comboBox_2->itemData(ui->comboBox_2->currentIndex(), Qt::ToolTipRole).toString()));
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            textCharFormat.setFontItalic(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->lineEdit->setEnabled(1);
            if(ui->lineEdit->text()=="AT command")
                ui->lineEdit->clear();
            ui->pushButton->setText("Cl&ose");
            ui->lineEdit->setFocus();
            ui->pushButton_2->setEnabled(0);
            ui->comboBox->setEnabled(0);
            return 1;
        }
        else {
            mySerialPort->close();
            QMessageBox::critical(this, tr("Error"),tr("Can't configure the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
            refreshAvSerialPort();
        }

    }
    else{
        QMessageBox::critical(this, tr("Error"),tr("Can't opened the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
        refreshAvSerialPort();
    }


    return 0;
}

//-----------------------------------------------------------------------------------------------------------
void Widget::closeSerialPort()
{
    mySerialPort->close();
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
    textCharFormat.setFontItalic(1);
    textCharFormat.setFontWeight(0);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
    ui->plainTextEdit->appendPlainText(tr("Close >...<\r\n"));
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    textCharFormat.setFontItalic(0);
    ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
    ui->comboBox->setEnabled(1);
    if(ui->lineEdit->text().isEmpty())
        ui->lineEdit->setText("AT command");
    ui->lineEdit->setEnabled(0);
    ui->pushButton->setText("&Open");
}

//-----------------------------------------------------------------------------------------------------------
void Widget::refreshAvSerialPort()
{
    if(ui->pushButton->text()=="Cl&ose")
        closeSerialPort();
    dozvilNaOnovlPortiv=0;
    QStringList oldList;
    QStringList oldListInfo;
    QStringList newList;
    QStringList newListInfo;
    for(qint32 i=1; i<ui->comboBox_2->count(); i++) {
        oldList.append(ui->comboBox_2->itemText(i));
        oldListInfo.append(ui->comboBox_2->itemData(i, Qt::ToolTipRole).toString());
    }
    ui->comboBox_2->clear();
    ui->comboBox_2->addItem("REFRESH");
    foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts()) {
        newList<<info.portName();
        newListInfo<< info.description()+",   "+ info.manufacturer() +",   "+ info.systemLocation();
    }
    qint32 currIndex=0;

    for(qint32 i=0; i<oldList.count(); i++) {
            bool vidsutnij=1;
            for(qint32 j=0; j<newList.count();j++) {
                if(oldList[i]==newList[j]) {
                    newList.removeAt(j);
                    newListInfo.removeAt(j);
                    vidsutnij=0;
                    break;
                }
            }
            if(vidsutnij) {
                oldList.removeAt(i);
                oldListInfo.removeAt(i);
                i--;
            }
        }

        for(qint32 i=0; i<newList.count(); i++) {
                ui->comboBox_2->addItem(newList[i]);
             ui->comboBox_2->setItemData(i+1, newListInfo[i], Qt::ToolTipRole);
        }
        for(qint32 i=0, j=ui->comboBox_2->count(); i<oldList.count();i++){
            ui->comboBox_2->addItem(oldList[i]);
            ui->comboBox_2->setItemData(i+j, oldListInfo[i], Qt::ToolTipRole);
            if(currPortName==oldList[i])
                currIndex=i+j;
        }
//        qDebug()<<QString::number(ui->comboBox_2->count());
        if(currIndex==0) {
            if(ui->comboBox_2->count()>1)
                ui->comboBox_2->setCurrentIndex(1);
            else
                ui->comboBox_2->setCurrentIndex(0);
        }
        else
            ui->comboBox_2->setCurrentIndex(currIndex);

}

//-----------------------------------------------------------------------------------------------------------
void Widget::loadSettings()
{
    QSettings setting ("Hello_ZB","Qt_net_terminalka");
    setting.beginGroup("SerialPort");
   qint32 pIndex= setting.value("baud").toInt();
   QRect rect = setting.value("position").toRect();
   if(rect.x()< 10)
       rect.setX(10);
   if(rect.y()<30)
       rect.setY(31);
    setGeometry(rect);
   if(pIndex)
       ui->comboBox->setCurrentIndex((--pIndex));
   else
          ui->comboBox->setCurrentIndex(1);

    currPortName=setting.value("curPortName").toString();

    pIndex= setting.value("databits").toInt();
    if(pIndex)
        ui->comboBox_3->setCurrentIndex(--pIndex);
    else
        ui->comboBox_3->setCurrentIndex(0);

    pIndex= setting.value("stopbits").toInt();
    if(pIndex)
        ui->comboBox_4->setCurrentIndex(--pIndex);
    else
        ui->comboBox_4->setCurrentIndex(ui->comboBox_4->count()-1);

    pIndex= setting.value("parity").toInt();
    if(pIndex)
        ui->comboBox_5->setCurrentIndex(--pIndex);
    else
        ui->comboBox_5->setCurrentIndex(0);

    pIndex= setting.value("flowcontrol").toInt();
    if(pIndex)
        ui->comboBox_6->setCurrentIndex(--pIndex);
    else
        ui->comboBox_6->setCurrentIndex(0);

    pIndex= setting.value("crlfN").toInt();
    if(pIndex)
        ui->comboBox_7->setCurrentIndex(--pIndex);
    else
        ui->comboBox_7->setCurrentIndex(0);

    pIndex= setting.value("crlfFN").toInt();
    if(pIndex)
        ui->comboBox_8->setCurrentIndex(--pIndex);
    else
        ui->comboBox_8->setCurrentIndex(3);
    QString str=setting.value("fnKey").toString();
    if(!str.isEmpty())
        ui->lineEdit_2->setText(str);
    ui->checkBox->setChecked(!setting.value("atcc").toBool());
    //    setting.setValue("atcc", ui->checkBox->isChecked());

    ui->toolButton_3->setChecked(setting.value("vis").toBool());
    ui->groupBox->setHidden(!ui->toolButton_3->isChecked());
    ui->groupBox_2->setHidden(!ui->toolButton_3->isChecked());
    ui->groupBox_3->setHidden(!ui->toolButton_3->isChecked());
//    setting.setValue("crlfN", ui->comboBox_7->currentIndex()+1);
//    setting.setValue("crlfFN", ui->comboBox_8->currentIndex()+1);
//    setting.setValue("fnKey", ui->lineEdit_2->text());
    complList=setting.value("list").toStringList();
    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::saveSettings()
{
    QSettings setting ("Hello_ZB","Qt_net_terminalka");
    setting.beginGroup("SerialPort");
    setting.setValue("baud",(ui->comboBox->currentIndex()+1));
    setting.setValue("curPortName", currPortName);
    setting.setValue("position", this->geometry());
    setting.setValue("databits", (ui->comboBox_3->currentIndex()+1));
    setting.setValue("stopbits", (ui->comboBox_4->currentIndex()+1));
    setting.setValue("parity", (ui->comboBox_5->currentIndex()+1));
    setting.setValue("flowcontrol", (ui->comboBox_6->currentIndex()+1));
    setting.setValue("crlfN", ui->comboBox_7->currentIndex()+1);
    setting.setValue("crlfFN", ui->comboBox_8->currentIndex()+1);
    setting.setValue("fnKey", ui->lineEdit_2->text());
    setting.setValue("vis", ui->toolButton_3->isChecked());
    setting.setValue("atcc", !ui->checkBox->isChecked());
    while(complList.count()>100)
        complList.removeLast();
    setting.setValue("list", complList);
    setting.endGroup();
}

//-----------------------------------------------------------------------------------------------------------

void Widget::on_pushButton_clicked()
{
    if(ui->comboBox_2->currentText()=="REFRESH")
        refreshAvSerialPort();
    else{
        if(ui->pushButton->text()=="&Open")
            openSerialPort();
        else
            closeSerialPort();
    }
}


void Widget::on_comboBox_2_highlighted(const QString &arg1)
{
    if(!arg1.isEmpty() && (arg1=="REFRESH" || dozvilNaOnovlPortiv))
    refreshAvSerialPort();
}

void Widget::on_comboBox_2_activated(const QString &arg1)
{
    if(arg1=="REFRESH")
    {
         dozvilNaOnovlPortiv=0;
         refreshAvSerialPort();
    }
    else
    {
        if(ui->pushButton->text()=="&Open")
            openSerialPort();
        else
        {
            closeSerialPort();
            openSerialPort();
        }
    }
}
//-----------------------------------------------------------------------------------------------------------

void Widget::readToPlainText()
{
    QByteArray data=mySerialPort->readAll();
    if(!data.isEmpty()) {
        ui->plainTextEdit->moveCursor(QTextCursor::End);

        QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
        textCharFormat.setFontItalic(0);
        textCharFormat.setFontWeight(0);
        ui->plainTextEdit->setCurrentCharFormat(textCharFormat);

        ui->plainTextEdit->insertPlainText(data);
        ui->plainTextEdit->moveCursor(QTextCursor::End);
    }

}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
void Widget::clearLine()
{
    if(ui->lineEdit->isEnabled()){
        if(!ui->lineEdit->text().isEmpty() && ui->lineEdit->text().left(3)!="+++" && ui->lineEdit->text().left(3)!="&&&"){
            delete myCompl;
            if(selPosition)
                complList.insert(0,ui->lineEdit->text().left(ui->lineEdit->text().count()-1));
            else
                complList.insert(0,ui->lineEdit->text());

            myCompl=new QCompleter(complList, this);
            myCompl->setCaseSensitivity(Qt::CaseInsensitive);
            ui->lineEdit->setCompleter(myCompl);
        }
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();
    }

}



void Widget::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.right(1)==ui->lineEdit_2->text())
    {
        QByteArray data=arg1.left(arg1.count()-1).toLocal8Bit();
        if(!data.isEmpty() && data.left(3)!="+++" && data.left(3)!="&&&") {

            QString str=arg1.left(arg1.count()-1).toUpper();
            for(qint32 i=0; i<complList.count(); i++){
                if(str==complList[i].toUpper()){
                    complList.removeAt(i);
                    break;
                }

            }
        }

        ui->plainTextEdit->moveCursor(QTextCursor::End);
        QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
        textCharFormat.setFontItalic(0);
        textCharFormat.setFontWeight(QFont::Bold);
        ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
        ui->plainTextEdit->insertPlainText(data+"\r\n");
        ui->plainTextEdit->moveCursor(QTextCursor::End);


        switch (ui->comboBox_8->currentIndex()) {
        case 0: { data+="\r\n"; break; }
        case 1: { data+="\r" ; break; }
        case 2: { data+="\n"; break; }
        }
        mySerialPort->write(data);
        selPosition=1;
        QTimer::singleShot(50, this, SLOT(clearLine()));
    }
    else {
        if(ui->checkBox->isChecked() &&( arg1.left(3)=="+++" || arg1.left(3)=="&&&")){

            QByteArray data=arg1.toLocal8Bit();

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(0);
            textCharFormat.setFontWeight(QFont::Bold);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->insertPlainText(data+"\r\n");
            ui->plainTextEdit->moveCursor(QTextCursor::End);

            data+="\r\n";

            mySerialPort->write(data);
            ui->lineEdit->clear();
        }
    }
}

void Widget::on_pushButton_2_clicked()
{
    ui->pushButton_2->setEnabled(0);
    SerialPort::DataBits dataBits=static_cast<SerialPort::DataBits>(ui->comboBox_3->itemData((ui->comboBox_3->currentIndex())).toInt()); //currentText().toInt());
    SerialPort::Parity parity=static_cast<SerialPort::Parity>(ui->comboBox_5->itemData(ui->comboBox_5->currentIndex()).toInt());
    SerialPort::StopBits stopBits=static_cast<SerialPort::StopBits>(ui->comboBox_4->itemData(ui->comboBox_4->currentIndex()).toInt());
    SerialPort::FlowControl flowControl=static_cast<SerialPort::FlowControl>(ui->comboBox_6->itemData(ui->comboBox_6->currentIndex()).toInt());

        if(mySerialPort->setRate(ui->comboBox->currentText().toInt()) && mySerialPort->setParity(parity) && mySerialPort->setDataBits(dataBits) && mySerialPort->setStopBits(stopBits) && mySerialPort->setFlowControl(flowControl)){

            ui->plainTextEdit->moveCursor(QTextCursor::End);
            QTextCharFormat textCharFormat=ui->plainTextEdit->currentCharFormat();
            textCharFormat.setFontItalic(1);
            textCharFormat.setFontWeight(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->plainTextEdit->appendPlainText(tr("Changed!\r\n"));
            ui->plainTextEdit->moveCursor(QTextCursor::End);
            textCharFormat.setFontItalic(0);
            ui->plainTextEdit->setCurrentCharFormat(textCharFormat);
            ui->lineEdit->setFocus();
        }
        else {
            mySerialPort->close();
            QMessageBox::critical(this, tr("Error"),tr("Can't configure the serial port: %1,\nerror code: %2").arg(ui->comboBox_2->currentText()).arg(mySerialPort->error()));
            refreshAvSerialPort();
        }
}

//-----------------------------------------------------------------------------------------------------------
void Widget::changeIndxToPushButton(qint32 indx)
{
    if(ui->pushButton->text()=="Cl&ose"){
        ui->pushButton_2->setEnabled(1);
    }
}

//-----------------------------------------------------------------------------------------------------------
void Widget::focusLineEdit(qint32 indx)
{
    if(ui->lineEdit->isEnabled())
        ui->lineEdit->setFocus();
}

//-----------------------------------------------------------------------------------------------------------
void Widget::focusLineEdit2()
{
    if(ui->lineEdit->isEnabled())
        ui->lineEdit->setFocus();
}
//-----------------------------------------------------------------------------------------------------------


void Widget::onSokDisplayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(this, "Error", "The following error occurred: "+_sok->errorString());
    }
}

void Widget::onSokReadyRead()
{
    QDataStream in(_sok);
    //если считываем новый блок первые 2 байта это его размер
    if (_blockSize == 0) {
        //если пришло меньше 2 байт ждем пока будет 2 байта
        if (_sok->bytesAvailable() < (int)sizeof(quint16))
            return;
        //считываем размер (2 байта)
        in >> _blockSize;
        qDebug() << "_blockSize now " << _blockSize;
    }
    //ждем пока блок прийдет полностью
    if (_sok->bytesAvailable() < _blockSize)
        return;
    else
        //можно принимать новый блок
        _blockSize = 0;
    //3 байт - команда серверу
    quint8 command;
    in >> command;
    qDebug() << "Received command " << command;

    switch (command)
    {
        case MyClient::comAutchSuccess:
        {
            //ui->pbSend->setEnabled(true);
            //ui->lineEdit_3->setEnabled(true);
            //ui->lineEdit_3->setFocus();
            AddToLog("Enter as "+_name,0);
        }
        break;
        case MyClient::comUsersOnline:
        {
            AddToLog("Received user list "+_name,0);
//            ui->pbSend->setEnabled(true);
//            ui->lineEdit_3->setEnabled(true);
//            ui->lineEdit_3->setFocus();
            QString users;
            in >> users;
            if (users == "")
                return;
//            QStringList l =  users.split(",");
//            ui->lwUsers->addItems(l);
        }
        break;
        case MyClient::comPublicServerMessage:
        {
            QString message;
            in >> message;
            AddToLog("[PblcSrvrMssg]: "+message, 2);
        }
        break;
        case MyClient::comMessageToAll:
        {
            QString user;
            in >> user;
            QString message;
            in >> message;
            if(user==_name)
            AddToLog("["+user+"]: "+message,1);
            else
                AddToLog("["+user+"]: "+message,2);
        }
        break;
        case MyClient::comMessageToUsers:
        {
            QString user;
            in >> user;
            QString message;
            in >> message;
            if(user==_name)
            AddToLog("["+user+"]: "+message,1);
            else
                AddToLog("["+user+"]: "+message,2);
        }
        break;
        case MyClient::comPrivateServerMessage:
        {
            QString message;
            in >> message;
            AddToLog("[PrvtSrvrMssg]: "+message, 2);
        }
        break;
        case MyClient::comUserJoin:
        {
            QString name;
            in >> name;
           // ui->lwUsers->addItem(name);
            AddToLog(name+" joined", 0);
        }
        break;
        case MyClient::comUserLeft:
        {
            QString name;
            in >> name;
            /*for (int i = 0; i < ui->lwUsers->count(); ++i)
                if (ui->lwUsers->item(i)->text() == name)
                {
                    ui->lwUsers->takeItem(i);
                    AddToLog(name+" left", Qt::green);
                    break;
                }*/
        }
        break;
        case MyClient::comErrNameInvalid:
        {
            QMessageBox::information(this, "Error", "This name is invalid.");
            _sok->disconnectFromHost();
        }
        break;
        case MyClient::comErrNameUsed:
        {
            QMessageBox::information(this, "Error", "This name is already used.");
            _sok->disconnectFromHost();
        }
        break;
    }
}

void Widget::onSokConnected()
{
    //ui->pbConnect->setEnabled(false);
    //ui->pbDisconnect->setEnabled(true);
    ui->toolButton_5->setChecked(1);
    _blockSize = 0;
    AddToLog("Connected to"+_sok->peerAddress().toString()+":"+QString::number(_sok->peerPort()),0);

    //try autch
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << (quint8)MyClient::comAutchReq;

    _name = "Terminal";
    out << _name;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    _sok->write(block);
}

void Widget::onSokDisconnected()
{
   // ui->pbConnect->setEnabled(true);
   // ui->pbDisconnect->setEnabled(false);
    //ui->pbSend->setEnabled(false);
    ui->toolButton_5->setChecked(1);
//    ui->lineEdit_3->setEnabled(false);
  //  ui->lwUsers->clear();
    AddToLog("Disconnected from "+_sok->peerAddress().toString()+":"+QString::number(_sok->peerPort()), 0);
}

void Widget::AddToLog(QString text, qint32 tag)
{
    QTextCursor myCursor= ui->plainTextEdit->textCursor();
    myCursor.movePosition(QTextCursor::End);
    ui->plainTextEdit->setTextCursor(myCursor);

    QString str="", str2="";
    QTextCharFormat myFormat=ui->plainTextEdit->currentCharFormat();
    switch(tag)
    {
    case 0: {
        myFormat.setFontItalic(1); //setForeground(Qt::black ); cursive
        myFormat.setFontWeight(0);
        str="\r\n";
        str2="\r\n";
        break;
    }
    case 1: {
        myFormat.setFontItalic(0); //setForeground(Qt::black );  bold
        myFormat.setFontWeight(QFont::Bold);
        break;
    }
    case 2: {
        myFormat.setFontItalic(0); //setForeground(Qt::black );   norm
        myFormat.setFontWeight(0);
        str2="\r\n";
        break;
    }
    }
    ui->plainTextEdit->setCurrentCharFormat(myFormat);
    ui->plainTextEdit->insertPlainText(str+text+str2);
    myCursor= ui->plainTextEdit->textCursor();
    myCursor.movePosition(QTextCursor::End);
    ui->plainTextEdit->setTextCursor(myCursor);
    //ui->lwLog->item(0)->setTextColor(color);
}

void Widget::on_toolButton_5_clicked(bool checked)
{
    if(checked)
        _sok->disconnectFromHost();
    else
        _sok->connectToHost(ui->leHost->text(), ui->sbPort->value());
}
