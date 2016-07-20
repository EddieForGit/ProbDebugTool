#include <iostream>
#include "dynamicProtoFactory.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

dynamicProtoFactory::dynamicProtoFactory(QStringList protoPathLst, std::string protoName) :
	m_pFileDescriptor(nullptr),
	m_packName("")
{
	m_messageMgr.clear();
	m_packName.clear();
	
	// <1> look up .proto file in current directory and compile it.
	google::protobuf::compiler::DiskSourceTree sourceTree;
	setTreeMapPath(sourceTree, protoPathLst); // set source tree paths.
	google::protobuf::compiler::Importer *m_importer = 
		new google::protobuf::compiler::Importer(&sourceTree, NULL); // get importer. (MultiFileErrorCollector set NULL)
	m_pFileDescriptor = m_importer->Import(protoName); // to get FileDescriptor for runtime compile [protoName] by importer.

	if (m_pFileDescriptor != nullptr)
	{
		// <2> create all message.
		if (createMessage())
		{
			m_packName = m_pFileDescriptor->package();
			std::cout << "create Proto " << protoName << " OK." << std::endl;
		}
		else 
			std::cout << "create Proto " << protoName << " is error!" << std::endl;
	}
	else 
		std::cout << protoName << " read error. " << sourceTree.GetLastErrorMessage() << std::endl;
}

dynamicProtoFactory::~dynamicProtoFactory()
{
	delete m_pFileDescriptor;

	//deletes all the values stored in "map"
	qDeleteAll(m_messageMgr);

	//removes all items from the map
	m_messageMgr.clear();
}

google::protobuf::Message * dynamicProtoFactory::getMessage(QString type)
{
	google::protobuf::Message * out_Message = nullptr;

	if (getMessageMgr().size() == 0)
	{
		std::cout << "getMessage error, the MessageMgr is null." << std::endl;
	}
	else if (!m_messageMgr.contains(type))
	{
		std::cout << "getMessage error, the MessageMar is not contains : " << type.toStdString() << std::endl;
	}
	else
	{
		auto msg = m_messageMgr[type];
		msg->Clear();
		out_Message = m_messageMgr[type];
	}

	return out_Message;
}

std::string dynamicProtoFactory::getPackName()
{
	return m_packName;
}

bool dynamicProtoFactory::createMessage()
{
	bool CMok(true);

	if (m_pFileDescriptor->message_type_count() > 0)
	{
		for (int i = 0; i < m_pFileDescriptor->message_type_count(); ++i)
		{
			// get descriptor
			const google::protobuf::Descriptor *descriptor = m_pFileDescriptor->message_type(i);
			if (descriptor != NULL)
			{
				// build a dynamic message by [msgName] proto
				const google::protobuf::Message *message = 
					const_cast<google::protobuf::Message*>(m_factory.GetPrototype(descriptor));

				// create a real instance of [msgName]
				google::protobuf::Message *realMessage = message->New();

				// <3> set all Messages to Map
				setMessage(realMessage);
			}
			else 
				CMok = false;
		}
	}
	else
	{
		std::cout << "allDescriptor's message type count error!" << std::endl;
		CMok = false;
	}
	
	return CMok;
}

QMap<QString, google::protobuf::Message*> dynamicProtoFactory::getMessageMgr()
{
	return m_messageMgr;
}

void dynamicProtoFactory::setTreeMapPath(google::protobuf::compiler::DiskSourceTree &sourceTree, QStringList protoPathLst)
{
	if (!protoPathLst.empty())
	{
		for (int i = 0; i < protoPathLst.size(); ++i)
		{
			sourceTree.MapPath("", protoPathLst.at(i).toStdString());
		}
	}
}

void dynamicProtoFactory::setMessage(google::protobuf::Message *message)
{
	if (message == nullptr) return;
	
	QString key = QString::fromStdString(message->GetTypeName());
	if (getMessageMgr().contains(key)) return;

	//if (m_messageMgr.contains(key))
	//{
	//	const google::protobuf::Message *pOld = m_messageMgr[key];
	//	if (pOld != nullptr)
	//	{
	//		delete pOld;
	//		pOld = nullptr;
	//	}
	//}

	m_messageMgr.insert(key, message);
	//std::cout << message->GetTypeName() << std::endl;
}