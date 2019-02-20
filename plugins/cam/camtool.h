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
#include <string>

using namespace std;

class CamTool : public QDialog
{
public:
    CamTool(QWidget* parent = nullptr);
    void LayOut();
    void createBox();

private slots:
    void setDesc();

private:
    QLabel *lab1,*lab2,*lab3,*lab4,*lab5,*lab6,*lab7;
    QComboBox *tab;
    QLineEdit *edit1,*edit2,*edit3,*edit4,*edit5,*edit6;
    QGroupBox *box1;
    QGridLayout *b1_layout,*layout;
    QDialogButtonBox *buttonBox;

};

#endif // CAMTOOL_H
