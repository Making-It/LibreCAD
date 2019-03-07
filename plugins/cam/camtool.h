#ifndef CAMTOOL_H
#define CAMTOOL_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <string>
#include <set>
#include "cam_info.h"

using namespace std;

class Tool_Info
{
public:
    Tool_Info()
    {
        tool_code = 1;
        diameter = 5;
        tool_type = CAM_INFO::ToolType::Mill;
        spin_speed = 2500;
        feed_rate = 500;
        plunge_rate = 250;
    }

    int tool_code;
    double diameter;
    CAM_INFO::ToolType tool_type;
    double spin_speed,feed_rate,plunge_rate;
};

class CamTool : public QDialog
{
public:
    CamTool(QWidget* parent = nullptr);
    void LayOut();
    void createBox();
    bool setToolData();
    void removeCode(int code);

public slots:
    void setFlag(bool flag);
    void setInfo(Tool_Info info);
    Tool_Info getInfo();
    void reLayout();

private slots:
    void setDesc();

private:
    QPushButton *ok_btn,*cancel_btn;
    QLabel *lab1,*lab2,*lab3,*lab4,*lab5,*lab6,*lab7;
    QLabel *unit_lab1,*unit_lab2;
    QComboBox *tab;
    QLineEdit *edit1,*edit2,*edit3,*edit4,*edit5,*edit6;
    QGroupBox *box1;
    QGridLayout *b1_layout,*layout;
    QDialogButtonBox *buttonBox;

    Tool_Info cur_info;
    set<int> code_st;
    bool t_flag;//true -> add tool ; false -> edit tool

};

#endif // CAMTOOL_H
