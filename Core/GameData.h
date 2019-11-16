#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "types.h"

#include <assert.h>

#include <map>
#include <string>

struct IntServer
{
	int paidValue;
	int freeValue;
};

class GameData
{
protected:
	static std::map<std::string, std::string> stringData;
	static std::map<std::string, int> intData;
    static std::map<std::string, IntServer> intServerData;
    static bool _firstStart;
    static bool _wasUpdate;
public:
    
    static bool FirstStart() { return _firstStart; }
    static void SetFirstStart(bool b) { _firstStart = b; }

	static void Buy(const std::string &moneyName, int price, const std::string &varName, int value)
	{
        assert(int_server_var(moneyName) >= price);
        std::map<std::string, IntServer>::iterator moneyIter = intServerData.find(moneyName);
        std::map<std::string, IntServer>::iterator varIter = intServerData.find(varName);
        assert(moneyIter != intServerData.end());
        if (varIter == intServerData.end())
        {
            int_server_create(varName, 0, 0);
            varIter = intServerData.find(varName);
        }

        
        int subFree = moneyIter->second.freeValue - price;
        int subPaid = moneyIter->second.paidValue;
        
        if (subFree < 0)
        {
            subPaid = moneyIter->second.paidValue + subFree;
            subFree = 0;
        }
        
        int addFree = moneyIter->second.freeValue - subFree;
        int addPaid = moneyIter->second.paidValue - subPaid;
        
        moneyIter->second.freeValue -= price;
        if (moneyIter->second.freeValue < 0)
        {
            moneyIter->second.paidValue += moneyIter->second.freeValue;
            moneyIter->second.freeValue = 0;
			
            varIter->second.paidValue += value;
        }
        else
        {
            varIter->second.freeValue += value;
        }
    }
	static void Sub(const std::string &name, int price)
	{
        price = std::min(price, int_server_var(name));
        
        assert(int_server_var(name) >= price);
		
        std::map<std::string, IntServer>::iterator moneyIter = intServerData.find(name);
        assert(moneyIter != intServerData.end());

        int subFree = moneyIter->second.freeValue - price;
        int subPaid = moneyIter->second.paidValue;
        
        if (subFree < 0)
        {
            subPaid = moneyIter->second.paidValue + subFree;
            subFree = 0;
        }
        
        int addFree = subFree - moneyIter->second.freeValue;
        int addPaid = subPaid - moneyIter->second.paidValue;
        
        AddValues(name, addPaid, addFree);
    }
	static void AddValues(const std::string &name, int pVal, int fVal)
	{
        std::map<std::string, IntServer>::iterator moneyIter = intServerData.find(name);
        assert(moneyIter != intServerData.end());

        moneyIter->second.paidValue += pVal;
        moneyIter->second.freeValue += fVal;
        
        if (pVal != 0 || fVal != 0)
        {
        }
    }
	static void int_server_create(const std::string &name, int valuePaid, int valueFree)
	{
        std::map<std::string, IntServer>::iterator iter = intServerData.find(name);

        if (iter == intServerData.end())
        {
            intServerData[name].paidValue = 0;
            intServerData[name].freeValue = 0;
        }
        else
        {
            valuePaid -= iter->second.paidValue;
            valueFree -= iter->second.freeValue;
        }
        AddValues(name, valuePaid, valueFree);
    }
    static int int_server_var(const std::string &name)
    {
        std::map<std::string, IntServer>::iterator i = intServerData.find(name);
        assert(i != intServerData.end());
        return i->second.freeValue + i->second.paidValue;
    }
    static int int_server_var_def(const std::string &name, int defValue)
    {
        std::map<std::string, IntServer>::iterator i = intServerData.find(name);
        if (i == intServerData.end())
        {
            return defValue;
        }
        return i->second.freeValue + i->second.paidValue;
    }
    static const IntServer &int_server(const std::string &name)
    {
        assert(int_server_exist(name));
        return intServerData[name];
    }
    static bool int_server_exist(const std::string &name)
    {
        return intServerData.find(name) != intServerData.end();
    }

	static void string_create(const std::string &name, const std::string &value) 
	{
		stringData[name] = value;
	}
	static std::string &string_var(const std::string &name) 
	{
		assert(string_exist(name));
		return stringData[name];
	} 
	static bool string_exist(const std::string &name)
	{
		return stringData.find(name) != stringData.end();
	}
	static void int_create(const std::string &name, int value) 
	{
		intData[name] = value; 
	}
	static int &int_var(const std::string &name) 
	{
		assert(int_exist(name));
		return intData[name];
	}
	static int int_var_def(const std::string &name, int defValue) 
	{
		if (!int_exist(name))
			return defValue;

		return intData[name];
	}
	static bool int_exist(const std::string &name)
	{
		return intData.find(name) != intData.end();
	}
	static void load(bool forceReset = false);
	static void save();
};

struct IntVariableRender
{
private:
	float _time;
	float _timer;
	int _startValue;
public:
	const std::string name;
	std::string strValue;
	int intValue;
	IntVariableRender(const std::string &Name);
	void AddValues(int pVal, int fVal, float time = 0.f);
	void BuyItem(int price, const std::string &var, int value);
	void Sub(int value);
	void Sync();
	void Update(float dt);
};

//void PrintString(cFileBase *file, const std::string &s);

#endif//GAME_DATA_H
