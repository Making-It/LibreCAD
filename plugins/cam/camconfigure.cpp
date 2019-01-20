#include <QMessageBox>
#include <QDebug>

#include "camconfigure.h"
#include "rs_polyline.h"

#include <math.h>


//using namespace std;
using namespace ClipperLib;

QString CamConfigure::name() const
{
    return QString(tr("cam"));
}

PluginCapabilities CamConfigure::getCapabilities() const
{
    PluginCapabilities cap;
    cap.menuEntryPoints.append(PluginMenuLocation("plugins_menu",tr("cam")));

    return cap;

}


void CamConfigure::execComm(Document_Interface *doc, QWidget *parent, QString cmd)
{
    Q_UNUSED(cmd);

    vector<QPointF> points;
    if(!selectEntity(doc,points,parent)) return;

    QPointF start_point;
    //if(!selectStartPoint(doc,start_point)) return;

    int cnt = 0;
    bool cancel = false;
    while(!selectStartPoint(doc,start_point,points,parent,cancel))
    {
        ++cnt;
        if(cnt > 10) return;
        if(cancel) return;
    }


    CamDlg cam_dlg(parent);
    int res = cam_dlg.exec();

    if(res == QDialog::Accepted)
    {
        double delta = cam_dlg.getRadius();
        double lead_length = cam_dlg.getLeadLength();
        double overcut = cam_dlg.getOverCut();

        CAM_INFO::DirectionType direction = cam_dlg.getDirection();
        CAM_INFO::LeadType lead_type = cam_dlg.getLead();
        CAM_INFO::SideType side = cam_dlg.getSide();

        vector<QPointF> path;
        sortPath(points,path,start_point,direction,side);

        tool_path = getOffset(path,delta * (static_cast<int>(side)) );

        //路径标虚线
        setDot(doc);

        //添加起刀路径
        addCamPath(path,lead_length,overcut,lead_type);

        //get lead_point
        getNormalPoint(lead_point,tool_path[0],tool_path[1],lead_length,side);
        getNormalPoint(lead_point1,path[0],path[1],lead_length + delta,side);


        //生成刀具图形
        doc->addLines(tool_path,true);
        doc->addLine(&lead_point,&tool_path[0]);
        doc->addLine(&lead_point1,&path[0]);

        /*
        //恢复线型为SolidLine
        int color_val;
        DPI::LineWidth line_width;
        DPI::LineType line_type;

        doc->getCurrentLayerProperties(&color_val,&line_width,&line_type);
        line_type = DPI::LineType::SolidLine;
        doc->setCurrentLayerProperties(color_val,line_width,line_type);
        */
    }
}

void CamConfigure::sortPath(vector<QPointF>& points,vector<QPointF>& path,
              QPointF& start_point,CAM_INFO::DirectionType direction,CAM_INFO::SideType side)
{
    if((direction == CAM_INFO::DirectionType::Left && side == CAM_INFO::SideType::Outside)
            || (direction == CAM_INFO::DirectionType::Right && side == CAM_INFO::SideType::Inside))
    {
        reverse(points.begin(),points.end());
    }

    path.clear();

    auto it = find(points.begin(),points.end(),start_point);
    for(auto iter = it;iter != points.end();iter++)
    {
        path.emplace_back(*iter);
    }

    for(auto iter = points.begin();iter != it;iter++)
    {
        path.emplace_back(*iter);
    }

    path.emplace_back(start_point);

}

bool CamConfigure::selectEntity(Document_Interface *doc,vector<QPointF>& points,QWidget* parent)
{
    QList<Plug_Entity *> e_lt;
    bool flag = true;

    bool yes = doc->getSelect(&e_lt,QString("select a polyline!"));

    if(e_lt.empty() || ! yes) return false;

    if(e_lt.size() > 1)
    {
        QMessageBox::information(parent,"Error","The entity you selected more than one!");
        return false;
    }

    for(auto it=e_lt.begin();it!=e_lt.end();it++)
    {
        auto entity = *it;

        QHash<int,QVariant> data;
        entity->getData(&data);

        if(data.value(DPI::ETYPE) == DPI::POLYLINE){
            if(data.value(DPI::CLOSEPOLY).toInt() == 0)
            {
                QMessageBox::information(parent,"Error","The polyline selected is open!");
                continue;
            }

            QList<Plug_VertexData> vertexs;
            entity->getPolylineData(&vertexs);
            int n = vertexs.size();

            for(int i=0;i<n;i++){
                QPointF pt(vertexs.at(i).point.x(),vertexs.at(i).point.y());
                points.emplace_back(pt);
            }
        }
        else{
            flag = false;
            break;
        }
    }

    if(!flag){
        QMessageBox::information(parent,"Error","The graphic you selected is not a polyline!");
        return false;
    }

    if(points.empty()) return false;

    unique(points.begin(),points.end());

    return true;
}

bool CamConfigure::selectStartPoint(Document_Interface* doc,QPointF& point,
                                    const vector<QPointF>& points,QWidget* parent,bool& cancel)
{
    bool yes1 = doc->getPoint(&point,QString(tr("select tool start point on polyline!")));
    if(!yes1 || point.isNull())
    {
        //QMessageBox::information(parent,"Error","The start point didn't be selected!");
        cancel = true;
        return false;
    }

    if(points.end() == find(points.begin(),points.end(),point))
    {
        QMessageBox::information(parent,"Error",
                                 "The start point selected is not on polyline,please select agin!");
        return false;
    }

    QMessageBox::information(parent,"info","The start point selected is on polyline!");
    return true;
}

void CamConfigure::setDot(Document_Interface* doc)
{
    int color_val;
    DPI::LineWidth line_width;
    DPI::LineType line_type;

    //设置线型为DashLine2
    doc->getCurrentLayerProperties(&color_val,&line_width,&line_type);
    line_type = DPI::LineType::DashLine2;
    doc->setCurrentLayerProperties(color_val,line_width,line_type);

    //把原实体线型恢复为SolidLine
    /*
    QList<Plug_Entity*> e_lt1;
    bool ok = doc->getAllEntities(&e_lt1,false);

    if(!ok || e_lt1.empty()) return;

    for(auto entity : e_lt1)
    {
        QHash<int,QVariant> data;
        entity->getData(&data);

        if(data.value(DPI::ETYPE) == DPI::POLYLINE)
        {
            if(data.contains(DPI::LTYPE))
                data[DPI::LTYPE]= QString("SolidLine");
            else
                data.insert(DPI::LTYPE,QString("SolidLine"));

            entity->updateData(&data);
        }
        else
        {
            if(data.contains(DPI::LTYPE))
                data[DPI::LTYPE] = QString("DashLine2");
            else
                data.insert(DPI::LTYPE,QString("DashLine2"));

            entity->updateData(&data);
        }
    }
    */
}

void CamConfigure::addCamPath(vector<QPointF>& path,double dis,double overcut,
                              CAM_INFO::LeadType lead_type)
{



}


std::vector<QPointF> CamConfigure::getOffset(const vector<QPointF>& vec,double delta)
{
    vector<QPointF> vec1;
    Path subj;
    Paths solution;
    int off_val = delta;

    if(!vec.empty())
    {
        for(QPointF point : vec)
        {
            subj << IntPoint(point.x(),point.y());
        }

        ClipperOffset co;
        co.AddPath(subj,jtRound,etClosedPolygon);
        co.Execute(solution,off_val);
    }

    for(Path path : solution)
        for(IntPoint point : path)
        {
            vec1.emplace_back(QPointF(point.X,point.Y));
        }

    return vec1;

}

void CamConfigure::getNormalPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double l,
                                  CAM_INFO::SideType side)
{
    double x1 = point1.x();
    double y1 = point1.y();
    double x2 = point2.x();
    double y2 = point2.y();
    double x = point1.x(),y = point1.y();

    if(side == CAM_INFO::SideType::Outside)
    {
        x = (y2*((-l*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1))-2*x1*y1)
             + l*y1*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             +x1*y2*y2+x1*y1*y1+x1*x2*x2-2*x1*x1*x2+x1*x1*x1)
             /(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1);

        y = (l*x2*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             - l*x1*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             +y1*y2*y2-2*y1*y1*y2+y1*y1*y1+(x2*x2-2*x1*x2+x1*x1)*y1)
             /(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1);
    }
    else if(side == CAM_INFO::SideType::Inside)
    {
        x = (y2*((l*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1))-2*x1*y1)
             - l*y1*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             +x1*y2*y2+x1*y1*y1+x1*x2*x2-2*x1*x1*x2+x1*x1*x1)
             /(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1);

        y = (-l*x2*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             +l*x1*sqrt(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1)
             +y1*y2*y2-2*y1*y1*y2+y1*y1*y1+(x2*x2-2*x1*x2+x1*x1)*y1)
             /(y2*y2-2*y1*y2+y1*y1+x2*x2-2*x1*x2+x1*x1);
    }

    point.setX(x);
    point.setY(y);
}

void CamConfigure::getExtensionPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double dis)
{

}


