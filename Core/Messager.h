// Messager.h: interface for the Messager class.
#ifndef MYENGINE_MESSAGER_H
#define MYENGINE_MESSAGER_H

#include "types.h"
#include "rapidxml/rapidxml.hpp"

#include <string>
#include <list>

//
// наследуемся от этого класса, если хотим обрабатывать сообщения
//

class Messager
{
public:

	// переопределяем этот метод обработки сообщений
	virtual void OnMessage(const std::string &message){};

	virtual std::string GetMyName(){return _name;};

	// определяем свое имя получателя через конструктор ...
	Messager(std::string receiverName);
	// ... или имя должно быть задано в xml так <messager name="[ имя получателя ]"/>
	Messager(rapidxml::xml_node<> *xe);

	~Messager();
	static void SendMessage(const std::string &receiverName, const std::string &message, float delay = 0.f);

	static bool CanCut(const std::string &message, const std::string &substr, std::string &result);

private:
	typedef std::list<Messager *> List;
	static List _receiver;
	std::string _name;
	struct Letter{
		std::string receiver;
		std::string message;
		float timer;
	};
	typedef std::list<Letter *> AllMessages;
	static AllMessages _incoming;
	static AllMessages _messages;
	static void CoreSendMessages(float dt); // только для вызова из ядра
	friend class Core;
};

class MessageReader
{
private:
	std::string _receiver;
	std::string _text;
public:
	MessageReader(rapidxml::xml_node<> *xe)
		: _receiver(xe->first_attribute("receiver")->value())
		, _text(xe->first_attribute("text")->value())
	{}
	void Send()
	{
		Messager::SendMessage(_receiver, _text);
	}
};

#endif//MYENGINE_MESSAGER_H
