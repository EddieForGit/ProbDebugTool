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

	enum Type_SetInfo {
		// Model&View須同步
		T_Con	= 0,
		T_Ng	= 1,
		T_Fg	= 2,

		T_count	= 3
	};

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

private:
	
	Ui::ui_main *ui;
	QWidget *ui_setting;
};

