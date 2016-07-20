#include "stdafx.h"
#include "ProbDebugTool.h"
#include "View/mainUi.h"
#include "Model/PDTModel.h"	
#include "Tools/ProbTools/ProbDllLoader.h"
#include "Tools/ProtoTools/protoManager.h"

ProbDebugTool::ProbDebugTool(QObject* parent)
	: QObject(parent)
	, m_model(new PDTModel())
	, m_ui(new mainUi())

	
{
	if (!m_model->getLoader()->getProbs().isEmpty())
	{
		m_ui->show();

		// ToView
		QObject::connect(this, SIGNAL(onInit(QMap<QString, int>)), m_ui, SLOT(slot_onInit(QMap<QString, int>)));
		QObject::connect(this, SIGNAL(onSetInfo(QMap<int, QMap<QString, QVariant>>)), m_ui, SLOT(slot_onSetInfo(QMap<int, QMap<QString, QVariant>>)));

		// FromView
		QObject::connect(m_ui, SIGNAL(onChangeProb(QString)), this, SLOT(slot_onChangeProb(QString)));
		QObject::connect(m_ui, SIGNAL(onSetSetting(QMap<int, QMap < QString, QVariant>>)), this, SLOT(slot_onSetSetting(QMap<int, QMap < QString, QVariant>>)));

		// send init
		emit onInit(m_model->getLoader()->getProbs());
	}
}


ProbDebugTool::~ProbDebugTool()
{
	delete m_model;
	m_ui->deleteLater();
}

void ProbDebugTool::slot_onChangeProb(QString name)
{
	auto id = m_model->getLoader()->getProbs().value(name);

	// set current id
	m_model->getLoader()->setCurId(id);

	// get setInfo
	auto info = m_model->getSetInfo();
	
	// to view setInfo
	emit onSetInfo(info);
}

void ProbDebugTool::slot_onSetSetting(QMap<int, QMap<QString, QVariant>> settings)
{
	// set settings
	m_model->setSettings(settings);

	// init strips
	m_model->initStrips();

	//
}

#include "ProbDebugTool.moc"