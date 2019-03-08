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

    connect(this,SIGNAL(dataChanged(Tool_Info)),this,SLOT(setSpeed(Tool_Info)));
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
    b1_lab1->setText(tr("Tool:"));

    b1_lab2 = new QLabel(this);
    b1_lab2->setText(tr("Feed Rate:"));

    b1_lab3 = new QLabel(this);
    b1_lab3->setText(tr("Plunge Rate:"));

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
    b1_edit1->setText(tr("0"));
    b1_edit1->setValidator(new QDoubleValidator(-1000.0,1000.0,2,this));

    b1_edit2 = new QLineEdit(this);
    b1_edit2->setText(tr("0"));
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
    box2 = new QGroupBox(tr("Side / Directions"));

    b2_lab1 = new QLabel(this);
    b2_lab1->setText(tr("Side:"));

    b2_lab2 = new QLabel(this);
    b2_lab2->setText(tr("Direction:"));

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
    b3_lab1->setText(tr("Lead In Type:"));

    b3_lab2 = new QLabel(this);
    b3_lab2->setText(tr("Lead Out Type:"));
    b3_lab2->setPalette(pa);

    b3_lab3 = new QLabel(this);
    b3_lab3->setText(tr("Lead In Length:"));

    b3_lab4 = new QLabel(this);
    b3_lab4->setText(tr("Lead Out Length:"));
    b3_lab4->setPalette(pa);

    b3_lab5 = new QLabel(this);
    b3_lab5->setText(tr("Overcut:"));

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
    b3_edit1->setText(tr("10"));
    b3_edit1->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b3_edit2 = new QLineEdit(this);
    b3_edit2->setText(tr("10"));
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

void CamDlg::createFourthGroup()
{
    box4 = new QGroupBox(tr("Cutting Depths:"));

    b4_lab0 = new QLabel(this);
    b4_lab0->setText(tr("Start Z"));

    b4_lab1 = new QLabel(this);
    b4_lab1->setText(tr("Safe Z (a):"));

    b4_lab2 = new QLabel(this);
    b4_lab2->setText(tr("Start Depth (b):"));

    b4_lab3 = new QLabel(this);
    b4_lab3->setText(tr("Cut Depth (c):"));

    b4_lab4 = new QLabel(this);
    b4_lab4->setText(tr("Passes:"));

    b4_edit0 = new QLineEdit(this);
    b4_edit0->setText(tr("50"));

    b4_edit1 = new QLineEdit(this);
    b4_edit1->setText(tr("0"));
    b4_edit1->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b4_edit2 = new QLineEdit(this);
    b4_edit2->setText(tr("0"));
    b4_edit2->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b4_edit3 = new QLineEdit(this);
    b4_edit3->setText(tr("4"));
    b4_edit3->setValidator(new QDoubleValidator(-50.0,50.0,2,this));

    b4_edit4 = new QLineEdit(this);
    b4_edit4->setText(tr("1"));
    b4_edit4->setValidator(new QIntValidator(0,10,this));

    b4_unit_lab0 = new QLabel(this);
    b4_unit_lab0->setText(tr("mm"));

    b4_unit_lab1 = new QLabel(this);
    b4_unit_lab1->setText(tr("mm"));

    b4_unit_lab2 = new QLabel(this);
    b4_unit_lab2->setText(tr("mm"));

    b4_unit_lab3 = new QLabel(this);
    b4_unit_lab3->setText(tr("mm"));

    b4_layout = new QGridLayout;

    b4_layout->addWidget(b4_lab0,0,0);
    b4_layout->addWidget(b4_lab1,1,0);
    b4_layout->addWidget(b4_lab2,2,0);
    b4_layout->addWidget(b4_lab3,3,0);
    b4_layout->addWidget(b4_lab4,4,0);

    b4_layout->addWidget(b4_edit0,0,1);
    b4_layout->addWidget(b4_edit1,1,1);
    b4_layout->addWidget(b4_edit2,2,1);
    b4_layout->addWidget(b4_edit3,3,1);
    b4_layout->addWidget(b4_edit4,4,1);

    b4_layout->addWidget(b4_unit_lab0,0,2);
    b4_layout->addWidget(b4_unit_lab1,1,2);
    b4_layout->addWidget(b4_unit_lab2,2,2);
    b4_layout->addWidget(b4_unit_lab3,3,2);

    box4->setLayout(b4_layout);

}

void CamDlg::layOut()
{
    setWindowTitle(tr("Cam Toolpath"));

    createFirstGroup();
    createSecondGroup();
    createThirdGroup();
    createFourthGroup();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(box1);
    vbox->addWidget(box2);
    vbox->addWidget(box3);
    vbox->addWidget(box4);
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

    dep_info.start_z = b4_edit0->text().toDouble();
    dep_info.safe_z = b4_edit1->text().toDouble();
    dep_info.start_depth = b4_edit2->text().toDouble();
    dep_info.cut_depth = b4_edit3->text().toDouble();
    dep_info.passes = b4_edit4->text().toDouble();


    this->accept();
}

double CamDlg::getRadius() const
{
    return radius;
}

double CamDlg::getLeadLength() const
{
    return lead_length;
}

double CamDlg::getOverCut() const
{
    return over_cut;
}

DirectionType CamDlg::getDirection() const
{
    return direction;
}

SideType CamDlg::getSide() const
{
    return side;
}

LeadType CamDlg::getLead() const
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

        b1_edit1->setText(QVariant(new_info.feed_rate).toString());
        b1_edit2->setText(QVariant(new_info.plunge_rate).toString());

        emit dataChanged(tool_mp[cur_code]);
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

        Tool_Info cur_info = tool_mp[cur_code];

        b1_edit1->setText(QVariant(cur_info.feed_rate).toString());
        b1_edit2->setText(QVariant(cur_info.plunge_rate).toString());

        emit dataChanged(tool_mp[cur_code]);
    }
}

void CamDlg::changeCurCode()
{
    QString str = b1_tab->currentText();
    int sz = str.size();

    cur_code = QString(str.at(sz - 1)).toInt();
}

void CamDlg::setSpeed(Tool_Info info)
{
    b1_edit1->setText(QVariant(info.feed_rate).toString());
    b1_edit2->setText(QVariant(info.plunge_rate).toString());
}

Tool_Info CamDlg::getToolInfo()
{
    return tool_mp[cur_code];
}

Depth_Info CamDlg::getDepthInfo() const
{
    return dep_info;
}