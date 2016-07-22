#include "stdafx.h"
#include "ProbDebugTool.h"
#include "View/mainUi.h"
#include "Model/PDTModel.h"	
#include "Tools/ProbTools/ProbDllLoader.h"
#include "Tools/ProtoTools/protoManager.h"
#include "BaseInfo.h"

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
		QObject::connect(this, SIGNAL(onUpdateUI(QMap<int, QMap<int, QMap<int, QMap<int, int>>>>)), m_ui, SLOT(slot_onUpdateUI(QMap<int, QMap<int, QMap<int, QMap<int, int>>>>)));

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

	// init model's data.
	m_model->setInitOK(false);// set initOK, 需要初始strips的flag.
	m_model->getLoader()->setCurId(id);// set current id.
	m_model->clearStripsMgr(); // clear stripsMgr.

	// to view setInfo
	auto info = m_model->getSetInfo();// get setInfo
	emit onSetInfo(info);
}

void ProbDebugTool::slot_onSetSetting(QMap<int, QMap<QString, QVariant>> settings)
{
	// set settings
	m_model->setSettings(settings);

	// init strips 要全部轉輪表(NG/FG的大小輪)
	auto size = settings[Type_SetInfo::T_Con][STRIP_COUNT_NG].toInt();
	for (int i = 0; i < size; ++i)
	{
		m_model->Spin(Type_Spin::S_Ng, i);
	}
	size = settings[Type_SetInfo::T_Con][STRIP_COUNT_FG].toInt();
	for (int i = 0; i < size; ++i)
	{
		m_model->Spin(Type_Spin::S_Fg, i);
	}

	// ui change
	if (m_model->getInitOK())
	{
		// send update ui.
		emit onUpdateUI(m_model->getStripsMgr());
	}
}

#include "ProbDebugTool.moc"