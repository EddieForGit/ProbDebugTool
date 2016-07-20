#include "PDTModel.h"
#include "../Tools/ProbTools/ProbDllLoader.h"
#include "../Tools/ProtoTools/protoManager.h"
#include <QtCore/qvariant.h>
#include <QtCore/qsettings.h>
#include <QtCore/qfile.h>

PDTModel::PDTModel()
	: QObject()
	, m_protoMgr(new protoManager("../protoCollect.ini"))
	, store_path("../store/")
	, proto_path("../gen/prob/proto/add_proto.ini")
{
	if (m_protoMgr)
	{
		m_loader = new ProbDllLoader(m_protoMgr);

		connect(m_loader, SIGNAL(onProb_out(google::protobuf::Message*)), this, SLOT(slot_onProb_out(google::protobuf::Message*)));
	}

	if (QFile(proto_path).exists())
	{
		QSettings s(proto_path, QSettings::IniFormat);
		s.beginGroup("protoList");
		for (auto k : s.allKeys())
		{
			m_protoNames.insert(k.toInt(), s.value(k).toString());
		}
		s.endGroup();
	}
	else
	{
		qDebug() << "add_proto.ini is not exist.";
	}

}


PDTModel::~PDTModel()
{
	delete m_loader;
	delete m_protoMgr;
}

ProbDllLoader * PDTModel::getLoader()
{
	return m_loader;
}

QMap<int, QMap<QString, QVariant>> PDTModel::getSetInfo()
{
	QMap<int, QMap<QString, QVariant>> all_map;
	auto proto_name = m_protoNames[m_loader->getCurId()];

	// Con
	QMap<QString, QVariant> con_map;
	con_map.insert(REEL_SIZE_X, "");
	con_map.insert(REEL_SIZE_Y, "");
	con_map.insert(STRIPS_NAME, "");
	con_map.insert(REELS_NAME, "");
	con_map.insert(STRIP_COUNT_NG, "");
	con_map.insert(STRIP_COUNT_FG, "");
	all_map.insert(Type_SetInfo::T_Con, con_map);

	// 取得當前遊戲proto的feild	NG
	QMap<QString, QVariant> ng_map;
	auto msg = m_protoMgr->getMessage(proto_name, QString("%1.%2").arg(proto_name).arg(NG_SPIN));
	for (int idx = 0; idx < msg->GetDescriptor()->field_count(); ++idx)
	{
		ng_map.insert(QString::fromStdString(msg->GetDescriptor()->field(idx)->name()), 0);
	}
	all_map.insert(Type_SetInfo::T_Ng, ng_map);

	// 取得當前遊戲proto的feild	FG
	QMap<QString, QVariant> fg_map;
	msg = m_protoMgr->getMessage(proto_name, QString("%1.%2").arg(proto_name).arg(FG_SPIN));
	for (int idx = 0; idx < msg->GetDescriptor()->field_count(); ++idx)
	{
		fg_map.insert(QString::fromStdString(msg->GetDescriptor()->field(idx)->name()), 0);
	}
	all_map.insert(Type_SetInfo::T_Fg, fg_map);

	// restore. 如果有紀錄檔, update setInfo.
	QString path = QString("%1%2.ini").arg(store_path).arg(m_loader->getCurId());
	if(QFile(path).exists())
	{
		QSettings s(path, QSettings::IniFormat);
		for (auto idx : s.allKeys())
		{
			int type = -1;

			auto sp = idx.split("/");
			if (sp.at(0) == "Con")
			{
				type = Type_SetInfo::T_Con;
			}
			else if (sp.at(0) == "Ng")
			{
				type = Type_SetInfo::T_Ng;
			}
			else if (sp.at(0) == "Fg")
			{
				type = Type_SetInfo::T_Fg;
			}
			else
				qDebug() << "read restore error, the type is illegal.";

			all_map[type][sp.at(1)] = s.value(QString("%1/%2").arg(sp.at(0)).arg(sp.at(1)));
		}		
	}
	

	m_settings = all_map;

	return all_map;
}

void PDTModel::setSettings(QMap<int, QMap<QString, QVariant>> settings)
{
	if (settings.size() == m_settings.size())
	{
		QMapIterator<int, QMap<QString, QVariant>> all_map(settings);
		while (all_map.hasNext())
		{
			all_map.next();
			QMapIterator<QString, QVariant> map(all_map.value());
			while (map.hasNext())
			{
				map.next();
				
				// update m_settings
				m_settings[all_map.key()][map.key()] = map.value();
			}
		}

		// save restore
		saveRestore(m_settings);
	}
	else
	{
		qDebug() << "Set settings error, the settings size is diffrent.";
	}
}

void PDTModel::initStrips()
{
	auto proto_name = m_protoNames[m_loader->getCurId()];
	auto msg = m_protoMgr->getMessage(proto_name, QString("%1.%2").arg(proto_name).arg(NG_SPIN));

	for (int idx = 0; idx < msg->GetDescriptor()->field_count(); ++idx)
	{
		auto name = QString::fromStdString(msg->GetDescriptor()->field(idx)->name());
		auto type = msg->GetDescriptor()->field(idx)->type();
		auto data = m_settings[Type_SetInfo::T_Ng][name];
		// 判斷此field是不是 [轉輪debug介面]
		auto isReels = name == m_settings[Type_SetInfo::T_Con][REELS_NAME] ? true : false;

		setProtoFeild(msg, name, type, data, isReels);
	}

	auto stripName = m_settings[Type_SetInfo::T_Con][STRIPS_NAME].toString().toStdString();
	m_protoMgr->setItem(msg, stripName, -1);
	m_loader->SendToProbDll(msg);
}

void PDTModel::saveRestore(QMap<int, QMap<QString, QVariant>> data)
{
	QString path = QString("%1%2.ini").arg(store_path).arg(m_loader->getCurId());

	if (!QFile(path).exists())
	{
		// create ini file.
		QFile f(path);
		if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
	}

	// save
	QSettings s(path, QSettings::IniFormat);

	QMapIterator<int, QMap<QString, QVariant>> all_map(data);
	while (all_map.hasNext())
	{
		all_map.next();

		QString type("");
		if (all_map.key() == Type_SetInfo::T_Con)
			type = "Con";
		else if (all_map.key() == Type_SetInfo::T_Ng)
			type = "Ng";
		else if (all_map.key() == Type_SetInfo::T_Fg)
			type = "Fg";
		else
			qDebug() << "save store error, the type is illegal.";

		QMapIterator<QString, QVariant> map(all_map.value());
		while (map.hasNext())
		{
			map.next();
			s.setValue(QString("%1/%2").arg(type).arg(map.key()), map.value().toString());
		}
	}
}

void PDTModel::setProtoFeild(google::protobuf::Message *msg, QString name, int type, QVariant data, bool reels)
{
	if (msg == nullptr || name.isEmpty()) return;

	if (reels) // 此次是不是設定盤面的介面field
	{
		auto count = m_settings[Type_SetInfo::T_Con][REEL_SIZE_X].toInt();
		for (int idx = 0; idx < count; ++idx)
			m_protoMgr->addRepeated(msg, name.toStdString(), QVariant(-1));
	}
	else
	{
		switch (type)
		{
		case google::protobuf::FieldDescriptor::TYPE_BOOL:
			m_protoMgr->setItem(msg, name.toStdString(), data.toBool());
			break;
		case google::protobuf::FieldDescriptor::TYPE_INT32:
			m_protoMgr->setItem(msg, name.toStdString(), data.toInt());
			break;
		case google::protobuf::FieldDescriptor::TYPE_INT64:
			m_protoMgr->setItem(msg, name.toStdString(), data.toInt());
			break;
		case google::protobuf::FieldDescriptor::TYPE_STRING:
			m_protoMgr->setItem(msg, name.toStdString(), data.toString().toStdString());
			break;
			//case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
			//	m_protoM->setMessageMsg(msg, name, data.to)
			//break;
		default:
			qDebug() << "auto compose proto error.";
			break;
		}
	}
}

void PDTModel::slot_onProb_out(google::protobuf::Message *msg)
{
	qDebug() << QString::fromStdString(msg->DebugString());
	auto b = 1;
}

#include "PDTModel.moc"