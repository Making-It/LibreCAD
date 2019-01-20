/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
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

#include "rs_actiondrawpoint.h"

#include <QAction>
#include <QMouseEvent>
#include <QDebug>
#include <algorithm>
#include "rs_dialogfactory.h"
#include "rs_graphicview.h"
#include "rs_commandevent.h"
#include "rs_point.h"
#include "rs_coordinateevent.h"
#include "rs_polyline.h"
#include "rs_line.h"
#include "rs_arc.h"
#include "rs_atomicentity.h"

using namespace std;


RS_ActionDrawPoint::RS_ActionDrawPoint(RS_EntityContainer& container,
                                       RS_GraphicView& graphicView)
        :RS_PreviewActionInterface("Draw Points",
						   container, graphicView)
		, pt(new RS_Vector{})
{
	actionType=RS2::ActionDrawPoint;
}

RS_ActionDrawPoint::~RS_ActionDrawPoint() = default;


void RS_ActionDrawPoint::trigger() {
	if (pt->valid) {
		RS_Point* point = new RS_Point(container, RS_PointData(*pt));
        container->addEntity(point);

        if (document) {
            document->startUndoCycle();
            document->addUndoable(point);
            document->endUndoCycle();
        }

		graphicView->moveRelativeZero(*pt);
		graphicView->redraw((RS2::RedrawMethod) (RS2::RedrawDrawing | RS2::RedrawOverlay));
    }
}



void RS_ActionDrawPoint::mouseMoveEvent(QMouseEvent* e) {
    snapPoint(e);
}



void RS_ActionDrawPoint::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button()==Qt::LeftButton) {
        RS_CoordinateEvent ce(snapPoint(e));
        coordinateEvent(&ce);
    } else if (e->button()==Qt::RightButton) {
        init(getStatus()-1);
    }
}



void RS_ActionDrawPoint::coordinateEvent(RS_CoordinateEvent* e) {
	if (e==nullptr) {
        return;
    }

    RS_Vector mouse = e->getCoordinate();

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
            else if(type == RS2::EntityLine)
            {
                getLineData(&points,entity);
            }
        }
    }

    unique(points.begin(),points.end());

    if(isNear(mouse,points))
        qDebug() << "find near point!!!";

    /*******xcg add 20190109*******/


	*pt = mouse;
    trigger();
}





void RS_ActionDrawPoint::commandEvent(RS_CommandEvent* e) {
    QString c = e->getCommand().toLower();

	if (checkCommand("help", c)) {
		RS_DIALOGFACTORY->commandMessage(msgAvailableCommands()
										 + getAvailableCommands().join(", "));
		return;
	}
}



QStringList RS_ActionDrawPoint::getAvailableCommands() {
	return {};
}


void RS_ActionDrawPoint::updateMouseButtonHints() {
	switch (getStatus()) {
	case 0:
		RS_DIALOGFACTORY->updateMouseWidget(tr("Specify location"), tr("Cancel"));
		break;
	default:
		RS_DIALOGFACTORY->updateMouseWidget();
		break;
	}
}



void RS_ActionDrawPoint::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}

// EOF
//xcg add 20190109
/*
QList<RS_Entity> RS_ActionDrawPoint::getEntity()
{
    QList<RS_Entity> lt;
    for(auto e : *container)
    {
        if(e->isSelected())
        {
            lt.append(e);
        }
    }

    return lt;
}*/

/************xcg add 20190109 ***********/
void RS_ActionDrawPoint::getPolyLineData(QList<RS_PointData*> *data,RS_Entity* entity)
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
bool RS_ActionDrawPoint::isNear(RS_Vector& point,const QList<RS_PointData *>& points)
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

/************xcg add 20190110 ***********/
void RS_ActionDrawPoint::getLineData(QList<RS_PointData *> *data,RS_Entity* entity)
{
    if(!entity) return;
    RS2::EntityType type = entity->rtti();
    if(type != RS2::EntityLine) return;

    RS_Line* line = static_cast<RS_Line*>(entity);
    data->append(new RS_PointData(RS_Vector(line->getStartpoint().x,line->getStartpoint().y)));
    data->append(new RS_PointData(RS_Vector(line->getEndpoint().x,line->getEndpoint().y)));

}

/************xcg add 20190110 ***********/
