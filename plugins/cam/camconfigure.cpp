#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>

#include <math.h>
#include "camconfigure.h"
#include "rs_polyline.h"


//using namespace std;
using namespace ClipperLib;

QString CamConfigure::name() const
{
    return QString(tr("CAM"));
}

PluginCapabilities CamConfigure::getCapabilities() const
{
    PluginCapabilities cap;
    cap.menuEntryPoints.append(PluginMenuLocation("plugins_menu",tr("CAM")));

    return cap;
}


void CamConfigure::execComm(Document_Interface *doc, QWidget *parent, QString cmd)
{
    Q_UNUSED(cmd);

    vector<QPointF> points;
    vector<Plug_VertexData> g_points;
    if(!selectEntity(doc,points,g_points,parent)) return;

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
        t_info = cam_dlg.getToolInfo();
        d_info = cam_dlg.getDepthInfo();

        delta = t_info.diameter * 0.5;
        lead_length = cam_dlg.getLeadLength();
        overcut = cam_dlg.getOverCut();

        direction = cam_dlg.getDirection();
        lead_type = cam_dlg.getLead();
        side = cam_dlg.getSide();

        //排序路径
        //sortPath(points,start_point,direction,side);
        sortGPath(g_points,start_point,direction,side);

        if(g_path.empty())
        {
            QMessageBox::information(parent,"Warning","g_path is empty!");
            return;
        }

        //tool_path = getOffset(path,delta * (static_cast<int>(side)) );


        //get leadin_point/leadout_point
        if(lead_type == CAM_INFO::LeadType::Normal)
        {
            Plug_VertexData pt1 = g_path[0];
            //判断g_path起始点是否为圆弧起点，从而计算leadin_point的正确位置位置
            if(pt1.bulge == 0)
            {
                getNormalPoint(leadin_point,path[0],path[1],lead_length,side,direction);
            }
            else
            {
                getNormalPoint1(leadin_point,path[path.size() - 1],path[path.size() - 2],lead_length,side,direction);
            }

            Plug_VertexData pt2 = g_path[g_path.size()-2];
            //判断g_path终点是否为圆弧起点，从而计算leadout_point的正确位置
            if(pt2.bulge == 0)
            {
                getNormalPoint1(leadout_point,path[path.size() - 1],path[path.size() - 2],lead_length,side,direction);
            }
            else
            {
                getNormalPoint(leadout_point,path[0],path[1],lead_length,side,direction);
            }
        }

        //添加起刀路径
        addCamPath();

        //路径标虚线
        setLineType(doc,DPI::LineType::DashLine2);

        //生成刀具图形
        //doc->addLines(tool_path,true);
        doc->addLines(path,false);

        //恢复实线
        //setLineType(doc,DPI::LineType::SolidLine);

        generateGCode(parent);
    }
}

void CamConfigure::generateGCode(QWidget* parent)
{
    //生成G代码
    QFile file;
    file.setFileName(QFileDialog::getSaveFileName(parent,QString(tr("Save File")),
                                                  QString("C:/Users/xcg/Desktop/libreCAD图纸"),QString("GCode Files (*.gcode)")));

    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    int cnt = 0;

    QTextStream stream(&file);

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G00 Z" << d_info.safe_z;
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G00 " << "X" << leadin_point.x() << " " << "Y" << leadin_point.y();
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "T" << t_info.tool_code << " " << "M06";
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "S" << t_info.spin_speed << " " << "M03";
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G00" << " " << "Z" << d_info.start_depth;
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G01" << " " << "Z" << (-1)*d_info.cut_depth << " F" << t_info.plunge_rate;
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "F" << t_info.feed_rate;
    stream << "\n";

    for(int i=0;i<g_path.size();i++)
    {
        Plug_VertexData pt = g_path[i];

        cnt += 10;
        stream << "N" << cnt << " ";
        if(i == 0)
        {
            //Left -> G41  Right -> G42 ; 顺时针 -> G02  逆时针 -> G03
            if(direction == CAM_INFO::DirectionType::Left)
            {
                stream << "G41 " << "X" << pt.point.x() << " " << "Y" << pt.point.y();
            }
            else
            {
                stream << "G42 " << "X" << pt.point.x() << " " << "Y" << pt.point.y();
            }
        }
        else
        {
            Plug_VertexData pt1 = g_path[i-1];

            //直线
            if(pt1.bulge == 0)
            {
                stream << "G01" << " " << "X" << pt.point.x() << " " << "Y" << pt.point.y();
            }
            else
            {
                double r;
                //如果此时是最后一个点，跳出循环
                /*if(i == g_path.size() - 1)
                {
                    r = polylineRadius(pt1,pt);
                }
                else
                {
                    r = polylineRadius(pt1,g_path[0]);
                }
                */
                r = polylineRadius(pt1,pt);

                if(pt.bulge > 0)//逆时针
                {
                    stream << "G03" << " ";
                }
                else
                {
                    stream << "G02" << " ";
                }

                stream << "X" << pt.point.x() << " " << "Y" << pt.point.y() << " ";
                stream << "R" << r;
            }

        }

        stream << "\n";
    }

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G40" << " " << "X" << leadout_point.x() << " " << "Y" << leadout_point.y();
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "Z" << d_info.start_depth;
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "G00 " << "Z" << d_info.safe_z;
    stream << "\n";

    cnt += 10;
    stream << "N" << cnt << " ";
    stream << "M30";
    stream << "\n";

    file.close();
    g_path.clear();

}

void CamConfigure::sortPath(vector<QPointF>& points,
                            QPointF& start_point,CAM_INFO::DirectionType direction,CAM_INFO::SideType side)
{
    //规定绘图顺序都按照 逆时针，如果要求的刀具轨迹为顺时针，则反转路径
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

void CamConfigure::sortGPath(vector<Plug_VertexData>& g_points,QPointF& start_point,
                             CAM_INFO::DirectionType direction,CAM_INFO::SideType side)
{
    //规定绘图顺序都按照 逆时针，如果要求的刀具轨迹为顺时针，则反转路径
    //顺时针 -> bugle < 0 ; 逆时针 -> bugle > 0
    if((direction == CAM_INFO::DirectionType::Left && side == CAM_INFO::SideType::Outside)
            || (direction == CAM_INFO::DirectionType::Right && side == CAM_INFO::SideType::Inside))
    {
        for(int i = 0;i<g_points.size();i++)
        {
            Plug_VertexData& pt = g_points[i];
            if(pt.bulge > 0)
            {
                if(i < g_points.size()-1)
                {
                    //Plug_VertexData& next_pt = g_points[i+1];
                    g_points[i+1].bulge = (-1)*pt.bulge;
                }
                else
                {
                    //Plug_VertexData& next_pt = g_points.front();
                    g_points[0].bulge = (-1)*pt.bulge;
                }

                pt.bulge = 0.0;
            }
        }

        reverse(g_points.begin(),g_points.end());
    }
    else
    {
        for(int i = 0;i<g_points.size();i++)
        {
            Plug_VertexData& pt = g_points[i];
            if(pt.bulge < 0)
            {
                if(i < g_points.size()-1)
                {
                    //Plug_VertexData& next_pt = g_points[i+1];
                    g_points[i+1].bulge = (-1)*pt.bulge;
                }
                else
                {
                    //Plug_VertexData& next_pt = g_points.front();
                    g_points[0].bulge = (-1)*pt.bulge;
                }

                pt.bulge = 0.0;
            }
        }
    }

    auto it = find_if(g_points.begin(),g_points.end(),
                      [start_point](const Plug_VertexData& vertex)->bool
    {return vertex.point == start_point;});

    g_path.clear();
    copy(it,g_points.end(),back_inserter(g_path));
    copy(g_points.begin(),it,back_inserter(g_path));


    //如果为内侧且选择的起始点不在圆弧上，重新定位start_point
    if(side == CAM_INFO::SideType::Inside && g_path[0].bulge == 0)
    {
        QPointF pt1 = g_path[0].point;
        QPointF pt2 = g_path[1].point;

        QPointF pm;
        pm.setX((pt1.x()+pt2.x())/2);
        pm.setY((pt1.y()+pt2.y())/2);

        start_point = pm;
        //删除原来的起始点
        g_path.erase(g_path.begin());

        g_path.insert(g_path.begin(),Plug_VertexData(start_point,0.0));
        g_path.emplace_back(Plug_VertexData(pt1,0.0));
    }

    g_path.emplace_back(Plug_VertexData(start_point,0.0));

    //由g_path生成path
    path.clear();
    for(Plug_VertexData pt : g_path)
    {
        path.emplace_back(pt.point);
    }
}

bool CamConfigure::selectEntity(Document_Interface *doc,vector<QPointF>& points,
                                vector<Plug_VertexData>& g_points,QWidget* parent)
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
                points.emplace_back(vertexs.at(i).point);
                g_points.emplace_back(vertexs.at(i));
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

void CamConfigure::setLineType(Document_Interface* doc,DPI::LineType type)
{
    int color_val;
    DPI::LineWidth line_width;
    DPI::LineType line_type;

    //设置线型为DashLine2
    doc->getCurrentLayerProperties(&color_val,&line_width,&line_type);
    line_type = type;
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

void CamConfigure::addCamPath()
{
    QPointF homePoint(0.0,0.0);

    path.insert(path.begin(),homePoint);
    path.insert(path.begin()+1,leadin_point);

    path.push_back(leadout_point);
}

void CamConfigure::addGPath()
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
                                  CAM_INFO::SideType side,CAM_INFO::DirectionType direction)
{
    double x1 = point1.x();
    double y1 = point1.y();
    double x2 = point2.x();
    double y2 = point2.y();
    double x = point1.x(),y = point1.y();

    //如果是刀具在工件左边，则以point1->point2的向量逆时针旋转90度，再根据lead_length得出目标点point位置
    //否则，顺时针旋转90度计算出point的位置
    if(direction == CAM_INFO::DirectionType::Left)
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
    else
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

void CamConfigure::getNormalPoint1(QPointF& point,const QPointF& point1,const QPointF& point2,double l,
                                   CAM_INFO::SideType side,CAM_INFO::DirectionType direction)
{
    double x1 = point1.x();
    double y1 = point1.y();
    double x2 = point2.x();
    double y2 = point2.y();
    double x = point1.x(),y = point1.y();

    //如果是刀具在工件右边，则以point1->point2的向量逆时针旋转90度，再根据lead_length得出目标点point位置
    //否则，顺时针旋转90度计算出point的位置
    if(direction == CAM_INFO::DirectionType::Right)
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
    else
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

double CamConfigure::polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB)
{
    double dChord = sqrt( pow(ptA.point.x() - ptB.point.x(), 2) + pow(ptA.point.y() - ptB.point.y(), 2));

    return fabs( 0.5 * dChord / sin( 2.0 * atan(ptA.bulge)));
}


