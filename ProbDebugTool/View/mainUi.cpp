#include "mainUi.h"
#include "ui_main.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>

mainUi::mainUi(QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f)
	, ui_setting(nullptr)
{
	ui = new Ui::ui_main();
	ui->setupUi(this);

	QObject::connect(this->ui->comboBox_prob, &QComboBox::currentTextChanged, [=](QString str) {
		if (!str.isEmpty())
			emit this->onChangeProb(str);
	});
}

mainUi::~mainUi()
{
	delete ui;
	ui_setting->deleteLater();
}

void mainUi::createUiGroupBox(QGroupBox *gb, QString objName, QMap<QString, QVariant> info)
{
	gb->setObjectName(objName);
	QGridLayout *ly = new QGridLayout();
	ly->setAlignment(Qt::AlignTop);
	QMapIterator<QString, QVariant> con(info);
	int m_index = 0;
	while (con.hasNext())
	{
		con.next();

		QLabel *labl = new QLabel(con.key());
		ly->addWidget(labl, m_index, 0);

		QLineEdit *text = new QLineEdit(con.value().toString());
		ly->addWidget(text, m_index, 1);

		m_index += 1;
	}
	gb->setLayout(ly);
}

QMap<int, QMap<QString, QVariant>> mainUi::getSettings()
{
	QMap<int, QMap<QString, QVariant>> all_map;

	auto groupBoxs = ui_setting->findChildren<QGroupBox*>(); // 取得ui_setting內QGroupBox的成員.
	for (auto gb : groupBoxs)
	{
		QMap<QString, QVariant> map;

		auto labels = gb->findChildren<QLabel*>();
		auto edits = gb->findChildren<QLineEdit*>();

		if (labels.count() == edits.count()) // 一個label對一個edit, 避免key對value不匹配.
		{
			for (int idx = 0; idx < labels.size(); ++idx)
			{
				map.insert(labels.at(idx)->text(), edits.at(idx)->text());
			}

			int type(-1);
			QString name = gb->objectName();
			if (name == "gb_con")
			{
				type = Type_SetInfo::T_Con;
			}
			else if (name == "gb_ng")
			{
				type = Type_SetInfo::T_Ng;
			}
			else if (name == "gb_fg")
			{
				type = Type_SetInfo::T_Fg;
			}
			else
				qDebug() << "setting's type is error.";

			all_map.insert(type, map);
		}
		else
		{
			qDebug() << "label's size and lineedit's size is diffrent by setting.";
		}
	}

	return all_map;
}

void mainUi::slot_onSetInfo(QMap<int, QMap<QString, QVariant>> setInfo)
{
	// 收到setInfo, 設定設定頁UI資訊. (依照傳來的setInfo內容動態產生UI畫面)
	ui_setting = new QWidget();
	ui_setting->setWindowTitle("Set");
	ui_setting->setWindowFlags(ui_setting->windowFlags() & ~Qt::WindowCloseButtonHint); //disable close btn
	QVBoxLayout *ui_ly = new QVBoxLayout();
	

	QHBoxLayout *gb_ly = new QHBoxLayout();
	gb_ly->setAlignment(Qt::AlignTop);
	ui_ly->addLayout(gb_ly);

	//Con
	QGroupBox *gb = new QGroupBox();
	gb->setTitle("Configure");
	createUiGroupBox(gb, "gb_con", setInfo[Type_SetInfo::T_Con]);
	gb_ly->addWidget(gb);

	//ng
	gb = new QGroupBox();
	gb->setTitle("NG");
	createUiGroupBox(gb, "gb_ng", setInfo[Type_SetInfo::T_Ng]);
	gb_ly->addWidget(gb);

	//fg
	gb = new QGroupBox();
	gb->setTitle("FG");
	createUiGroupBox(gb, "gb_fg", setInfo[Type_SetInfo::T_Fg]);
	gb_ly->addWidget(gb);


	QHBoxLayout *btn_ly = new QHBoxLayout();
	btn_ly->setAlignment(Qt::AlignCenter);
	ui_ly->addLayout(btn_ly);

	// ok
	QPushButton *btn = new QPushButton("OK");
	btn->setMaximumWidth(this->ui->set_btn->width());
	btn->setMaximumHeight(this->ui->set_btn->height());
	btn_ly->addWidget(btn);
	QObject::connect(btn, &QPushButton::clicked, [=]() {
		emit onSetSetting(getSettings()); // 發出ui_setting頁上的資訊.
	});

	// cancel
	btn = new QPushButton("Cancel");
	btn->setMaximumWidth(this->ui->set_btn->width());
	btn->setMaximumHeight(this->ui->set_btn->height());
	btn_ly->addWidget(btn);
	QObject::connect(btn, &QPushButton::clicked, [=]() {
		ui_setting->hide();
		ui_setting->deleteLater();
	});
	

	ui_setting->setLayout(ui_ly);
	ui_setting->setWindowModality(Qt::ApplicationModal);
	ui_setting->show();
}

void mainUi::slot_onInit(QMap<QString, int> probs)
{
	auto names = probs.keys();

	this->ui->comboBox_prob->clear();
	this->ui->comboBox_prob->insertItem(0, "");

	for (int idx = 0; idx < names.count(); ++idx)
	{
		// 因多一個空白default, 所以加1
		this->ui->comboBox_prob->insertItem(idx + 1, names.at(idx));
	}
}

#include "mainUi.moc"