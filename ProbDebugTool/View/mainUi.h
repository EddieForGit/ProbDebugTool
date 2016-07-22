#pragma once
#include <QtWidgets/qwidget.h>
#include <QtCore/qmap.h>

class QGroupBox;

namespace Ui {
	class ui_main;
}

class mainUi : public QWidget
{
	Q_OBJECT
public:
	mainUi(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~mainUi();

private:
	void createUiGroupBox(QGroupBox *gb, QString objName, QMap<QString, QVariant> info);
	QMap<int, QMap < QString, QVariant>> getSettings(); // 取得ui_setting頁上面的資訊.

signals:
	void onChangeProb(QString);
	void onSetSetting(QMap<int, QMap < QString, QVariant>>);

protected slots:
	void slot_onInit(QMap<QString, int> probs);
	void slot_onSetInfo(QMap<int, QMap<QString, QVariant>> setInfo);
	void slot_onUpdateUI(QMap<int, QMap<int, QMap<int, QMap<int, int>>>> updateUi);

private:
	
	Ui::ui_main *ui;
	QWidget *ui_setting;
};

