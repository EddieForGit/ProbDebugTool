#include "ProtoMethod.h"
#include <google/protobuf/descriptor.pb.h>

//#include <iostream>

ProtoMethod::ProtoMethod(){}
ProtoMethod::~ProtoMethod(){}

void ProtoMethod::setItem(google::protobuf::Message * msg, std::string field_name, std::string data)
{
	if (msg == nullptr || field_name.empty() || data.empty()) return;
	msg->GetReflection()->SetString(msg, msg->GetDescriptor()->FindFieldByName(field_name), data);
}

void ProtoMethod::setItem(google::protobuf::Message* msg, std::string field_name, google::protobuf::Message* data)
{
	if (msg == nullptr || field_name.empty() || data == nullptr) return;
	auto sm = msg->GetReflection()->MutableMessage(msg, msg->GetDescriptor()->FindFieldByName(field_name));
	sm->Clear();
	sm->CopyFrom(*data);
}

void ProtoMethod::addRepeated(google::protobuf::Message * msg, std::string field_name, std::string data)
{
	if (msg == nullptr || field_name.empty() || data.empty()) return;

	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	msg->GetReflection()->AddString(msg, field, data);
}

void ProtoMethod::addRepeated(google::protobuf::Message* msg, std::string field_name, google::protobuf::Message* data)
{
	if (msg == nullptr || field_name.empty() || data == nullptr) return;
	msg->GetReflection()->AddMessage(msg, msg->GetDescriptor()->FindFieldByName(field_name))->CopyFrom(*data);
}

void ProtoMethod::setRepeated(google::protobuf::Message * msg, std::string field_name, __int32 Idx, std::string data)
{
	if (msg == nullptr || field_name.empty() || data.empty()) return;

	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	msg->GetReflection()->SetRepeatedString(msg, field, Idx, data);
}

void ProtoMethod::setRepeated(google::protobuf::Message* msg, std::string field_name, __int32 Idx, google::protobuf::Message* data)
{
	if (msg == nullptr || field_name.empty() || data == nullptr) return;
	auto sm = msg->GetReflection()->MutableRepeatedMessage(msg, msg->GetDescriptor()->FindFieldByName(field_name), Idx);
	sm->Clear();
	sm->CopyFrom(*data);
}

//QVariant ProtoMethod::getMessageItem(google::protobuf::Message * msg, std::string field_name)
//{
//	if (msg == nullptr || field_name.empty()) return QVariant();
//
//	QVariant _d;
//	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
//	auto type = field->type();
//
//	switch (type)
//	{
//	case google::protobuf::FieldDescriptor::TYPE_BOOL:
//		_d = QVariant(msg->GetReflection()->GetBool(*msg, field));
//		break;
//	case google::protobuf::FieldDescriptor::TYPE_INT32:
//		_d = QVariant(msg->GetReflection()->GetInt32(*msg, field));
//		break;
//	case google::protobuf::FieldDescriptor::TYPE_INT64:
//		_d = QVariant(msg->GetReflection()->GetInt64(*msg, field));
//		break;
//	case google::protobuf::FieldDescriptor::TYPE_STRING:
//		_d = QVariant(QString::fromStdString(msg->GetReflection()->GetString(*msg, field)));
//		break;
//	default:
//		qDebug() << "get message error, field type is not exist. ";
//		break;
//	}
//
//	return _d;
//}



bool ProtoMethod::getItemBool(google::protobuf::Message * m, std::string n)
{
	if (m == nullptr || n.empty()) return false;
	return m->GetReflection()->GetBool(*m, m->GetDescriptor()->FindFieldByName(n));
}

google::protobuf::int32 ProtoMethod::getItemInt32(google::protobuf::Message * m, std::string n)
{
	if (m == nullptr || n.empty()) return 0;
	return m->GetReflection()->GetInt32(*m, m->GetDescriptor()->FindFieldByName(n));
}

google::protobuf::int64 ProtoMethod::getItemInt64(google::protobuf::Message * m, std::string n)
{
	if (m == nullptr || n.empty()) return 0;
	return m->GetReflection()->GetInt64(*m, m->GetDescriptor()->FindFieldByName(n));
}

google::protobuf::string ProtoMethod::getItemString(google::protobuf::Message * m, std::string n)
{
	if (m == nullptr || n.empty()) return "";
	return m->GetReflection()->GetString(*m, m->GetDescriptor()->FindFieldByName(n));
}

google::protobuf::Message* ProtoMethod::getItemMessage(google::protobuf::Message * m, std::string n)
{
	if (m == nullptr || n.empty()) return nullptr;
	return m->GetReflection()->MutableMessage(m, m->GetDescriptor()->FindFieldByName(n));
}

google::protobuf::RepeatedField<bool>* ProtoMethod::getRepeatedBool(google::protobuf::Message * msg, std::string field_name)
{
	if (msg == nullptr || field_name.empty()) return nullptr;
	return msg->GetReflection()->MutableRepeatedField<bool>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
}

google::protobuf::RepeatedField<__int32>* ProtoMethod::getRepeatedInt32(google::protobuf::Message * msg, std::string field_name)
{
	if (msg == nullptr || field_name.empty()) return nullptr;
	return msg->GetReflection()->MutableRepeatedField<__int32>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
}

google::protobuf::RepeatedField<__int64>* ProtoMethod::getRepeatedInt64(google::protobuf::Message * msg, std::string field_name)
{
	if (msg == nullptr || field_name.empty()) return nullptr;
	return msg->GetReflection()->MutableRepeatedField<__int64>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
}

google::protobuf::RepeatedPtrField<google::protobuf::string>* ProtoMethod::getRepeatedString(google::protobuf::Message * msg, std::string field_name)
{
	if (msg == nullptr || field_name.empty()) return nullptr;
	return msg->GetReflection()->MutableRepeatedPtrField<google::protobuf::string>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
}

google::protobuf::RepeatedPtrField<google::protobuf::Message>* ProtoMethod::getRepeatedMessage(google::protobuf::Message * msg, std::string field_name)
{
	if (msg == nullptr || field_name.empty()) return nullptr;
	return msg->GetReflection()->MutableRepeatedPtrField<google::protobuf::Message>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
}

void ProtoMethod::copyRepeated(google::protobuf::Message * msg, std::string field_name, google::protobuf::RepeatedPtrField<google::protobuf::string>* source)
{
	if (msg == nullptr || field_name.empty() || source->empty()) return;
	auto tmp = msg->GetReflection()->MutableRepeatedPtrField<google::protobuf::string>(msg, msg->GetDescriptor()->FindFieldByName(field_name));
	tmp->Clear();
	tmp->CopyFrom(*source);
}

void ProtoMethod::copyRepeated(google::protobuf::Message * msg, std::string field_name, google::protobuf::RepeatedPtrField<google::protobuf::Message>* source)
{
	if (msg == nullptr || field_name.empty() || source->empty()) return;
	auto field = msg->GetDescriptor()->FindFieldByName(field_name);
	auto tmp = msg->GetReflection()->MutableRepeatedPtrField<google::protobuf::Message>(msg, field);
	tmp->Clear();
	for (int i = 0; i < source->size(); ++i)
		msg->GetReflection()->AddMessage(msg, field)->CopyFrom(source->Get(i));	
}