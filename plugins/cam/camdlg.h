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

class CamDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CamDlg(QWidget* parent = nullptr);
    ~CamDlg();

    void init();
    void layOut();
    double getRadius();
    double getLeadLength();
    double getOverCut();

    LeadType getLead();
    DirectionType getDirection();
    SideType getSide();

    void createFirstGroup();
    void createSecondGroup();
    void createThirdGroup();

private slots:
    void setData();
    void changeLabel(int index);
    void addTool();
    void editTool();
    void changeCurCode();
    //void selectStartPoint();


private:
    CamTool *tool_dlg;
    QGroupBox *box1,*box2,*box3;
    QPushButton *addtool,*edittool;

    QLabel *b3_unit_lab1,*b3_unit_lab2,*b3_unit_lab3;
    QLabel *b1_lab1,*b1_lab2,*b1_lab3;
    QLabel *b2_lab1,*b2_lab2;
    QLabel *b3_lab1,*b3_lab2,*b3_lab3,*b3_lab4,*b3_lab5;

    QLineEdit *b1_edit1,*b1_edit2;
    QComboBox *b1_tab,*b2_tab1,*b2_tab2;

    QComboBox *b3_tab1,*b3_tab2;
    QLineEdit *b3_edit1,*b3_edit2,*b3_edit3;

    QGridLayout *b1_layout,*b2_layout,*b3_layout;

    QGridLayout *layout;
    QDialogButtonBox *buttonBox;

    double radius;
    double lead_length,lead_radius,over_cut;

    DirectionType direction;
    SideType side;
    LeadType lead_type;


    map<int,Tool_Info> tool_mp;
    int cur_code;
    vector<int> code_vec;
};

#endif // CAMDLG_H
