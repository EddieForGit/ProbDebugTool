/****************************************************************/
/* Message的一些方法及功能都包在這.								*/
/*																*/
/*											2016.03.08 Eddie	*/
/****************************************************************/
// 支援 bool, int32, int64, string, message 型態.

//*********** note *************
// * RepeatedField :
//		is used to represent repeated fields of a primitive type (in other words, everything except strings and nested Messages).
// * RepeatedPtrField :
//		is like RepeatedField, but used for repeated strings or Messages.

#pragma once
#include <google/protobuf/message.h>
#include <QtCore\qvariant.h>
#include <QtCore\qdebug.h>
#include <google/protobuf/descriptor.pb.h>

class ProtoMethod
{
public:
	ProtoMethod();
	~ProtoMethod();

	// set message
	template<typename T>
	void setItem(google::protobuf::Message* msg, std::string field_name, T data);
	void setItem(google::protobuf::Message* msg, std::string field_name, std::string data);
	void setItem(google::protobuf::Message* msg, std::string field_name, google::protobuf::Message* data);

	// add repeated
	template<typename T>
	void addRepeated(google::protobuf::Message* msg, std::string field_name, T data);
	void addRepeated(google::protobuf::Message* msg, std::string field_name, std::string data);
	void addRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::Message* data);

	// set repeated
	template<typename T>
	void setRepeated(google::protobuf::Message* msg, std::string field_name, __int32 Idx, T data);
	void setRepeated(google::protobuf::Message* msg, std::string field_name, __int32 Idx, std::string data);
	void setRepeated(google::protobuf::Message* msg, std::string field_name, __int32 Idx, google::protobuf::Message* data);

	// get item
	bool						getItemBool(google::protobuf::Message * m, std::string n);
	google::protobuf::int32		getItemInt32(google::protobuf::Message * m, std::string n);
	google::protobuf::int64		getItemInt64(google::protobuf::Message * m, std::string n);
	google::protobuf::string	getItemString(google::protobuf::Message * m, std::string n);
	google::protobuf::Message*	getItemMessage(google::protobuf::Message * m, std::string n);
	//QVariant getMessageItem(google::protobuf::Message* msg, std::string field_name);
	//google::protobuf::Message* getItemMessage(google::protobuf::Message* msg, std::string field_name);

	// get repeated
	google::protobuf::RepeatedField<bool>*							getRepeatedBool(google::protobuf::Message* msg, std::string field_name);
	google::protobuf::RepeatedField<__int32>*						getRepeatedInt32(google::protobuf::Message* msg, std::string field_name);
	google::protobuf::RepeatedField<__int64>*						getRepeatedInt64(google::protobuf::Message* msg, std::string field_name);
	google::protobuf::RepeatedPtrField<google::protobuf::string>*	getRepeatedString(google::protobuf::Message* msg, std::string field_name);
	google::protobuf::RepeatedPtrField<google::protobuf::Message>*	getRepeatedMessage(google::protobuf::Message* msg, std::string field_name);

	// copy repeated
	template<typename T>
	void copyRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::RepeatedField<T>* source);
	void copyRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::RepeatedPtrField<google::protobuf::string>* source);
	void copyRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::RepeatedPtrField<google::protobuf::Message>* source);

private:

};

template<typename T>
void ProtoMethod::setItem(google::protobuf::Message* msg, std::string field_name, T data)
{
	if (msg == nullptr || field_name.empty()) return;

	QVariant _d(data);
	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	auto type = field->type();

	switch (type)
	{
	case google::protobuf::FieldDescriptor::TYPE_BOOL:
		msg->GetReflection()->SetBool(msg, field, _d.toBool());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT32:
		msg->GetReflection()->SetInt32(msg, field, _d.toInt());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT64:
		msg->GetReflection()->SetInt64(msg, field, _d.toInt());
		break;
	//case google::protobuf::FieldDescriptor::TYPE_STRING:
	//	msg->GetReflection()->SetString(msg, field, _d.toString().toStdString());
	//	break;
	default:
		qDebug() << QString(" %1 set message error, field type is not exist. ").arg(QString::fromStdString(field_name));
		break;
	}
}

template<typename T>
void ProtoMethod::addRepeated(google::protobuf::Message* msg, std::string field_name, T data)
{
	if (msg == nullptr || field_name.empty()) return;

	QVariant _d(data);
	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	auto type = field->type();

	switch (type)
	{
	case google::protobuf::FieldDescriptor::TYPE_BOOL:
		msg->GetReflection()->AddBool(msg, field, _d.toBool());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT32:
		msg->GetReflection()->AddInt32(msg, field, _d.toInt());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT64:
		msg->GetReflection()->AddInt64(msg, field, _d.toInt());
		break;
	//case google::protobuf::FieldDescriptor::TYPE_STRING:
	//	msg->GetReflection()->AddString(msg, field, _d.toString().toStdString());
	//	break;
	default:
		qDebug() << "add repeated error, field type is not exist. ";
		break;
	}
}

template<typename T>
void ProtoMethod::setRepeated(google::protobuf::Message* msg, std::string field_name, __int32 Idx, T data)
{
	if (msg == nullptr || field_name.empty()) return;

	QVariant _d(data);
	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	auto type = field->type();

	switch (type)
	{
	case google::protobuf::FieldDescriptor::TYPE_BOOL:
		msg->GetReflection()->SetRepeatedBool(msg, field, Idx, _d.toBool());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT32:
		msg->GetReflection()->SetRepeatedInt32(msg, field, Idx, _d.toInt());
		break;
	case google::protobuf::FieldDescriptor::TYPE_INT64:
		msg->GetReflection()->SetRepeatedInt64(msg, field, Idx, _d.toInt());
		break;
	//case google::protobuf::FieldDescriptor::TYPE_STRING:
	//	msg->GetReflection()->SetRepeatedString(msg, field, Idx, _d.toString().toStdString());
	//	break;
	default:
		qDebug() << "Set repeated error, field type is not exist. ";
		break;
	}
}

template<typename T>
void ProtoMethod::copyRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::RepeatedField<T>* source)
{
	if (msg == nullptr || field_name.empty() || source->empty()) return;

	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	auto tmp = msg->GetReflection()->MutableRepeatedField<T>(msg, field);
	tmp->Clear();
	tmp->CopyFrom(*source);	
}