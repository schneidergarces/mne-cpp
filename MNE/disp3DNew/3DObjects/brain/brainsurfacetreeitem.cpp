//=============================================================================================================
/**
* @file     brainsurfacetreeitem.cpp
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
* @brief    BrainSurfaceTreeItem class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "brainsurfacetreeitem.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DNEWLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

BrainSurfaceTreeItem::BrainSurfaceTreeItem(const int & iType, const QString & text)
: AbstractTreeItem(iType, text)
, m_pRenderable3DEntity(new Renderable3DEntity())
{
}


//*************************************************************************************************************

BrainSurfaceTreeItem::~BrainSurfaceTreeItem()
{
}


//*************************************************************************************************************

QVariant BrainSurfaceTreeItem::data(int role) const
{
    return QStandardItem::data(role);
}


//*************************************************************************************************************

void  BrainSurfaceTreeItem::setData(const QVariant & value, int role)
{
    QStandardItem::setData(value, role);
}


//*************************************************************************************************************

bool BrainSurfaceTreeItem::addData(const Surface & tSurface, Qt3DCore::QEntity * parent)
{
    //Create renderable 3D entity
    m_pRenderable3DEntity = new Renderable3DEntity(parent);

    //Create color from curvature information with default gyri and sulcus colors
    MatrixX3f matCurvatureColor = createCurvatureVertColor(tSurface.curv());

    //Set renderable 3D entity mesh and color data
    m_pRenderable3DEntity->setMeshData(tSurface.rr(), tSurface.nn(), tSurface.tris(), -tSurface.offset(), matCurvatureColor);

    //Add data which is held by this BrainSurfaceTreeItem
    QVariant data;

    data.setValue(matCurvatureColor);
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurrentColorVert);
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurvatureColorVert);

    data.setValue(tSurface.rr());
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceVert);

    data.setValue(tSurface.tris());
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceTris);

    data.setValue(tSurface.nn());
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceNorm);

    data.setValue(tSurface.curv());
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurv);

    data.setValue(tSurface.offset());
    this->setData(data, BrainSurfaceTreeItemRoles::SurfaceOffset);

    data.setValue(m_pRenderable3DEntity);
    this->setData(data, BrainSurfaceTreeItemRoles::Renderable3DEntity);

    //Add surface meta information as item children
    BrainTreeItem *itemSurfColorInfoOrigin = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceColorInfoOrigin, "Color origin");
    *this<<itemSurfColorInfoOrigin;
    data.setValue(QString("Color from curvature"));
    itemSurfColorInfoOrigin->setData(data, BrainTreeItemRoles::SurfaceColorInfoOrigin);
    itemSurfColorInfoOrigin->setData(data, Qt::DisplayRole);

    BrainTreeItem *itemSurfColSulci = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceColorSulci, "Sulci color");
    *this<<itemSurfColSulci;
    data.setValue(QColor(50,50,50));
    itemSurfColSulci->setData(data, BrainTreeItemRoles::SurfaceColorSulci);
    itemSurfColSulci->setData(data, Qt::DecorationRole);

    BrainTreeItem *itemSurfColGyri = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceColorGyri, "Gyri color");
    *this<<itemSurfColGyri;
    data.setValue(QColor(125,125,125));
    itemSurfColGyri->setData(data, BrainTreeItemRoles::SurfaceColorGyri);
    itemSurfColGyri->setData(data, Qt::DecorationRole);

    BrainTreeItem *itemSurfFileName = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceFileName, tSurface.fileName());
    *this<<itemSurfFileName;
    data.setValue(tSurface.fileName());
    itemSurfFileName->setData(data, BrainTreeItemRoles::SurfaceFileName);

    BrainTreeItem *itemSurfType = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceType, tSurface.surf());
    *this<<itemSurfType;
    data.setValue(tSurface.surf());
    itemSurfType->setData(data, BrainTreeItemRoles::SurfaceType);

    BrainTreeItem *itemSurfPath = new BrainTreeItem(BrainTreeModelItemTypes::SurfaceFilePath, tSurface.filePath());
    *this<<itemSurfPath;
    data.setValue(tSurface.filePath());
    itemSurfPath->setData(data, BrainTreeItemRoles::SurfaceFilePath);

    return true;
}


//*************************************************************************************************************

void BrainSurfaceTreeItem::updateVertColor()
{
    if(this->hasChildren()) {
        QString sColorInfoOrigin;
        QColor colorSulci, colorGyri;
        QVariant data;
        MatrixX3f matNewVertColor;

        for(int i = 0; i<this->rowCount(); i++) {
            if(this->child(i,0)->type() == BrainTreeModelItemTypes::SurfaceColorInfoOrigin) {
                sColorInfoOrigin = this->child(i,0)->data(BrainTreeItemRoles::SurfaceColorInfoOrigin).toString();
            }

            if(this->child(i,0)->type() == BrainTreeModelItemTypes::SurfaceColorSulci) {
                colorSulci = this->child(i,0)->data(BrainTreeItemRoles::SurfaceColorSulci).value<QColor>();
            }

            if(this->child(i,0)->type() == BrainTreeModelItemTypes::SurfaceColorGyri) {
                colorGyri = this->child(i,0)->data(BrainTreeItemRoles::SurfaceColorGyri).value<QColor>();
            }
        }

        if(sColorInfoOrigin == "Color from curvature") {
            //Create color from curvature information with default gyri and sulcus colors
            matNewVertColor = createCurvatureVertColor(this->data(BrainSurfaceTreeItemRoles::SurfaceCurv).value<VectorXf>(), colorSulci, colorGyri);

            data.setValue(matNewVertColor);
            this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurvatureColorVert);
        }

        if(sColorInfoOrigin == "Color from annotation") {
            //Create color from curvature information with default gyri and sulcus colors
            matNewVertColor = createCurvatureVertColor(this->data(BrainSurfaceTreeItemRoles::SurfaceCurv).value<VectorXf>(), colorSulci, colorGyri);

            data.setValue(matNewVertColor);
            this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurvatureColorVert);
        }

        //Set renderable 3D entity mesh and color data
        m_pRenderable3DEntity->setVertColor(matNewVertColor);
        this->setData(data, BrainSurfaceTreeItemRoles::SurfaceCurrentColorVert);
    }
}


//*************************************************************************************************************

MatrixX3f BrainSurfaceTreeItem::createCurvatureVertColor(const VectorXf & curvature, const QColor & colSulci, const QColor & colGyri)
{
    MatrixX3f matCurvatureColor(curvature.rows(), 3);

    for(int i = 0; i<curvature.rows() ; i++) {
        if(curvature[i] >= 0) {
            matCurvatureColor(i, 0) = colSulci.redF();
            matCurvatureColor(i, 1) = colSulci.greenF();
            matCurvatureColor(i, 2) = colSulci.blueF();
        } else {
            matCurvatureColor(i, 0) = colGyri.redF();
            matCurvatureColor(i, 1) = colGyri.greenF();
            matCurvatureColor(i, 2) = colGyri.blueF();
        }
    }

    return matCurvatureColor;
}
