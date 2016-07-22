#pragma once
#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <google/protobuf/message.h>

class ProbDllLoader;
class protoManager;
class QVariant;

class PDTModel : public QObject
{
	Q_OBJECT
public:
	PDTModel();
	~PDTModel();

	void Spin(int spin_type, int debug_reel_type = -1); // �o�� prob �� spin ����.
	void clearStripsMgr();	// clear m_stripsMgr
	
	QMap<int, QMap<QString, QVariant>> getSetInfo(); // ���o��e�C����setInfo��T.
	void setSettings(QMap<int, QMap<QString, QVariant>> settings); // update m_settings.
	QMap<int, QMap<QString, QVariant>> getSettings(); // get m_settings.
	QMap<int, QMap<int, QMap<int, QMap<int, int>>>> getStripsMgr(); // get m_stripsMgr.

	// get ; set;
	ProbDllLoader* getLoader();
	void setInitOK(bool b);
	bool getInitOK();

private:
	void saveRestore(QMap<int, QMap<QString, QVariant>> data);
	void setProtoFeild(google::protobuf::Message* msg, QString name, int type, QVariant data/*, bool reels = false*/);

protected slots:
	void slot_onProb_out(google::protobuf::Message* msg);

private:
	QString store_path;	// store�ɸ��|
	QString proto_path; // proto�ɸ��|
	bool initOK;	// �O�_�w�g��l������������a

	// Mgr 
	QMap<int, QString> m_protoNames;				// ����prob�ι�����id 
	QMap<int, QMap<QString, QVariant>> m_settings;	// ��e��ܪ�prob����proto��field���e
	QMap<int, QMap<int, QMap<int, QMap<int, int>>>> m_stripsMgr;// NG/FG, �j�p��, �ĴX��, data

	// Tools
	protoManager *m_protoMgr;
	ProbDllLoader *m_loader;
};

