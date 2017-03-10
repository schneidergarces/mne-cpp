//=============================================================================================================
/**
* @file     hemispheretreeitem.cpp
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     November, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch and Matti Hamalainen. All rights reserved.
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
* @brief    HemisphereTreeItem class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "hemispheretreeitem.h"
#include "../freesurfer/fssurfacetreeitem.h"
#include "../freesurfer/fsannotationtreeitem.h"
#include "../sourcespace/sourcespacetreeitem.h"
#include "../sourceactivity/mneestimatetreeitem.h"

#include <fs/label.h>
#include <fs/annotationset.h>
#include <fs/surfaceset.h>

#include <mne/mne_forwardsolution.h>
#include <mne/mne_sourceestimate.h>


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QList>
#include <QVariant>
#include <QStringList>
#include <QColor>
#include <QStandardItem>
#include <QStandardItemModel>


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace Eigen;
using namespace FSLIB;
using namespace MNELIB;
using namespace DISP3DLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

HemisphereTreeItem::HemisphereTreeItem(int iType, const QString& text)
: AbstractTreeItem(iType, text)
, m_pSurfaceItem(Q_NULLPTR)
, m_pAnnotItem(Q_NULLPTR)
{
    this->setEditable(false);    
    this->setCheckable(true);
    this->setCheckState(Qt::Checked);
    this->setToolTip("Brain hemisphere item");
}


//*************************************************************************************************************

HemisphereTreeItem::~HemisphereTreeItem()
{
}


//*************************************************************************************************************

QVariant HemisphereTreeItem::data(int role) const
{
    return AbstractTreeItem::data(role);
}


//*************************************************************************************************************

void HemisphereTreeItem::setData(const QVariant& value, int role)
{
    AbstractTreeItem::setData(value, role);
}


//*************************************************************************************************************

int HemisphereTreeItem::columnCount() const
{
    return 2;
}


//*************************************************************************************************************

FsSurfaceTreeItem* HemisphereTreeItem::addData(const Surface& tSurface, const Annotation& tAnnotation, Qt3DCore::QEntity* p3DEntityParent)
{
    //Set name of HemisphereTreeItem based on the hemisphere information
    switch (tSurface.hemi()) {
    case 0:
        this->setText("Left hemisphere");
        break;
    case 1:
        this->setText("Right hemisphere");
        break;
    default:
        this->setText("Unknown hemisphere");
        break;
    }

    QVariant data;
    data.setValue(tSurface.hemi());

    this->setData(data, Data3DTreeModelItemRoles::SurfaceHemi);

    //Add childs
    //Add surface child
    m_pSurfaceItem = new FsSurfaceTreeItem(Data3DTreeModelItemTypes::SurfaceItem);

    QList<QStandardItem*> list;
    list << m_pSurfaceItem;
    list << new QStandardItem(m_pSurfaceItem->toolTip());
    this->appendRow(list);

    m_pSurfaceItem->addData(tSurface, p3DEntityParent);

    //Add annotation child
    if(!tAnnotation.isEmpty()) {
        m_pAnnotItem = new FsAnnotationTreeItem(Data3DTreeModelItemTypes::AnnotationItem);
        connect(m_pAnnotItem, &FsAnnotationTreeItem::annotationVisibiltyChanged,
                m_pSurfaceItem, &FsSurfaceTreeItem::onAnnotationVisibilityChanged);

        list.clear();
        list << m_pAnnotItem;
        list << new QStandardItem(m_pAnnotItem->toolTip());
        this->appendRow(list);

        m_pAnnotItem->addData(tSurface, tAnnotation);
    }

    return m_pSurfaceItem;
}


//*************************************************************************************************************

SourceSpaceTreeItem* HemisphereTreeItem::addData(const MNEHemisphere& tHemisphere, Qt3DCore::QEntity* p3DEntityParent)
{
    //Set name of HemisphereTreeItem based on the hemisphere information
    QVariant data;

    switch (tHemisphere.id) {
        case 101:
            this->setText("Left hemisphere");
            data.setValue(0);
            break;
        case 102:
            this->setText("Right hemisphere");
            data.setValue(1);
            break;
        default:
            this->setText("Unknown hemisphere");
            data.setValue(-1);
            break;
    }

    this->setData(data, Data3DTreeModelItemRoles::SurfaceHemi);

    //Add childs
    //Add surface child
    SourceSpaceTreeItem* pSourceSpaceItem = new SourceSpaceTreeItem(Data3DTreeModelItemTypes::SourceSpaceItem);

    QList<QStandardItem*> list;
    list << pSourceSpaceItem;
    list << new QStandardItem(pSourceSpaceItem->toolTip());
    this->appendRow(list);

    pSourceSpaceItem->addData(tHemisphere, p3DEntityParent);

    return pSourceSpaceItem;
}


//*************************************************************************************************************

void HemisphereTreeItem::setRtVertColor(const MatrixX3f& sourceColorSamples)
{
    if(m_pSurfaceItem) {
        m_pSurfaceItem->setRtVertColor(sourceColorSamples);
    }
}


//*************************************************************************************************************

FsSurfaceTreeItem* HemisphereTreeItem::getSurfaceItem()
{
   return m_pSurfaceItem;
}


//*************************************************************************************************************

FsAnnotationTreeItem* HemisphereTreeItem::getAnnotItem()
{
    return m_pAnnotItem;
}


//*************************************************************************************************************

void HemisphereTreeItem::onCheckStateChanged(const Qt::CheckState& checkState)
{
    for(int i = 0; i < this->rowCount(); i++) {
        if(this->child(i)->isCheckable()) {
            this->child(i)->setCheckState(checkState);
        }
    }
}

