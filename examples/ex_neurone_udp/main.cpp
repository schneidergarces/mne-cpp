#include <iostream>

////inlcude below does not find QUdpSocket
//#include <QUdpSocket>

////from "https://stackoverflow.com/questions/31205059/qt-qudpsocket-no-such-file-or-directory"
////does nto work, inlcude below does not find QUdpSocket
//#include <QtNetwork/QUdpSocket>

//// include below finds QUdpSocket, but fails to find "QtNetwork/qtnetworkglobal.h"
#include <C:/Qt/5.9.1/msvc2015_64/include/QtNetwork/QUdpSocket>



using namespace std;

int main()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::192.168.200.220, 5000);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        processTheDatagram(datagram);

}


