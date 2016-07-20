#pragma once
#include <QtCore\qobject.h>
#include <QtCore\qmap.h>
#include <google\protobuf\message.h>

struct ProbInfo;
class protoManager;

class ProbDllLoader : public QObject
{
	Q_OBJECT
public:
	ProbDllLoader(protoManager*);
	~ProbDllLoader();

	// prob_in interface.
	void SendToProbDll(google::protobuf::Message* _in_msg);

	// prob_out interface.
	void out(int no, int game_id, const char *begin, int size);
	

	QMap<QString, int> getProbs();
	void setCurId(int id);
	int getCurId();

	//// set prob_in reels.
	//void setInReels(QList<int> strips, google::protobuf::Message* msg, int spin_type);
	//void setInStripsType(int strip_type, google::protobuf::Message* msg, int spin_type);
	//void sendProb(google::protobuf::Message* msg);

	//// init spin message's item except the item's vaule is "ENTER" from config.ini.
	//google::protobuf::Message* initSpin(int spin_type);

	//// get prob's name map
	//QMap<QString, int> getProbNameMap();

signals:
	//void prob_out_signal(google::protobuf::Message*);
	void onProb_out(google::protobuf::Message*);

private:
	void initProbFromDll(QString path);
	
	//// auto compose proto item, type is message item's type, name is message item's name.
	//void autoComposeProto(
	//	google::protobuf::Message *msg, 
	//	int type,
	//	std::string name,
	//	QVariant data);

	ProbInfo* find_prob_by_game_id(int id);
	
private:

	int cur_id;
	QMap<int, ProbInfo *> m_probs;

	//tool
	protoManager* m_protoM;

};

