﻿#include <QtGui>
#include <QGLWidget>

#include "CraftuxHome.h"
#include "GameWindow.h"
#include "LocalServerConnector.h"
#include "ServerConnector.h"
#include "ui_CraftuxHome.h"

CraftuxHome::CraftuxHome(QWidget *parent) :
	QWidget(parent), ui(new Ui::CraftuxHome), b_optionDialogOpened(false), m_optionsDialog(NULL)
{
	ui->setupUi(this);
	connect(ui->soloButton, SIGNAL(clicked()), this, SLOT(soloGameLaunch()));
	connect(ui->optionsButton, SIGNAL(clicked()), this, SLOT(openOptions()));
	connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
}

CraftuxHome::~CraftuxHome()
{
	delete ui;
}

void CraftuxHome::soloGameLaunch()
{
	ClientConfiguration* configuration = new ClientConfiguration();
	configuration->loadDefaultConfigFile();
	// Création et génération du monde
	ServerConnector* connector = new LocalServerConnector();
	connector->world().setSeed(configuration->getSeed());
	GameWindow* gameWindow = new GameWindow(configuration, connector);
	this->close();
	gameWindow->show();
}

void CraftuxHome::openOptions()
{
	if(b_optionDialogOpened)
	{
		// we don't open two options dialog at one time
	}
	else
	{
		if(m_optionsDialog != NULL) delete m_optionsDialog;
		m_optionsDialog = new OptionsDialog(this);
		m_optionsDialog->show();
		connect(m_optionsDialog, SIGNAL(finished(int)), this, SLOT(optionsClosed()));
		b_optionDialogOpened = true;
	}
}

void CraftuxHome::optionsClosed()
{
	b_optionDialogOpened = false;
	m_optionsDialog = NULL;
}

