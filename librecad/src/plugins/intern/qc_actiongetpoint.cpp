/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2011 Rallaz (rallazz@gmail.com)
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
**
**
** This file is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
**********************************************************************/

#include "qc_actiongetpoint.h"

#include <QPointF>
#include <QMouseEvent>
#include <QDebug>
#include "rs_snapper.h"
#include "rs_dialogfactory.h"
#include "rs_graphicview.h"
#include "rs_line.h"
#include "rs_coordinateevent.h"
#include "rs_preview.h"
#include "rs_debug.h"
#include "rs_polyline.h"
#include "rs_atomicentity.h"
#include "rs_arc.h"


struct QC_ActionGetPoint::Points {
		RS_MoveData data;
		RS_Vector referencePoint;
		RS_Vector targetPoint;
		QString mesage;
};

QC_ActionGetPoint::QC_ActionGetPoint(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_PreviewActionInterface("Get Point",
						   container, graphicView)
        , canceled(false)
		, completed{false}
		, setTargetPoint{false}
		, pPoints(new Points{})
{
    pPoints->targetPoint = RS_Vector(0,0);
}

QC_ActionGetPoint::~QC_ActionGetPoint() = default;


void QC_ActionGetPoint::trigger() {

    RS_DEBUG->print("QC_ActionGetPoint::trigger()");
    completed = true;
    updateMouseButtonHints();
}


void QC_ActionGetPoint::mouseMoveEvent(QMouseEvent* e) {
    RS_DEBUG->print("QC_ActionGetPoint::mouseMoveEvent begin");

        RS_Vector mouse = snapPoint(e);
        if(setTargetPoint){
			if (pPoints->referencePoint.valid) {
				pPoints->targetPoint = mouse;
                deletePreview();
				RS_Line *line =new RS_Line{preview.get(),
						pPoints->referencePoint, mouse};
                line->setPen(RS_Pen(RS_Color(0,0,0), RS2::Width00, RS2::DotLine ));
                preview->addEntity(line);
                RS_DEBUG->print("QC_ActionGetPoint::mouseMoveEvent: draw preview");
                drawPreview();
                preview->addSelectionFrom(*container);
            }
        } else {
			pPoints->targetPoint = mouse;
        }

    RS_DEBUG->print("QC_ActionGetPoint::mouseMoveEvent end");
}



void QC_ActionGetPoint::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button()==Qt::LeftButton) {
        RS_CoordinateEvent ce(snapPoint(e));
        coordinateEvent(&ce);
    } else if (e->button()==Qt::RightButton) {
        canceled = true;
        completed = true;
        finish();
    }
}

void QC_ActionGetPoint::coordinateEvent(RS_CoordinateEvent* e) {

	if (e==nullptr) return;

    RS_Vector pos = e->getCoordinate();

    /*******xcg add 20190109*******/
    //QList<RS_Entity> lt = getEntity();
    QList<RS_Entity*> lt = container->getEntityList();
    QList<RS_PointData* > points;
    if(!lt.empty())
    {
        for(int i=0;i<lt.size();i++)
        {
            auto entity = lt[i];
            RS2::EntityType type = entity->rtti();

            if(type == RS2::EntityPolyline)
            {
                getPolyLineData(&points,entity);
            }
        }

    }

    if(isNear(pos,points))
        qDebug() << "find near point!!!";
    /*******xcg add 20190109*******/

		pPoints->targetPoint = pos;
		graphicView->moveRelativeZero(pPoints->targetPoint);
        trigger();
}


void QC_ActionGetPoint::updateMouseButtonHints() {
    if (!completed)
		RS_DIALOGFACTORY->updateMouseWidget(pPoints->mesage, tr("Cancel"));
    else
        RS_DIALOGFACTORY->updateMouseWidget();
}


void QC_ActionGetPoint::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}


void QC_ActionGetPoint::setBasepoint(QPointF* basepoint){
	pPoints->referencePoint.x = basepoint->x();
	pPoints->referencePoint.y = basepoint->y();
    setTargetPoint = true;
}


void QC_ActionGetPoint::setMesage(QString msg){
	pPoints->mesage = msg;
}


void QC_ActionGetPoint::getPoint(QPointF *point)
{
    if (pPoints)
    {
        point->setX(pPoints->targetPoint.x);
        point->setY(pPoints->targetPoint.y);
    }

}

/************xcg add 20190109 ***********/
void QC_ActionGetPoint::getPolyLineData(QList<RS_PointData*> *data,RS_Entity* entity)
{
    if (!entity) return;
    RS2::EntityType et = entity->rtti();
    if (et != RS2::EntityPolyline) return;
    RS_Polyline *l = static_cast<RS_Polyline*>(entity);

    RS_Entity* nextEntity = 0;
    RS_AtomicEntity* ae = nullptr;
    RS_Entity* v = l->firstEntity(RS2::ResolveNone);
    double bulge=0.0;
//bad polyline without vertex
    if (!v) return;

//First polyline vertex
    if (v->rtti() == RS2::EntityArc) {
        bulge = ((RS_Arc*)v)->getBulge();
    }
    ae = (RS_AtomicEntity*)v;
    data->append(new RS_PointData(RS_Vector(ae->getStartpoint().x,ae->getStartpoint().y)));

    for (v=l->firstEntity(RS2::ResolveNone); v; v=nextEntity) {
        nextEntity = l->nextEntity(RS2::ResolveNone);
        bulge = 0.0;
        if (!v->isAtomic()) {
            continue;
        }
        ae = (RS_AtomicEntity*)v;

        /*
        if (nextEntity) {
            if (nextEntity->rtti()==RS2::EntityArc) {
                bulge = ((RS_Arc*)nextEntity)->getBulge();
            }
        }
       */
        if (!l->isClosed() || nextEntity) {
            data->append(new RS_PointData(RS_Vector(ae->getEndpoint().x,
                                         ae->getEndpoint().y)));
        }
    }
}
/************xcg add 20190109 ***********/


/************xcg add 20190109 ***********/
bool QC_ActionGetPoint::isNear(RS_Vector& point,const QList<RS_PointData *>& points)
{
    if(points.empty()) return false;

    for(auto pt : points)
    {
        double x = pt->pos.x;
        double y = pt->pos.y;
        double dis = sqrt((x - point.x)*(x - point.x) + (y - point.y)*(y - point.y));

        if(dis <= MIN_DIS)
        {
            point.x = x;
            point.y = y;
            return true;
        }
    }

    return false;

}
/************xcg add 20190109 ***********/
// EOF
