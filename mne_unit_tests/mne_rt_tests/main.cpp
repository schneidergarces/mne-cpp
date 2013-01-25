//=============================================================================================================
/**
* @file     main.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2012
*
* @section  LICENSE
*
* Copyright (C) 2012, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Implements the main() application function.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "dataparsertest.h"
#include <rtCommand/commandmanager.h>


//*************************************************************************************************************
//=============================================================================================================
// STL INCLUDES
//=============================================================================================================

#include <stdio.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtCore/QCoreApplication>
#include <QVariant>
#include <QMetaType>
#include <QDebug>
#include <QObject>
#include <QJsonObject>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

//using namespace MNEUNITTESTS;
using namespace RTCOMMANDLIB;


//*************************************************************************************************************
//=============================================================================================================
// Methods
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// MAIN
//=============================================================================================================

//=============================================================================================================
/**
* The function main marks the entry point of the program.
* By default, main has the storage class extern.
*
* @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
* @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
* @return the value that was set to exit() (which is 0 if exit() is called via quit()).
*/
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataParserTest testParser;

    QVariant test(QVariant::String);
    qDebug() << test.typeName();

    QString jsonTestCommand =
                    "{"
                    "   \"encoding\": \"UTF-8\","
                    "   \"device\": \"Neuromag\","
                    "   \"description\": \"Vector View\","
                    "   \"commands\": {"
                    "       \"com1\": {"
                    "           \"description\": \"available commands\","
                    "           \"parameters\": {"
                    "               \"id\": {"
                    "                   \"description\": \"id descr\","
                    "                   \"type\": \"QString\" "
                    "               },"
                    "               \"id2\": {"
                    "                   \"description\": \"id2 descr\","
                    "                   \"type\": \"int\" "
                    "               },"
                    "               \"id3\": {"
                    "                   \"description\": \"id3 descr\","
                    "                   \"type\": \"bool\" "
                    "               }"
                    "           }"
                    "       },"
                    "       \"com2\": {"
                    "           \"description\": \"starts the measurement\","
                    "           \"parameters\": {}"
                    "       }"
                    "    }"
                    "}";

    CommandManager t_comManager2(QString("test2"));

    CommandManager t_comManager(jsonTestCommand.toLatin1(), "test1");

    qDebug() << "Contains help? " << t_comManager.hasCommand(QString("help"));
    qDebug() << "Contains test? " << t_comManager.hasCommand(QString("test"));
    qDebug() << "Contains meas? " << t_comManager.hasCommand(QString("meas"));


    qDebug() << t_comManager["help"].toJsonObject();
//    qDebug() << t_comManager.toJsonObject();

    qDebug() << "HELP " << t_comManager["help"]["id"];

    qDebug() << "HELP " << t_comManager["help"]["id2"];

    qDebug() << "Even comManager is not copied. Does it still contains help? " << t_comManager2.hasCommand(QString("help")) << " GOOD :-) it's still there.";

    qDebug() << t_comManager2["help"].toStringList();

    qDebug() << t_comManager2["com1"].toStringList();


    qDebug() << t_comManager2.toString();


    t_comManager2.parse(QString("help"));

//    t_comManager2.insertJsonCommands();


//    qDebug() << t_comManager.m_jsonDocumentOrigin.object().value(QString("commands")).toObject();

//    qDebug() << t_comManager.m_jsonDocumentOrigin.toJson();

    return a.exec();
}
