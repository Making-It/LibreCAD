#ifndef CAMCONFIGURE_H
#define CAMCONFIGURE_H

#include <QObject>
#include <vector>
#include <algorithm>

#include "clipper.hpp"
#include "qc_plugininterface.h"
#include "document_interface.h"
#include "doc_plugin_interface.h"
#include "camdlg.h"


using namespace std;

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

    void setDot(Document_Interface* doc);
    bool selectStartPoint(Document_Interface* doc,QPointF& point,
                          const vector<QPointF>& points,QWidget* parent,bool& cancel);
    bool selectEntity(Document_Interface* doc,vector<QPointF>& points,QWidget* parent);
    void sortPath(vector<QPointF>& points,vector<QPointF>& path,
                  QPointF& start_point,CAM_INFO::DirectionType direction,CAM_INFO::SideType side);

    void addCamPath(vector<QPointF>& path,double dis,double overcut,CAM_INFO::LeadType lead_type);
    void getNormalPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double dis,
                        CAM_INFO::SideType side);
    void getExtensionPoint(QPointF& point,const QPointF& point1,const QPointF& point2,double dis);
private:
    vector<QPointF> tool_path;
    QPointF lead_point,lead_point1;
    //convLTW convert;

};

#endif // CAMCONFIGURE_H
