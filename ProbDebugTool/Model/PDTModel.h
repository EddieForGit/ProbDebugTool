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

	void Spin(int spin_type, int debug_reel_type = -1); // 發給 prob 的 spin 介面.
	void clearStripsMgr();	// clear m_stripsMgr
	
	QMap<int, QMap<QString, QVariant>> getSetInfo(); // 取得當前遊戲的setInfo資訊.
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
	QString store_path;	// store檔路徑
	QString proto_path; // proto檔路徑
	bool initOK;	// 是否已經初始完畢全部轉輪帶

	// Mgr 
	QMap<int, QString> m_protoNames;				// 全部prob及對應的id 
	QMap<int, QMap<QString, QVariant>> m_settings;	// 當前選擇的prob對應proto的field內容
	QMap<int, QMap<int, QMap<int, QMap<int, int>>>> m_stripsMgr;// NG/FG, 大小輪, 第幾輪, data

	// Tools
	protoManager *m_protoMgr;
	ProbDllLoader *m_loader;
};

