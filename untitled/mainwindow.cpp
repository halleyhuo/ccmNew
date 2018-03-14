#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "HmiXtp.h"
#include "IDCXTP/Client/inc/xtpClient.h"
#include <QFile>
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->FluoroMode->addItem("ContinueFl");
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint); //Set window to fixed size
     this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
     this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window

	 //default layout
	 
	ui->widget_Contious->show();
	ui->maOffmasOnUnit->hide();
	ui->maOnmasOffUnit->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FluoroModeOn()
{
/*
    QString fileName(":/icons/FluoroModeOn.png");
    if(QFile(fileName).exists() == false)
        QMessageBox::warning(this, "ssss", "Wrong file name : " + fileName);
    else
    {
        ui->radioButton_Fluo_Mode->setIcon(QIcon(fileName));
        ui->radioButton_sub->setIcon(QIcon(":/icons/SubModeOff.png"));
        ui->radioButton_Rad_mode->setIcon(QIcon(":/icons/RadModeOff.png"));
    }
*/
	SUiCmdFrame_t				SUiCmdFrame;

	SUiCmdFrame.header.sender	= CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
	SUiCmdFrame.header.target	= XTP_NODE_MASTER;
	SUiCmdFrame.header.telID	= SUICMD;
	SUiCmdFrame.header.mode		= XTP_MODE_DIRECT;
	SUiCmdFrame.header.length	= sizeof(SUiCmdFrame);
	SUiCmdFrame.data.cmd		= UICMD_FLUORO_MODE;
	SUiCmdFrame.data.type		= PUSH_BUTTON;
	SUiCmdFrame.data.param		= UICMD_PARAM_PLS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);

}

void MainWindow::SubModeOn()
{
    QString fileName(":/icons/SubModeOn.png");
    if(QFile(fileName).exists() == false)
        QMessageBox::warning(this, "ssss", "Wrong file name : " + fileName);
    else
    {
        ui->radioButton_sub->setIcon(QIcon(fileName));
        ui->radioButton_Fluo_Mode->setIcon(QIcon(":/icons/FluoroModeOff.png"));
        ui->radioButton_Rad_mode->setIcon(QIcon(":/icons/RadModeOff.png"));
    }
}

void MainWindow::RadModeOn()
{
/*
    QString fileName(":/icons/RadModeOn.png");
    if(QFile(fileName).exists() == false)
        QMessageBox::warning(this, "ssss", "Wrong file name : " + fileName);
    else
    {
        ui->radioButton_Rad_mode->setIcon(QIcon(fileName));
        ui->radioButton_sub->setIcon(QIcon(":/icons/SubModeOff.png"));
        ui->radioButton_Fluo_Mode->setIcon(QIcon(":/icons/FluoroModeOff.png"));
    }
*/
	SUiCmdFrame_t				SUiCmdFrame;

	SUiCmdFrame.header.sender	= CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
	SUiCmdFrame.header.target	= XTP_NODE_MASTER;
	SUiCmdFrame.header.telID	= SUICMD;
	SUiCmdFrame.header.mode		= XTP_MODE_DIRECT;
	SUiCmdFrame.header.length	= sizeof(SUiCmdFrame);
	SUiCmdFrame.data.cmd		= UICMD_SNGL_MODE;
	SUiCmdFrame.data.type		= PUSH_BUTTON;
	SUiCmdFrame.data.param		= UICMD_PARAM_PLS;

	CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);	
}

void MainWindow::ColHeightIn()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_ColHeight;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_PLS;
    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::ColHeightOut()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_ColHeight;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_MINUS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::ColWidthIn()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_ColWidth;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_PLS;
    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::ColWidthOut()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_ColWidth;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_MINUS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::ExposureInc()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_EXP_PLUS;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_PLS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::ExposureDec()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_EXP_MINUS;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_MINUS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);
}

void MainWindow::FPSChange()
{
    SUiCmdFrame_t SUiCmdFrame;
    SUiCmdFrame.header.sender = CHmiXtpIf::Sigleton()->xtpClientIf->GetNodeID();
    SUiCmdFrame.header.target = XTP_NODE_MASTER;
    SUiCmdFrame.header.telID = SUICMD;
    SUiCmdFrame.header.mode  = XTP_MODE_DIRECT;
    SUiCmdFrame.header.length = sizeof(SUiCmdFrame);
    SUiCmdFrame.data.cmd      = UICMD_FPS_CHANGE;
    SUiCmdFrame.data.type     = PLUS_MINUS_BUTTON;
    SUiCmdFrame.data.param    = UICMD_PARAM_PLS;

    CHmiXtpIf::Sigleton()->xtpClientIf->XtpSend(&SUiCmdFrame.header);

}

void MainWindow::widgetUpdateShow(ushort fun_id, ushort param, uchar status)
{
    IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"widgetUpdateShow {%x,%d,%d} to mainwindow",
             fun_id,param,status);
    switch(fun_id)
    {
    case UI_SEL_CollimatorHeight:
    {
        float num = param/100.0;
        QString text = QString::number(num,'f',1);
        ui->ColHeight->display(text);
    }
        break;

    case UI_SEL_CollimatorWeight:
    {
        float num = param/100.0;
        QString text = QString::number(num,'f',1);
        ui->ColWidth->display(text);
    }
        break;

    case UI_SEL_Kv://lcdNumber_kV
        if(param>=1000)
        {
            int num = (param+5)/10;
            QString text = QString::number(num);
            ui->lcdNumber_kV->display(text);
        }
        else
        {
            float num = param/10.0;
            QString text = QString::number(num,'f',1);
            ui->lcdNumber_kV->display(text);
        }
        break;

    case UI_SEL_mA://lcdNumber_mA
    {
        float num = param/100.0;
        QString text = QString::number(num,'f',2);
        ui->lcdNumber_mA->display(text);

        break;
    }
    case UI_SEL_mAs:
    {
        if(param<100)
		{
            float num = param/1000.00;
            QString text = QString::number(num,'f',3);
	        ui->lcdNumber_mA->display(text);
		}
		else
		{
            float num = param/1000.0;
            QString text = QString::number(num,'f',3);
	        ui->lcdNumber_mA->display(text);
		}
        break;
    }

	

	case UI_SEL_FLMode://Fluoro Mode, Continue->Pulse, REMARK, may SUB & Roadmap involved in future
	{
		if(param == FlSingle)
		{
			QString fileName(":/icons/RadModeOn.png");
			if(QFile(fileName).exists() == false)
				QMessageBox::warning(this, "ssss", "Wrong file name : " + fileName);
			else
			{
				ui->radioButton_Rad_mode->setIcon(QIcon(fileName));
				ui->radioButton_sub->setIcon(QIcon(":/icons/SubModeOff.png"));
				ui->radioButton_Fluo_Mode->setIcon(QIcon(":/icons/FluoroModeOff.png"));
			}
			
			ui->widget_Contious->hide();
		}
        else 
        {
        
			QString fileName(":/icons/FluoroModeOn.png");
			if(QFile(fileName).exists() == false)
				QMessageBox::warning(this, "ssss", "Wrong file name : " + fileName);
			else
			{
				ui->radioButton_Fluo_Mode->setIcon(QIcon(fileName));
				ui->radioButton_sub->setIcon(QIcon(":/icons/SubModeOff.png"));
				ui->radioButton_Rad_mode->setIcon(QIcon(":/icons/RadModeOff.png"));
			}
			
	        if(FlContinue==param)
			{
	            ui->widget_Contious->show();
				ui->maOffmasOnUnit->hide();
				ui->maOnmasOffUnit->show();
			}
			else if(FlPulse==param || FlSupervision==param || FlRoadMap==param)
	        {
	            ui->widget_Contious->hide();
				ui->maOffmasOnUnit->show();
				ui->maOnmasOffUnit->hide();
	        }
        }
	}
		break;

	case UI_SEL_Fps:
	{
		if( (param%10) != 0)
		{
			float num = param/10.0;
	        QString text = QString::number(num,'f',1);
			ui->lcdNumber_fps->display(text);
		}
		else
		{		
			int num = param/10;
	         QString text = QString::number(num);
			ui->lcdNumber_fps->display(text);
		}
	}
		break;

    case UI_SEL_Tubeload:
    {
        IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"UI_SEL_Tubeload param = %d", param);
        if (param >= 0)
        {
            float num = param /10;
            QString txt = QString::number(num, 'f', 1);
            ui->tubeload_1->display(txt);
        }
    }
        break;

    default:
        break;
    }

}

void MainWindow::BindSigSlot(void* sigSrc)
{
    IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"BindSigSlot");
     connect(CHmiXtpIf::Sigleton(),SIGNAL(ShowUiEvent(ushort,ushort,uchar)),this,SLOT(widgetUpdateShow(ushort,ushort,uchar)));
}

