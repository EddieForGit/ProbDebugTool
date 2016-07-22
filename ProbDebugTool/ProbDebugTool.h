#pragma once
#include <QtCore/qobject.h>

class mainUi;
class PDTModel;

class ProbDebugTool : public QObject
{
	Q_OBJECT
public:
	ProbDebugTool(QObject* parent = 0);
	~ProbDebugTool();


signals:
	// to view
	void onInit(QMap<QString,int>);
	void onSetInfo(QMap<int, QMap<QString, QVariant>>);
	void onUpdateUI(QMap<int, QMap<int, QMap<int, QMap<int, int>>>>);

protected slots:
	void slot_onChangeProb(QString name);
	void slot_onSetSetting(QMap<int, QMap < QString, QVariant>> settings);

private:
	// model
	PDTModel *m_model;
	// ui
	mainUi *m_ui;
};

