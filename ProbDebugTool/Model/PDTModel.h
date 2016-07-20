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

	enum Type_SetInfo {
		// Model&View須同步
		T_Con = 0,
		T_Ng = 1,
		T_Fg = 2,

		T_count = 3
	};

#define REEL_SIZE_X		"ReelSizeX"
#define REEL_SIZE_Y		"ReelSizeY"
#define STRIPS_NAME		"StripsName"
#define REELS_NAME		"ReelsName"
#define STRIP_COUNT_NG	"NgStrip'Count"
#define STRIP_COUNT_FG	"FgStrip'Count"
#define NG_SPIN			"CmdIn_NgSpin"
#define FG_SPIN			"CmdIn_FgSpin"


public:
	PDTModel();
	~PDTModel();

	ProbDllLoader* getLoader();
	QMap<int, QMap<QString, QVariant>> getSetInfo(); // 取得當前遊戲的setInfo資訊
	void setSettings(QMap<int, QMap<QString, QVariant>> settings);
	void initStrips(); // 發給prob初始spin, 取得init轉輪表.

private:
	void saveRestore(QMap<int, QMap<QString, QVariant>> data);
	void setProtoFeild(google::protobuf::Message* msg, QString name, int type, QVariant data, bool reels = false);

protected slots:
	void slot_onProb_out(google::protobuf::Message* msg);

private:
	QString store_path;
	QString proto_path;

	QMap<int, QString> m_protoNames;
	QMap<int, QMap<QString, QVariant>> m_settings;

	protoManager *m_protoMgr;
	ProbDllLoader *m_loader;
};

