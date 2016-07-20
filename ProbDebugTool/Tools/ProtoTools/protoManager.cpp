#include "protoManager.h"
#include "dynamicProtoFactory.h"
#include <QtCore\qsettings.h>
#include <iostream>

protoManager::protoManager(QString iniPath) : ProtoMethod()
{
	m_protoMgr.clear();

	// 輸入ini檔路徑
	importProto(iniPath);
}

protoManager::~protoManager()
{
	qDeleteAll(m_protoMgr);
	m_protoMgr.clear();
}

google::protobuf::Message* protoManager::getMessage(QString protoName, QString typeName)
{
	google::protobuf::Message *m = nullptr;

	if (m_protoMgr.contains(protoName))
	{
		if (m_protoMgr[protoName]->getMessage(typeName) != nullptr)
		{
			m = m_protoMgr[protoName]->getMessage(typeName);
		}
	}

	return m;
}

void protoManager::importProto(QString iniPath)
{
	if (iniPath.isEmpty()) return;

	QSettings sr(iniPath, QSettings::IniFormat);
	
	if (sr.allKeys().size() > 0 && sr.childGroups().size() > 0)
	{
		for (auto groupName : sr.childGroups())
		{
			QString fileName(groupName);
			QStringList filePahts;

			// read .ini file
			sr.beginGroup(groupName);
			for (int i = 0; i < sr.allKeys().size(); ++i)
			{
				filePahts.append(sr.value(sr.allKeys().at(i)).toString());
			}
			sr.endGroup();

			// create dynamicProtoFactory
			dynamicProtoFactory *member = 
				new dynamicProtoFactory(filePahts, fileName.toStdString());// 輸入ini檔內的proto檔名, 路徑

			// save dynamicProtoFactory
			if (member->getPackName() != "")
			{
				setProtoMap(member);
			}
		}
	}
	else
		std::cout << "ini file is null." << std::endl;
}

void protoManager::setProtoMap(dynamicProtoFactory *p)
{
	QString key = QString::fromStdString(p->getPackName());

	if (m_protoMgr.contains(key))
	{
		auto *pOld = m_protoMgr[key];

		if (pOld != nullptr)
		{
			delete pOld;
			pOld = nullptr;
		}
	}

	m_protoMgr.insert(key, p);
}
