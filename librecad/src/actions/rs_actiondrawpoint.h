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

#ifndef RS_ACTIONDRAWPOINT_H
#define RS_ACTIONDRAWPOINT_H

#include "rs_previewactioninterface.h"
#include "rs_point.h"
#include "rs_document.h"

#define MIN_DIS 1.0 //xcg add 20190109
/**
 * This action class can handle user events to draw points.
 *
 * @author Andrew Mustun
 */
class RS_ActionDrawPoint : public RS_PreviewActionInterface {
	Q_OBJECT
public:
    RS_ActionDrawPoint(RS_EntityContainer& container,
                       RS_GraphicView& graphicView);
	~RS_ActionDrawPoint() override;

    //QList<RS_Entity> getEntity();//xcg add 20190109
    void getPolyLineData(QList<RS_PointData*> *data,RS_Entity* entity);//xcg add 20190109
    void getLineData(QList<RS_PointData *> *data,RS_Entity* entity);//xcg add 20190110
    bool isNear(RS_Vector& point,const QList<RS_PointData *>& points);//xcg add 20190109
	void trigger() override;

	void mouseMoveEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	
	void coordinateEvent(RS_CoordinateEvent* e) override;
	void commandEvent(RS_CommandEvent* e) override;
		QStringList getAvailableCommands() override;

	void updateMouseButtonHints() override;
	void updateMouseCursor() override;

private:
	std::unique_ptr<RS_Vector> pt;
};

#endif
