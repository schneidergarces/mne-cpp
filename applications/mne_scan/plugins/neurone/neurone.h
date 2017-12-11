//=============================================================================================================
/**
* @file     neurone.h
* @author   Lorenz Esch <lorenz.esch@tu-ilmenau.de>;
*           Nils Schneider <schneidergarces@gmail.com>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
* @version  1.0
* @date     December, 2017
*
* @section  LICENSE
*
* Copyright (C) 2017, Lorenz Esch, Nils Schneider and Matti Hamalainen. All rights reserved.
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
* @brief    Contains the declaration of the neurone class.
*
*/

#ifndef NEURONE_H
#define NEURONE_H


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "neurone_global.h"

// ?? fstream and buffer needed?
#include <fstream>
#include <scShared/Interfaces/ISensor.h>
#include <utils/generics/circularmatrixbuffer.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

// ?? needed?
#include <QFile>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QObject>
#include <QDebug>
#include <QDataStream>


//*************************************************************************************************************
//=============================================================================================================
// EIGEN INCLUDES
//=============================================================================================================

// ?? more needed?
#include <unsupported/Eigen/FFT>
#include <Eigen/Geometry>


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS  // ?? needed?
//=============================================================================================================

namespace SCMEASLIB {
    class NewRealTimeMultiSampleArray;
}

namespace FIFFLIB {
    class FiffStream;
    class FiffInfo;
}


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE neuronePLUGIN  // ?? needed?
//=============================================================================================================

namespace NEURONEPLUGIN
{


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS  // ?? none needed for now?
//=============================================================================================================

//class neuroneProducer;
//class neuroneSetupWidget;
//class neuroneSetupProjectWidget;


//=============================================================================================================
/**
* neurone...
*
* @brief The neurone class provides a EEG connector for receiving data from NeurOne UDP digital out.
*/


class NEURONESHARED_EXPORT NeurOne : public SCSHAREDLIB::ISensor
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "scsharedlib/1.0" FILE "neurone.json") //New Qt5 Plugin system replaces Q_EXPORT_PLUGIN2 macro
    // Use the Q_INTERFACES() macro to tell Qt's meta-object system about the interfaces
    Q_INTERFACES(SCSHAREDLIB::ISensor)

public:




    //=========================================================================================================
    /**
    * Constructs a NeurOne.
    */
    NeurOne();

    //=========================================================================================================
    /**
    * Destroys the NeurOne.
    */
    virtual ~NeurOne();

    //=========================================================================================================
    /**
    * Clone the plugin
    */
    virtual QSharedPointer<SCSHAREDLIB::IPlugin> clone() const;

    //=========================================================================================================
    /**
    * Initialise input and output connectors.
    */
    virtual void init();

    //=========================================================================================================
    /**
    * Is called when plugin is detached of the stage. Can be used to safe settings.
    */
    virtual void unload();

    //=========================================================================================================
    /**
    * Sets up the fiff info with the current data chosen by the user.
    */
    void setUpFiffInfo();

    //=========================================================================================================
    /**
    * Starts the NeurOne by starting the tmsi's thread.
    */
    virtual bool start();

    //=========================================================================================================
    /**
    * Stops the NeurOne by stopping the tmsi's thread.
    */
    virtual bool stop();

    //=========================================================================================================
    /**
    * Set/Add received samples to a QList.
    */
    void setSampleData(Eigen::MatrixXd &matRawBuffer);

    virtual IPlugin::PluginType getType() const;
    virtual QString getName() const;

    virtual QWidget* setupWidget();

    //======== from ex_neurone Server:

    void initSocket()
    {
        m_pUdpSocket = new QUdpSocket(this);
        m_pUdpSocket->bind(QHostAddress::AnyIPv4, 50000);

        connect(m_pUdpSocket, &QUdpSocket::readyRead,
                this, &NeurOne::readPendingDatagrams);
    }

    void readPendingDatagrams()
    {
        while (m_pUdpSocket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = m_pUdpSocket->receiveDatagram();
            qDebug() << "Datagram on port 50000 from IP "<<datagram.senderAddress();
            processDatagram(datagram);
        }
    }

    void processDatagram(const QNetworkDatagram &datagram)
    {


        QByteArray data = datagram.data();
        QDataStream stream(data);

        quint8 iSamplePacketIdentifier, iUnitIndication, iNotInUse;
        quint32 iPacketSequencyNumber;
        quint16 iNumberChannels, iNumberSamplesInBundle;
        quint64 iSampleIndex, iTimeStamp;

        Eigen::MatrixXi matData;

        stream >> iSamplePacketIdentifier;
        stream >> iUnitIndication;
        stream >> iNotInUse;
        stream >> iNotInUse;
        stream >> iPacketSequencyNumber;
        stream >> iNumberChannels;
        stream >> iNumberSamplesInBundle;
        stream >> iSampleIndex;
        stream >> iTimeStamp;

        //Get data
        matData.resize(iNumberChannels, iNumberSamplesInBundle);

        for(int j = 0; j < iNumberSamplesInBundle; ++j) {
            for(int i = 0; i < iNumberChannels; ++i) {
                unsigned char* pData = new unsigned char[3];
                stream.readRawData((char*)pData, 3);
//                qDebug() << (quint8)pData[0];
//                qDebug() << (quint8)pData[1];
//                qDebug() << (quint8)pData[2];
//                qDebug()<<"";
                matData(i,j) = sample24Bit(pData);
                delete pData;
            }
        }

//        qDebug() << "iSamplePacketIdentifier " << iSamplePacketIdentifier;
//        qDebug() << "iUnitIndication " << iUnitIndication;
//        qDebug() << "iPacketSequencyNumber " << iPacketSequencyNumber;
//        qDebug() << "iNumberChannels " << iNumberChannels;
//        qDebug() << "iNumberSamplesInBundle " << iNumberSamplesInBundle;
//        qDebug() << "iSampleIndex " << iSampleIndex;
//        qDebug() << "iTimeStamp " << iTimeStamp;

//        std::cout << matData << std::endl;
    }

    int sample24Bit(unsigned char *pData) {
        int result = (
            pData[0] << 16 |
            pData[1] << 8 |
            pData[2]
        );

        if (result > 8388608) // 2^24/2
        {
            result = result - 16777216; // 2^24
        }

        return result;
    }

protected:
    //=========================================================================================================
    /**
    * The starting point for the thread. After calling start(), the newly created thread calls this function.
    * Returning from this method will end the execution of the thread.
    * Pure virtual method inherited by QThread.
    */
    virtual void run();

    //======== from ex_neurone Server:
    QUdpSocket* m_pUdpSocket;
};


} // NAMESPACE

#endif // NEURONE_H
