#ifndef CAMDLG_H
#define CAMDLG_H

#include <QPushButton>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPalette>
#include <vector>
#include <map>
#include "cam_info.h"
#include "camtool.h"

using namespace CAM_INFO;
using namespace std;

struct Depth_Info
{
    double start_z,safe_z,start_depth,cut_depth;
    int passes;
};

class CamDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CamDlg(QWidget* parent = nullptr);
    ~CamDlg();

    void init();
    void layOut();

    double getRadius() const;
    double getLeadLength() const;
    double getOverCut() const;
    Depth_Info getDepthInfo() const;

    LeadType getLead() const;
    DirectionType getDirection() const;
    SideType getSide() const;
    Tool_Info getToolInfo();

    void createFirstGroup();
    void createSecondGroup();
    void createThirdGroup();
    void createFourthGroup();

signals:
    void dataChanged(Tool_Info info);

private slots:
    void setData();
    void changeLabel(int index);
    void addTool();
    void editTool();
    void changeCurCode();
    void setSpeed(Tool_Info info);
    //void selectStartPoint();


private:
    CamTool *tool_dlg;
    QGroupBox *box1,*box2,*box3,*box4;
    QPushButton *addtool,*edittool;

    QLabel *b1_lab1,*b1_lab2,*b1_lab3;
    QLabel *b2_lab1,*b2_lab2;
    QLabel *b3_lab1,*b3_lab2,*b3_lab3,*b3_lab4,*b3_lab5;

    QLineEdit *b1_edit1,*b1_edit2;
    QComboBox *b1_tab,*b2_tab1,*b2_tab2;

    QComboBox *b3_tab1,*b3_tab2;
    QLineEdit *b3_edit1,*b3_edit2,*b3_edit3;
    QLabel *b3_unit_lab1,*b3_unit_lab2,*b3_unit_lab3;

    QLabel *b4_lab0,*b4_lab1,*b4_lab2,*b4_lab3,*b4_lab4;
    QLineEdit *b4_edit0,*b4_edit1,*b4_edit2,*b4_edit3,*b4_edit4;
    QLabel *b4_unit_lab0,*b4_unit_lab1,*b4_unit_lab2,*b4_unit_lab3;

    QGridLayout *b1_layout,*b2_layout,*b3_layout,*b4_layout;

    QGridLayout *layout;
    QDialogButtonBox *buttonBox;

    double radius;
    double lead_length,lead_radius,over_cut;

    DirectionType direction;
    SideType side;
    LeadType lead_type;

    Depth_Info dep_info;

    map<int,Tool_Info> tool_mp;
    int cur_code;
    vector<int> code_vec;
};

#endif // CAMDLG_H