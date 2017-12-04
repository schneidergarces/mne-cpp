//=============================================================================================================
/**
* @file     server.h
* @author   Ricky Tjen <ricky270@student.sgu.ac.id>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
* @version  1.0
* @date     April, 2016
*
* @section  LICENSE
*
* Copyright (C) 2016, Ricky Tjen and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Bar class declaration
*
*/

#ifndef SERVER_H
#define SERVER_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QObject>
#include <QDebug>
#include <QDataStream>


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================


//=============================================================================================================
/**
* Histogram display using Qtcharts, similar to matlab bar graph
*
* @brief Bar class for histogram display using QtCharts
*/
class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject* parent = Q_NULLPTR)
        :QObject(parent)
    {}

    void initSocket()
    {
        m_pUdpSocket = new QUdpSocket(this);
        m_pUdpSocket->bind(QHostAddress::AnyIPv4, 50000);

        connect(m_pUdpSocket, &QUdpSocket::readyRead,
                this, &Server::readPendingDatagrams);
    }

    void readPendingDatagrams()
    {
        while (m_pUdpSocket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = m_pUdpSocket->receiveDatagram();
            qDebug() << "Datagram on port 50000 from IP "<<datagram.senderAddress();
            processDatagram(datagram);
        }
    }

    void processDatagram(QNetworkDatagram datagram)
    {
        QByteArray data = datagram.data();

        qDebug()<<"Data size "<<data.size();



        QDataStream reader(data);

        char* raw = new char(2);
        int length = 2;
        reader.readRawData(raw, length);
        qDebug()<<"First Block uint8 "<<(quint8)raw;
        qDebug()<<"First Block  "<< raw;


//        qDebug()<<"Block int8 0"<<testVal;
//        QDataStream::readBytes(data,1) >> testVal;
//        qDebug()<<"Block int8 1"<<testVal;
//        QDataStream::readBytes(data,1) >> testVal;
//        qDebug()<<"Block int8 2"<<testVal;
//        QDataStream::readBytes(data,1) >> testVal;
//        qDebug()<<"Block int8 3"<<testVal;
//        qint32 testValA;
//        QDataStream::readBytes(data.data(),4) >> testValA;
//        qDebug()<<"Block int32 0"<<testValA;


        //qDebug()<<"testVal"<<testVal;

        //Eigen::MatrixXd matData = Eigen::MatrixXd(numberSensors, numberSamples);


    }

protected:
    QUdpSocket* m_pUdpSocket;
};

// NAMESPACE

#endif // SERVER_H
