#include "camtool.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <QVBoxLayout>
#include <QMessageBox>

using namespace CAM_INFO;

CamTool::CamTool(QDialog *parent):
    QDialog(parent)
{
    LayOut();
    //cur_info = info;

    connect(tab,SIGNAL(currentIndexChanged(int)),this,SLOT(setDesc()));
    connect(edit3,SIGNAL(textChanged(QString)),this,SLOT(setDesc()));

    connect(buttonBox,SIGNAL(accepted()),this,SLOT(setData()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}

void CamTool::createBox()
{
    box1 = new QGroupBox(tr("Tool Parameters"));

    lab1 = new QLabel(this);
    lab1->setText(tr("Tool Code:"));
    edit1 = new QLineEdit(this);
    //if(code_st.empty()) edit1->setText(tr("0"));
    edit1->setText(QVariant(code_st.size() + 1).toString());

    edit1->setValidator(new QIntValidator(0,10,this));

    lab2 = new QLabel(this);
    lab2->setText(tr("Tool Type:"));
    tab = new QComboBox(this);
    tab->insertItem(0,tr("Mill"));
    tab->insertItem(1,tr("Drill"));
    tab->setCurrentIndex(0);

    lab3 = new QLabel(this);
    lab3->setText(tr("Description:"));
    edit2 = new QLineEdit(this);
    edit2->setText(tr("Mill (5mm)"));

    lab4 = new QLabel(this);
    lab4->setText(tr("Diameter:"));
    edit3 = new QLineEdit(this);
    edit3->setText(tr("0"));
    unit_lab1 = new QLabel(this);
    unit_lab1->setText(tr("mm"));

    lab5 = new QLabel(this);
    lab5->setText(tr("Spindle Speed:"));
    edit4 = new QLineEdit(this);
    edit4->setText(tr("0"));
    unit_lab2 = new QLabel(this);
    unit_lab2->setText(tr("r.p.m"));

    lab6 = new QLabel(this);
    lab6->setText(tr("Feed Rate"));
    edit5 = new QLineEdit(this);
    edit5->setText(tr("0"));

    lab7 = new QLabel(this);
    lab7->setText(tr("Plunge Rate"));
    edit6 = new QLineEdit(this);
    edit6->setText(tr("0"));

    b1_layout = new QGridLayout;

    b1_layout->addWidget(lab1,0,0);
    b1_layout->addWidget(edit1,0,1);

    b1_layout->addWidget(lab2,1,0);
    b1_layout->addWidget(tab,1,1);

    b1_layout->addWidget(lab3,2,0);
    b1_layout->addWidget(edit2,2,1);

    b1_layout->addWidget(lab4,3,0);
    b1_layout->addWidget(edit3,3,1);
    b1_layout->addWidget(unit_lab1,3,2);

    b1_layout->addWidget(lab5,4,0);
    b1_layout->addWidget(edit4,4,1);
    b1_layout->addWidget(unit_lab2,4,2);

    b1_layout->addWidget(lab6,5,0);
    b1_layout->addWidget(edit5,5,1);

    b1_layout->addWidget(lab7,6,0);
    b1_layout->addWidget(edit6,6,1);

    box1->setLayout(b1_layout);
}

void CamTool::LayOut()
{
    setWindowTitle(tr("Edit CAM Tool"));
    createBox();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(box1);
    vbox->addWidget(buttonBox);

    this->setLayout(vbox);
}

void CamTool::setDesc()
{
    QString str;
    str.append(tab->currentIndex() == 0 ? tr("Mill") : tr("Drill"));
    str.append(tr(" ("));
    str.append(edit3->text());
    str.append(tr("mm)"));

    edit2->setText(str);
}

void CamTool::setData()
{
    cur_info.tool_code = edit1->text().toInt();
    if(code_st.find(cur_info.tool_code) != code_st.end() && t_flag)
    {
        QMessageBox::information(this,"Error","The ToolCode has been used!");
        return;
    }

    cur_info.diameter = edit3->text().toDouble();
    cur_info.tool_type = (tab->currentIndex() == 0) ? CAM_INFO::ToolType::Mill
                                                     : CAM_INFO::ToolType::Drill;

    cur_info.spin_speed = edit4->text().toDouble();
    cur_info.feed_rate = edit4->text().toDouble();
    cur_info.plunge_rate = edit4->text().toDouble();

    code_st.insert(cur_info.tool_code);

    this->accept();

}

Tool_Info CamTool::getInfo()
{
    return cur_info;
}

void CamTool::setFlag(bool flag)
{
    t_flag = flag;
}

void CamTool::setInfo(Tool_Info info)
{
    cur_info = info;
}

