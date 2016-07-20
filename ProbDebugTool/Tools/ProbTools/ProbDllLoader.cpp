#pragma once
#include "ProbDllLoader.h"
#include "../ProtoTools/protoManager.h"
#include <google/protobuf/descriptor.pb.h>

#include <QtCore\qlibrary.h>
#include <QtCore\qdebug.h>
#include <QtCore\qsettings.h>
#include <QtCore\qfile.h>
#include <QtCore\qxmlstream.h>
#include <iostream>

typedef void(__stdcall *ProbOut)(int no, int game_id, const char *begin, int size);
typedef void(*SetCallBack)(int game_id, ProbOut func);
typedef void(*CreateGlobalData)(bool byseat, const char *begin, int size);
typedef void(*CreateSeatProb)(int machine_id, const char *begin, int size);
typedef void(*DestroySeatProb)(int machine_id);
typedef void(*ProbIn)(int machine_id, const char *begin, int size);

struct ProbInfo
{
	ProbInfo()
		:initialized(false)
	{
	};

	int id;
	QString file;
	QString name;
	QString type;
	QLibrary lib;
	CreateGlobalData Create_Globa;
	CreateSeatProb Create_Seat_Prob;
	DestroySeatProb Destroy_Seat_Prob;
	ProbIn Prob_In;
	bool initialized;
};

ProbDllLoader* m_localmgr = nullptr;

void __stdcall prob_out(int machine_id, int game_id, const char *begin, int size)
{
	if (m_localmgr)
	{
		m_localmgr->out(machine_id, game_id, begin, size);
	}
}

ProbDllLoader::ProbDllLoader(protoManager* protos) 
	: QObject()
	, m_protoM(protos)
	, cur_id(0)
{
	// link dll for this.
	m_localmgr = this;

	// init probs from dll.
	initProbFromDll(QString("../gen/ProbMgr.ini"));

	if (m_probs.isEmpty())
	{
		qDebug() << "ProbDllLoader init error, probs is empty.";
	}
}

ProbDllLoader::~ProbDllLoader()
{
}

void ProbDllLoader::initProbFromDll(QString path)
{
	QSettings setting(path, QSettings::IniFormat);
	QString prefix = setting.value("Perfix", "../gen/prob").toString();
	QFile xmlfile(QString("%1/%2").arg(prefix).arg(setting.value("ProbList", "ProbList.xml").toString()));
	if (!xmlfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "GameList.xml file not exist!!" << endl;
		return;
	}

	QXmlStreamReader reader(&xmlfile);
	if (reader.readNextStartElement())
	{
		if (reader.name() == "ProbList")
		{
			while (reader.readNextStartElement())
			{
				if (reader.name() == "Prob")
				{
					QXmlStreamAttributes attributes = reader.attributes();

					int id = attributes.value("id").toString().toInt();
					ProbInfo *info = new ProbInfo;
					info->id = id;
					info->file = QString("%1/%2").arg(prefix).arg(attributes.value("file").toString());
					info->type = attributes.value("type").toString();
					info->name = reader.readElementText();

					info->lib.setFileName(info->file);

					if (info->lib.load())
					{
						((SetCallBack)info->lib.resolve("SetCallBack"))(id, prob_out);
						info->Create_Globa = (CreateGlobalData)info->lib.resolve("CreateGlobalData");
						info->Create_Seat_Prob = (CreateSeatProb)info->lib.resolve("CreateSeatProb");
						info->Destroy_Seat_Prob = (DestroySeatProb)info->lib.resolve("DestroySeatProb");
						info->Prob_In = (ProbIn)info->lib.resolve("ProbIn");
						// 初始化 Global
						m_probs.insert(id, info);
					}
					else
					{
						delete info;
					}
				}
			}
		}
	}


	///> CreateGlobalData
	for (auto p : m_probs)
	{
		auto InDll = find_prob_by_game_id(p->id);
		if (InDll)
		{
			std::string b("");
			InDll->Create_Globa(false, b.data(), b.size());
		}
	}
}

//void ProbDllLoader::initStrips(google::protobuf::Message* msg, int spin_type)
//{
//	if (msg == nullptr) return;
//
//	std::string strip_name("");
//	int map_type(-1);
//	if (spin_type == ROOL_DEF::T_NgSpin)
//	{
//		strip_name = m_model->getSettingInfo().value(CONFIG_NG_NAME).key(NG_ENTER).toStdString();
//		map_type = STRIPS_MAP::S_Ng;
//	}
//	else if (spin_type == ROOL_DEF::T_FgSpin)
//	{
//		strip_name = m_model->getSettingInfo().value(CONFIG_FG_NAME).key(FG_ENTER).toStdString();
//		map_type = STRIPS_MAP::S_Fg;
//	}
//		
//	auto strips = m_protoM->getRepeatedMessage(msg, strip_name);
//	
//	if (strips->size() == m_model->getConInfoMap().value(REEL_COUNT_X_NAME).toInt())
//	{
//		auto map = m_model->getStripsMap();
//		auto key = map[map_type].size();
//		for (int reel_idx = 0; reel_idx < strips->size(); ++reel_idx)
//		{
//			auto &tmp = strips->Get(reel_idx);
//			auto _size = tmp.GetReflection()->FieldSize(tmp, tmp.GetDescriptor()->FindFieldByName("reels"));
//			QMap<int, int> smap;
//			for (int o_idx = 0; o_idx < _size; ++o_idx)
//			{
//				auto v = tmp.GetReflection()->GetRepeatedInt32(tmp, tmp.GetDescriptor()->FindFieldByName("reels"), o_idx);
//				smap.insert(o_idx, v);
//			}
//			map[map_type][key].insert(reel_idx, smap);
//		}
//		m_model->setStripsMap(map);
//		qDebug() << QString("Init strips %1[%2] ok .").arg(map_type > 0 ? CONFIG_FG_NAME : CONFIG_NG_NAME).arg(key);
//	}
//	else 
//		qDebug() << "Init strips error, the reel's count is different.";
//}

//void ProbDllLoader::autoComposeProto(google::protobuf::Message *msg, int type, std::string name, QVariant data)
//{
//	if (msg == nullptr || name.empty() || data.isNull()) return;
//
//	switch (type)
//	{
//	case google::protobuf::FieldDescriptor::TYPE_BOOL:
//		m_protoM->setItem(msg, name, data.toBool());
//	break;
//	case google::protobuf::FieldDescriptor::TYPE_INT32:
//		m_protoM->setItem(msg, name, data.toInt());
//	break;
//	case google::protobuf::FieldDescriptor::TYPE_INT64:
//		m_protoM->setItem(msg, name, data.toInt());
//	break;
//	case google::protobuf::FieldDescriptor::TYPE_STRING:
//		m_protoM->setItem(msg, name, data.toString().toStdString());
//	break;
//	//case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
//	//	m_protoM->setMessageMsg(msg, name, data.to)
//	//break;
//	default:
//		std::cout << "auto compose proto error.";
//		break;
//	}
//}
//
//void ProbDllLoader::setInReels(QList<int> strips, google::protobuf::Message* msg, int spin_type)
//{
//	if (msg == nullptr) return;
//
//	auto reelx_count = getModel()->getConInfoMap().value(REEL_COUNT_X_NAME).toInt();
//	std::string strip_name;
//
//	if (spin_type == ROOL_DEF::T_NgSpin)
//	{
//		strip_name = getModel()->getSettingInfo().value(CONFIG_NG_NAME).key(NG_ENTER).toStdString();
//	}
//	else if (spin_type == ROOL_DEF::T_FgSpin)
//	{
//		strip_name = getModel()->getSettingInfo().value(CONFIG_FG_NAME).key(FG_ENTER).toStdString();
//	}
//		
//	if (strips.size() == reelx_count)
//	{
//		for (int i = 0; i < reelx_count; ++i)
//			m_protoM->addRepeated(msg, strip_name, strips.at(i));
//	}
//	else
//	{
//		for (int i = 0; i < reelx_count; ++i)
//			m_protoM->addRepeated(msg, strip_name, -1);
//	}
//}
//
//void ProbDllLoader::setInStripsType(int strip_type, google::protobuf::Message * msg, int spin_type)
//{
//	if (msg == nullptr) return;
//
//	std::string strip_name;
//
//	if (spin_type == ROOL_DEF::T_NgSpin)
//	{
//		strip_name = getModel()->getConInfoMap().value(NG_REEL_TYPE_NAME).toStdString();
//	}
//	else if (spin_type == ROOL_DEF::T_FgSpin)
//	{
//		strip_name = getModel()->getConInfoMap().value(FG_REEL_TYPE_NAME).toStdString();
//	}
//
//	m_protoM->setItem(msg, strip_name, strip_type);
//}

//void ProbDllLoader::sendProb(google::protobuf::Message* msg)
//{
//	if (msg == nullptr) return;
//
//	//std::cout << "*******************************************************************[ Start ]" << std::endl;
//	//std::cout << "Send [ " << msg->GetTypeName() << " ] " << std::endl;
//	////std::cout << msg->DebugString();
//	//std::cout << "*******************************************************************[  End  ]" << std::endl << std::endl;
//
//	//m_model->showPackLog(PACKET_DEF::A_Send, QString::fromStdString(msg->GetTypeName()));
//
//	SendToProbDll(msg);
//}

ProbInfo * ProbDllLoader::find_prob_by_game_id(int id)
{
	for (auto p : m_probs)
	{
		if (p->id == id)
			return p;
	}
	return nullptr;
}
//google::protobuf::Message* ProbDllLoader::initSpin(int spin_type)
//{
//	QString spin_name;
//	QString strips_name;
//	QString data_type;
//
//	if (spin_type == ROOL_DEF::T_NgSpin)
//	{
//		spin_name = NG_SPIN_IN_NAME;
//		strips_name = getModel()->getSettingInfo().value(CONFIG_NG_NAME).key(NG_ENTER);
//		data_type = CONFIG_NG_NAME;
//	}
//	else if (spin_type == ROOL_DEF::T_FgSpin)
//	{
//		spin_name = FG_SPIN_IN_NAME;
//		strips_name = getModel()->getSettingInfo().value(CONFIG_FG_NAME).key(FG_ENTER);
//		data_type = CONFIG_FG_NAME;
//	}
//
//	auto prob_name = m_model->getConInfoMap().value(PROB_NAME);
//	auto msg = m_protoM->getMessage(
//		prob_name,
//		QString("%1.%2").arg(prob_name).arg(m_model->getConInfoMap().value(spin_name)));
//
//	for (int i = 0; i < msg->GetDescriptor()->field_count(); ++i)
//	{
//		if (msg->GetDescriptor()->field(i)->name() != strips_name.toStdString())
//		{
//			autoComposeProto(
//				msg,
//				msg->GetDescriptor()->field(i)->type(),
//				msg->GetDescriptor()->field(i)->name(),
//				getModel()->getSettingInfo().value(data_type).value(QString::fromStdString(msg->GetDescriptor()->field(i)->name())));
//		}
//	}
//
//	return msg;
//}
//
//QMap<QString, int> ProbDllLoader::getProbNameMap()
//{
//	return m_pNameNo;
//}

void ProbDllLoader::SendToProbDll(google::protobuf::Message * _in_msg)
{
	auto InDll = find_prob_by_game_id(cur_id);

	if (InDll)
	{
		//message CmdPack
		//{
		//	required string type = 1;
		//	required bytes data = 2;
		//}

		// CmdPack
		auto CmdPack = m_protoM->getMessage("Base", "Base.CmdPack");
		//required string type = 1;
		m_protoM->setItem(CmdPack, "type", _in_msg->GetDescriptor()->full_name());
		//required bytes data = 2;
		m_protoM->setItem(CmdPack, "data", _in_msg->SerializeAsString());
		
		qDebug() << QString("***[send] prob %1").arg(QString::fromStdString(_in_msg->GetDescriptor()->full_name()));
		qDebug() << QString::fromStdString(_in_msg->DebugString());

		///> ProbIn
		InDll->Prob_In(0, CmdPack->SerializeAsString().data(), CmdPack->ByteSize());
	}
	else
		qDebug() << "Prob_In error!";
}

// 所有機率回乎介面
void ProbDllLoader::out(int no, int game_id, const char *begin, int size)
{
	auto cmd = m_protoM->getMessage("Base", "Base.CmdPack");

	if (cmd->ParsePartialFromArray(begin, size))
	{
		QString type = QString::fromStdString(m_protoM->getItemString(cmd, "type"));
		qDebug() << QString("***[accept] prob %1").arg(type);
		
		if (type == "SGT.Prob.FiftyLions.CmdOut_NgSpin")
		{
			auto out_prob = m_protoM->getMessage("SGT.Prob.FiftyLions", type);
			if (out_prob->ParsePartialFromString(m_protoM->getItemString(cmd, "data")))
			{
				qDebug() << QString::fromStdString(out_prob->DebugString());
				emit onProb_out(out_prob);
			}
		}
	}

	//QString str_name = m_model->getConInfoMap().value(QString(PROB_NAME));
	//QString str_ngSpin = QString("%1.%2").arg(str_name).arg(m_model->getConInfoMap().value(NG_SPIN_OUT_NAME));
	//QString str_fgSpin = QString("%1.%2").arg(str_name).arg(m_model->getConInfoMap().value(FG_SPIN_OUT_NAME));

	//auto o_ = m_protoM->getMessage("Base", "Base.CmdPack");
	//if (o_->ParsePartialFromArray(begin, size))
	//{
	//	//qDebug() << QString("success accept %1").arg(QString::fromStdString(m_protoM->getItemString(o_, "type")));
	//}

	////m_model->showPackLog(PACKET_DEF::A_Accept, QString::fromStdString(m_protoM->getItemString(o_, "type")));

	//if (m_protoM->getItemString(o_, "type") == str_ngSpin.toStdString())
	//{
	//	auto o_prob = m_protoM->getMessage(str_name, str_ngSpin);
	//	if (o_prob->ParsePartialFromString(m_protoM->getItemString(o_, "data")))
	//	{
	//		if (m_model->IsNgNeedInit())
	//		{
	//			//do init
	//			initStrips(o_prob, ROOL_DEF::T_NgSpin);
	//			getModel()->adjNeedInit();
	//		}
	//		else
	//		{
	//			//do normal
	//			emit prob_out_signal(o_prob);
	//		}
	//	}
	//}
	//else if (m_protoM->getItemString(o_, "type") == str_fgSpin.toStdString())
	//{
	//	auto o_prob = m_protoM->getMessage(str_name, str_fgSpin);
	//	if (o_prob->ParsePartialFromString(m_protoM->getItemString(o_, "data")))
	//	{
	//		if (m_model->IsFgNeedInit())
	//		{
	//			//do init
	//			initStrips(o_prob, ROOL_DEF::T_FgSpin);
	//			getModel()->adjNeedInit();
	//		}
	//		else
	//		{
	//			//do normal
	//			emit prob_out_signal(o_prob);
	//		}
	//	}
	//}
}

QMap<QString, int> ProbDllLoader::getProbs()
{
	QMap<QString, int> map;

	for (auto p : m_probs)
	{
		map.insert(p->name, p->id);
	}
	return map;
}

void ProbDllLoader::setCurId(int id)
{ cur_id = id; }

int ProbDllLoader::getCurId()
{ return cur_id; }

#include "ProbDllLoader.moc"