#ifndef CAMCONFIGURE_H
#define CAMCONFIGURE_H

#include <QObject>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include "clipper.hpp"
#include "qc_plugininterface.h"
#include "document_interface.h"
#include "doc_plugin_interface.h"
#include "camdlg.h"


using namespace std;

//class Plug_Entity;

class CamConfigure : public QObject,QC_PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "cam.json")
    Q_INTERFACES(QC_PluginInterface)


public:
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc, QWidget *parent, QString cmd) Q_DECL_OVERRIDE;
    vector<QPointF> getOffset(const vector<QPointF>& vec,double delta);

    void setLineType(Document_Interface* doc,DPI::LineType type);
    bool selectStartPoint(Document_Interface* doc,QPointF& point,
                          const vector<QPointF>& points,QWidget* parent,bool& cancel);
    bool selectEntity(Document_Interface* doc,vector<QPointF>& points,
                      vector<Plug_VertexData>& g_points,QWidget* parent);

    void sortPath(vector<QPointF>& points,QPointF& start_point,
                  CAM_INFO::DirectionType direction,CAM_INFO::SideType side);

    void sortGPath(vector<Plug_VertexData>& g_points,QPointF& start_point,
                   CAM_INFO::DirectionType direction,CAM_INFO::SideType side);

    void addCamPath();
    void addGPath();
    void generateGCode(QWidget* parent);
    void getNormalPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double dis,
                        CAM_INFO::DirectionType direction);
    void getNormalPoint1(QPointF& point,const QPointF& point1,const QPointF& point2,double dis,
                        CAM_INFO::DirectionType direction);
    void getExtensionPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double dis);

    double polylineRadius( const Plug_VertexData& ptA, const Plug_VertexData& ptB);
private:
    vector<Plug_VertexData> g_path;
    vector<QPointF> tool_path;
    vector<QPointF> path;//path中存放排序后的路径
    //map<QPointF,double> cir_mp;
    QPointF leadin_point,leadout_point;

    Tool_Info t_info;
    Depth_Info d_info;
    double delta,lead_length,overcut;
    CAM_INFO::DirectionType direction;
    CAM_INFO::LeadType lead_type;
    CAM_INFO::SideType side;

};

#endif // CAMCONFIGURE_H
