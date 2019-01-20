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
#include <QDialogButtonBox>
#include <QPalette>

#include "cam_info.h"

using namespace CAM_INFO;

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
    //void selectStartPoint();


private:
    QGroupBox *box1,*box2,*box3;
    QPushButton *ok_btn;

    QLabel *b1_lab1,*b1_lab2,*b1_lab3;
    QLabel *b2_lab1,*b2_lab2;
    QLabel *b3_lab1,*b3_lab2,*b3_lab3,*b3_lab4,*b3_lab5;

    QLineEdit *b1_edit1,*b1_edit2,*b1_edit3;
    QComboBox *b2_tab1,*b2_tab2;

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

};

#endif // CAMDLG_H
