/****************************************************************/
/* [讀取單一proto檔案]											*/
/* 讀入proto路徑及名稱, 存得編譯後的全部Message(by Proto).		*/
/* -protoPathLst- 包含improt路徑									*/
/*																*/
/*											2016.03.08 Eddie	*/
/****************************************************************/

#pragma once

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

#include <QtCore/qmap.h>

class dynamicProtoFactory
{
public:
	dynamicProtoFactory(QStringList protoPathLst, std::string protoName);
	~dynamicProtoFactory();

	// get message by descriptor name.
	google::protobuf::Message* getMessage(QString type);

	// get proto name.
	std::string getPackName();

protected:
	// set source tree map paths.
	void setTreeMapPath(google::protobuf::compiler::DiskSourceTree &sourceTree, QStringList protoPathLst);

	// create message.
	bool createMessage();

	// set all Messages to Map.
	void setMessage(google::protobuf::Message *message);

	// get m_messageMgr.
	QMap<QString, google::protobuf::Message*> getMessageMgr();

private:
	// proto name.
	std::string m_packName;

	// FileDescriptor.
	const google::protobuf::FileDescriptor* m_pFileDescriptor;

	google::protobuf::DynamicMessageFactory m_factory;

	// message map.
	QMap<QString, google::protobuf::Message*> m_messageMgr;
};

