#include "PDTModel.h"
#include "../Tools/ProbTools/ProbDllLoader.h"
#include "../Tools/ProtoTools/protoManager.h"
#include <QtCore/qvariant.h>
#include <QtCore/qsettings.h>
#include <QtCore/qfile.h>
#include "../BaseInfo.h"
#include <iostream>

PDTModel::PDTModel()
	: QObject()
	, m_protoMgr(new protoManager("../protoCollect.ini"))
	, store_path("../store/")
	, proto_path("../gen/prob/proto/add_proto.ini")
	, initOK(false)
{
	if (m_protoMgr)
	{
		m_loader = new ProbDllLoader(m_protoMgr);
		// m_loader
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

QMap<int, QMap<QString, QVariant>> PDTModel::getSettings()
{
	return m_settings;
}

QMap<int, QMap<int, QMap<int, QMap<int, int>>>> PDTModel::getStripsMgr()
{
	return m_stripsMgr;
}

void PDTModel::Spin(int spin_type, int debug_reel_type)
{
	auto proto_name = m_protoNames[m_loader->getCurId()];
	auto msg = m_protoMgr->getMessage(proto_name, QString("%1.%2").arg(proto_name).arg(spin_type == Type_Spin::S_Ng ? NG_SPIN : FG_SPIN));

	for (int idx = 0; idx < msg->GetDescriptor()->field_count(); ++idx)
	{
		auto name = QString::fromStdString(msg->GetDescriptor()->field(idx)->name());
		auto type = msg->GetDescriptor()->field(idx)->type();
		auto data = m_settings[spin_type == Type_Spin::S_Ng ? Type_SetInfo::T_Ng : Type_SetInfo::T_Fg][name];

		if (initOK)
		{
			// 設定field
			setProtoFeild(msg, name, type, data);
		}
		else
		{
			if (name == m_settings[Type_SetInfo::T_Con][REELS_NAME]) // 設定轉輪盤面
			{
				auto count = m_settings[Type_SetInfo::T_Con][REEL_SIZE_X].toInt();
				for (int idx = 0; idx < count; ++idx)
				{
					m_protoMgr->addRepeated(msg, name.toStdString(), QVariant(-1));
				}
			}
			else if (name == m_settings[Type_SetInfo::T_Con][STRIPS_NAME]) // 設定大小轉輪
			{
				m_protoMgr->setItem(msg, name.toStdString(), QVariant(debug_reel_type));
			}
			else
			{
				// 設定field
				setProtoFeild(msg, name, type, data);
			}
		}
	}

	m_loader->SendToProbDll(msg);
}

void PDTModel::clearStripsMgr()
{ m_stripsMgr.clear(); }

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

void PDTModel::setProtoFeild(google::protobuf::Message *msg, QString name, int type, QVariant data)
{
	if (msg == nullptr || name.isEmpty()) return;

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

void PDTModel::slot_onProb_out(google::protobuf::Message *msg)
{
	if (msg == nullptr) return;
	
	int type;
	auto typeName= QString::fromStdString(msg->GetDescriptor()->name());
	
	if (typeName == NG_OUT)
	{
		type = Type_Spin::S_Ng;
	} 
	else if (typeName == FG_OUT)
	{
		type = Type_Spin::S_Fg;
	} 

	if (initOK) // normal prob out
	{
		auto b = 1;
	}
	else // init strips
	{	
		auto strips = m_protoMgr->getRepeatedMessage(msg, m_settings[Type_SetInfo::T_Con][REELS_NAME].toString().toStdString());
		QMap<int, QMap<int, int>> reel_map;
		for (auto reel_idx = 0; reel_idx < strips->size(); ++reel_idx)
		{
			auto &reel_msg = strips->Get(reel_idx);
			auto row_size = reel_msg.GetReflection()->FieldSize(reel_msg, reel_msg.GetDescriptor()->FindFieldByName(PROB_REELS_NAME));
			QMap<int, int> row_map;
			for (auto row_idx = 0; row_idx < row_size; ++row_idx)
			{
				auto value = reel_msg.GetReflection()->GetRepeatedInt32(reel_msg, reel_msg.GetDescriptor()->FindFieldByName(PROB_REELS_NAME), row_idx);
				row_map.insert(row_idx, value);
			}
			reel_map.insert(reel_idx, row_map);
		}
		int reelType = m_stripsMgr[type].size();
		m_stripsMgr[type].insert(reelType, reel_map);

		// adj flag
		auto b = m_settings[Type_SetInfo::T_Con][STRIP_COUNT_NG].toInt();
		if (m_stripsMgr[Type_Spin::S_Ng].size() == m_settings[Type_SetInfo::T_Con][STRIP_COUNT_NG].toInt() &&
			m_stripsMgr[Type_Spin::S_Fg].size() == m_settings[Type_SetInfo::T_Con][STRIP_COUNT_FG].toInt())
		{
			this->setInitOK(true);
			qDebug() << "[Model] init stripsMgr OK.";
		}
	}
}

void PDTModel::setInitOK(bool b)
{ initOK = b; }

bool PDTModel::getInitOK()
{ return initOK; }


#include "PDTModel.moc"