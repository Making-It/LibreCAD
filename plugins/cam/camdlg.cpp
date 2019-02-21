#include "camdlg.h"
#include <QIntValidator>
#include <QDoubleValidator>

CamDlg::CamDlg(QWidget *parent):
    QDialog(parent)
{
    init();
    layOut();

    connect(buttonBox,SIGNAL(accepted()),this,SLOT(setData()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

    connect(b3_tab1,SIGNAL(activated(int)),b3_tab2,SLOT(setCurrentIndex(int)));
    connect(b3_tab2,SIGNAL(activated(int)),b3_tab1,SLOT(setCurrentIndex(int)));

    //根据tab选择的Lead类型，改变标签Lead In Length/Radius
    connect(b1_tab,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCurCode()));

    connect(b3_tab1,SIGNAL(currentIndexChanged(int)),this,SLOT(changeLabel(int)));

    connect(b3_edit1,SIGNAL(textChanged(QString)),b3_edit2,SLOT(setText(QString)));

    connect(addtool,SIGNAL(clicked(bool)),this,SLOT(addTool()));
    connect(edittool,SIGNAL(clicked(bool)),this,SLOT(editTool()));
}


CamDlg::~CamDlg()
{

}

void CamDlg::init()
{
    tool_dlg = new CamTool(this);
    lead_length = -1;
    lead_radius = -1;
    over_cut = -1;
    radius = -1;

    cur_code = -1;
    //code_vec.clear();
    //tool_mp.clear();
}

void CamDlg::changeLabel(int index)
{
    if(index > 2)
    {
        b3_lab3->setText(tr("Lead In Radius"));
        b3_lab4->setText(tr("Lead Out Radius"));
    }
    else
    {
        b3_lab3->setText(tr("Lead In Length"));
        b3_lab4->setText(tr("Lead Out Length"));
    }
}

void CamDlg::createFirstGroup()
{
    box1 = new QGroupBox(tr("Toolpath"));

    b1_lab1 = new QLabel(this);
    b1_lab1->setText(tr("Tool"));

    b1_lab2 = new QLabel(this);
    b1_lab2->setText(tr("Feed Rate"));

    b1_lab3 = new QLabel(this);
    b1_lab3->setText(tr("Plunge Rate"));

    b1_tab = new QComboBox(this);

    QHBoxLayout *hbox = new QHBoxLayout;

    addtool = new QPushButton(this);
    addtool->setText(tr("add"));
    addtool->setMinimumWidth(40);
    addtool->setMaximumWidth(40);
    hbox->addStretch();
    hbox->addWidget(addtool);

    edittool = new QPushButton(this);
    edittool->setText(tr("edit"));
    edittool->setMinimumWidth(45);
    edittool->setMaximumWidth(45);
    hbox->addStretch();
    hbox->addWidget(edittool);

    b1_edit1 = new QLineEdit(this);
    b1_edit1->setText(tr("500"));
    b1_edit1->setValidator(new QDoubleValidator(-1000.0,1000.0,2,this));

    b1_edit2 = new QLineEdit(this);
    b1_edit2->setText(tr("300"));
    b1_edit2->setValidator(new QDoubleValidator(-1000.0,1000.0,2,this));

    b1_layout = new QGridLayout;

    b1_layout->addWidget(b1_lab1,0,0);
    b1_layout->addWidget(b1_tab,0,1);
    b1_layout->addLayout(hbox,0,2);

    b1_layout->addWidget(b1_lab2,1,0);
    b1_layout->addWidget(b1_edit1,1,1);
    //b2_layout->addWidget(edittool,1,2);

    b1_layout->addWidget(b1_lab3,2,0);
    b1_layout->addWidget(b1_edit2,2,1);

    box1->setLayout(b1_layout);
}

void CamDlg::createSecondGroup()
{
    box2 = new QGroupBox(tr("Side/Directions"));

    b2_lab1 = new QLabel(this);
    b2_lab1->setText(tr("Side"));

    b2_lab2 = new QLabel(this);
    b2_lab2->setText(tr("Directions"));

    b2_tab1 = new QComboBox(this);
    b2_tab1->insertItem(0,tr("On"));
    b2_tab1->insertItem(1,tr("Outside"));
    b2_tab1->insertItem(2,tr("Inside"));
    b2_tab1->setCurrentIndex(1);//默认外偏置

    b2_tab2 = new QComboBox(this);
    b2_tab2->insertItem(0,tr("Climb/Left"));
    b2_tab2->insertItem(1,tr("Conventional/Right"));

    b2_layout = new QGridLayout;
    b2_layout->addWidget(b2_lab1,0,0);
    b2_layout->addWidget(b2_tab1,0,1);

    b2_layout->addWidget(b2_lab2,1,0);
    b2_layout->addWidget(b2_tab2,1,1);

    box2->setLayout(b2_layout);

}

void CamDlg::createThirdGroup()
{
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::gray);

    box3 = new QGroupBox(tr("Leads"));

    b3_lab1 = new QLabel(this);
    b3_lab1->setText(tr("Lead In Type"));

    b3_lab2 = new QLabel(this);
    b3_lab2->setText(tr("Lead Out Type"));
    b3_lab2->setPalette(pa);

    b3_lab3 = new QLabel(this);
    b3_lab3->setText(tr("Lead In Length"));

    b3_lab4 = new QLabel(this);
    b3_lab4->setText(tr("Lead Out Length"));
    b3_lab4->setPalette(pa);

    b3_lab5 = new QLabel(this);
    b3_lab5->setText(tr("Overcut"));

    b3_tab1 = new QComboBox(this);
    b3_tab1->insertItem(0,tr("None"));
    b3_tab1->insertItem(1,tr("Normal"));
    b3_tab1->insertItem(2,tr("Extension"));
    b3_tab1->insertItem(3,tr("Half Circle [from Center]"));
    b3_tab1->insertItem(4,tr("Quarter Circle [from Center]"));
    b3_tab1->insertItem(5,tr("Eighth Circle [from Center]"));
    b3_tab1->setCurrentIndex(1);

    b3_tab2 = new QComboBox(this);
    b3_tab2->insertItem(0,tr("None"));
    b3_tab2->insertItem(1,tr("Normal"));
    b3_tab2->insertItem(2,tr("Extension"));
    b3_tab2->insertItem(3,tr("Half Circle [to Center] "));
    b3_tab2->insertItem(4,tr("Quarter Circle [to Center]"));
    b3_tab2->insertItem(5,tr("Eighth Circle [to Center]"));
    b3_tab2->setCurrentIndex(1);

    //b3_tab2->setFocus(Qt::NoFocus);

    b3_edit1 = new QLineEdit(this);
    b3_edit1->setText(tr("0"));
    b3_edit1->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b3_edit2 = new QLineEdit(this);
    b3_edit2->setText(tr("0"));
    b3_edit2->setFocusPolicy(Qt::NoFocus);

    b3_edit3 = new QLineEdit(this);
    b3_edit3->setText(tr("0"));
    b3_edit3->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b3_unit_lab1 = new QLabel(this);
    b3_unit_lab1->setText(tr("mm"));

    b3_unit_lab2 = new QLabel(this);
    b3_unit_lab2->setText(tr("mm"));

    b3_unit_lab3 = new QLabel(this);
    b3_unit_lab3->setText(tr("mm"));

    b3_layout = new QGridLayout;
    b3_layout->addWidget(b3_lab1,0,0);
    b3_layout->addWidget(b3_lab2,1,0);
    b3_layout->addWidget(b3_lab3,2,0);
    b3_layout->addWidget(b3_lab4,3,0);
    b3_layout->addWidget(b3_lab5,4,0);

    b3_layout->addWidget(b3_tab1,0,1);
    b3_layout->addWidget(b3_tab2,1,1);

    b3_layout->addWidget(b3_edit1,2,1);
    b3_layout->addWidget(b3_edit2,3,1);
    b3_layout->addWidget(b3_edit3,4,1);

    b3_layout->addWidget(b3_unit_lab1,2,2);
    b3_layout->addWidget(b3_unit_lab2,3,2);
    b3_layout->addWidget(b3_unit_lab3,4,2);

    box3->setLayout(b3_layout);
}

void CamDlg::layOut()
{
    setWindowTitle(tr("Cam Profile Toolpath"));

    createFirstGroup();
    createSecondGroup();
    createThirdGroup();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(box1);
    vbox->addWidget(box2);
    vbox->addWidget(box3);
    vbox->addWidget(buttonBox);

    this->setLayout(vbox);
}

void CamDlg::setData()
{
    //get side
    if(b2_tab1->currentIndex() == 0)
        side = SideType::On;
    else if(b2_tab1->currentIndex() == 1)
        side = SideType::Outside;
    else
        side = SideType::Inside;

    //get direction
    if(b2_tab2->currentIndex() == 0)
        direction = DirectionType::Left;
    else
        direction = DirectionType::Right;

    //get lead
    int idx = b3_tab1->currentIndex();
    switch(idx)
    {
    case 0:
        lead_type = LeadType::None;
        break;
    case 1:
        lead_type = LeadType::Normal;
        break;
    case 2:
        lead_type = LeadType::Extension;
        break;
    case 3:
        lead_type = LeadType::HalfCircle;
        break;
    case 4:
        lead_type = LeadType::QuarterCircle;
        break;
    case 5:
        lead_type = LeadType::EighthCircle;
        break;
    }

    if(idx <= 2) lead_length = b3_edit1->text().toDouble();
    else lead_radius = b3_edit1->text().toDouble();

    over_cut = b3_edit3->text().toDouble();
    radius = b1_edit1->text().toDouble();

    this->accept();
}

double CamDlg::getRadius()
{
    return radius;
}

double CamDlg::getLeadLength()
{
    return lead_length;
}

double CamDlg::getOverCut()
{
    return over_cut;
}

DirectionType CamDlg::getDirection()
{
    return direction;
}

SideType CamDlg::getSide()
{
    return side;
}

LeadType CamDlg::getLead()
{
    return lead_type;
}

void CamDlg::addTool()
{
    //CamTool tool_dlg((QWidget*)this->parent());
    tool_dlg->setFlag(true);
    tool_dlg->setInfo(Tool_Info());
    tool_dlg->reLayout();

    int res = tool_dlg->exec();

    if(res == QDialog::Accepted)
    {
        if(!tool_dlg->setToolData()) return;

        Tool_Info new_info = tool_dlg->getInfo();

        cur_code = new_info.tool_code;
        tool_mp.insert(make_pair(cur_code,new_info));

        QString str("T");
        int idx = b1_tab->count();
        str.append(QVariant(cur_code).toString());
        b1_tab->insertItem(idx,str);
        b1_tab->setCurrentIndex(idx);
    }
}

void CamDlg::editTool()
{
    if(tool_mp.empty()) return;

    //CamTool tool_dlg(tool_mp[cur_code],(QWidget*)this->parent());
    tool_dlg->setFlag(false);
    tool_dlg->setInfo(tool_mp[cur_code]);
    tool_dlg->reLayout();

    int res = tool_dlg->exec();

    if(res == QDialog::Accepted)
    {
        //添加tool时，如果发现too_code重复，则返回
        if(!tool_dlg->setToolData()) return;

        Tool_Info new_info = tool_dlg->getInfo();

        if(cur_code != new_info.tool_code)
        {
            //编辑模式更改了tool code,从mp和st删除原有的info
            tool_mp.erase(cur_code);
            tool_dlg->removeCode(cur_code);

            int old_idx = b1_tab->currentIndex();

            //加入新的tool code
            cur_code = new_info.tool_code;
            tool_mp.insert(make_pair(cur_code,new_info));

            //设置ComboBox当前index
            QString str("T");
            int idx = b1_tab->count();
            str.append(QVariant(cur_code).toString());
            b1_tab->insertItem(idx,str);
            b1_tab->setCurrentIndex(idx);

            b1_tab->removeItem(old_idx);

        }
        else{
            tool_mp[cur_code] = new_info;
        }
    }
}

void CamDlg::changeCurCode()
{
    QString str = b1_tab->currentText();
    int sz = str.size();

    cur_code = QString(str.at(sz - 1)).toInt();
}

/*
void CamDlg::selectStartPoint()
{

}
*/
