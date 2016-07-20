/****************************************************************/
/* [���o�h��proto��]											*/
/*	Ū�J.ini��(proto���|), �s�o�sĶ�᪺����Message(by Proto).	*/
/*																*/
/*											2016.03.08 Eddie	*/
/****************************************************************/

#pragma once

#include "ProtoMethod.h"
#include <QtCore\qmap.h>

class dynamicProtoFactory;
class protoManager : public ProtoMethod
{
public:
	// path: the proto file's path collect.     ex: C:/......./protoCollect.ini
	protoManager(QString iniPath);
	~protoManager();

	// get m_protoMgr by protoName and descriptor name.
	google::protobuf::Message* getMessage(QString protoName, QString typeName);

protected:
	// open ini file and set proto member.
	void importProto(QString iniPath);

	// set proto to qmap.
	void setProtoMap(dynamicProtoFactory*);

private:
	// proto map.
	QMap<QString, dynamicProtoFactory*> m_protoMgr;
};

