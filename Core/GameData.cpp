#include "GameData.h"
#include "Render.h"
#include "Math.h"
#include "Core.h"
#include "rapidxml/rapidxml_print.hpp"
#include "InputSystem.h"

std::map<std::string, std::string> GameData::stringData;
std::map<std::string, int> GameData::intData;
std::map<std::string, IntServer> GameData::intServerData;

bool GameData::_firstStart = false;
bool GameData::_wasUpdate = false;

void GameData::load(bool forceReset)
{
    std::vector<char> buffer;
    Core::ReadFileToBuffer("../userprofile.xml", buffer);
    if (buffer.empty())
    {
        SetFirstStart(true);
        Core::ReadFileToBuffer("data/defaultprofile.xml", buffer);
        if (buffer.empty())
        {
            return;
        }
    }
    else
    {
        SetFirstStart(false);
    }

    if (buffer.size())
    { //    -  XML
        rapidxml::xml_document<> doc;

        try
        {
            doc.parse<0>(&buffer[0]);
        }
        catch (...)
        {
            if (!forceReset)
                load(true);
            return;
        }

		rapidxml::xml_node<> *root = doc.first_node();
		rapidxml::xml_node<> *element = root->first_node();
		std::string name;
		std::string type;
		while (element != NULL) {
			type = element->first_attribute("type")->value();
            name = element->first_attribute("name")->value();
			if (type == "string")
			{
				string_create(name, element->first_attribute("value")->value());
			}
			else if (type == "int")
			{
 				int_create(name, atoi(element->first_attribute("value")->value()));
			}
			else if (type == "server")
			{
                std::map<std::string, IntServer>::iterator moneyIter = intServerData.find(name);
                assert(moneyIter == intServerData.end());
                
                intServerData[name].paidValue = atoi(element->first_attribute("paid")->value());
                intServerData[name].freeValue = atoi(element->first_attribute("free")->value());
            }
			else
			{
				assert(false);
			}
			element = element->next_sibling();
		}
	}
	else
	{
        if (!forceReset)
            load(true);
	}
    _wasUpdate = false;
}

//void PrintString(cFileBase *file, const std::string &s)
//{
//    file->write(s.c_str(), s.size() * sizeof(std::string::allocator_type));
//	file->write("\n", strlen("\n"));
//}

void GameData::save()
{
    if (!_wasUpdate)
    {
        return;
    }

    FILE *file = fopen("userprofile.xml", "wt");
    
    if (!file)
    {
        return;
    }

    fprintf(file, "<root>\n");
    
	for (std::map<std::string, std::string>::iterator i = stringData.begin(), e = stringData.end(); i != e; ++i)
	{
        fprintf(file, "  <var type=\"string\" name=\"%s\" value=\"%s\"/>", (*i).first.c_str(), (*i).second.c_str());
	}

	for (std::map<std::string, int>::iterator i = intData.begin(), e = intData.end(); i != e; ++i)
	{
        fprintf(file, "  <var type=\"int\" name=\"%s\" value=\"%i\"/>", (*i).first.c_str(), (*i).second);
    }

    for (std::map<std::string, IntServer>::iterator i = intServerData.begin(), e = intServerData.end(); i != e; ++i)
    {
        fprintf(file, "  <var type=\"server\" name=\"%s\" paid=\"%i\" free=\"%i\"/>", (*i).first.c_str(), (*i).second.paidValue, (*i).second.freeValue);
    }

    fprintf(file, "</root>");
    
    fclose(file);

    _wasUpdate = false;
}

IntVariableRender::IntVariableRender(const std::string &Name)
	: name(Name)
{
	Sync();
}

void IntVariableRender::AddValues(int pVal, int fVal, float time) 
{
    _startValue = intValue;
    GameData::AddValues(name, pVal, fVal);
    intValue = GameData::int_server_var(name);
    if (time > 0.f)
    {
        _time = time;
        _timer = 0.f;
    }
    else
    {
        strValue = Math::IntToStr(intValue);
    }
}

void IntVariableRender::BuyItem(int price, const std::string &var, int value)
{
	GameData::Buy(name, price, var, value);
	Sync();
}

void IntVariableRender::Sub(int value)
{
	GameData::Sub(name, value);
	Sync();
}

void IntVariableRender::Sync() 
{
	_time = _timer = 0.f;
    intValue = GameData::int_server_var(name);
    strValue = Math::IntToStr(intValue);
}

void IntVariableRender::Update(float dt)
{
	if (_time > 0.f)
	{
		_timer += dt;
		if (_timer < _time)
			strValue = Math::IntToStr(Math::lerp(_startValue, intValue, _timer / _time));
		else
		{
			strValue = Math::IntToStr(intValue);
			_time = 0.f;
		}
	}
}

//void GameData::set_server_vars(json_t *parent)
//{
//#ifndef WIN32
//    for (std::map<std::string, IntServer>::iterator i = intServerData.begin(), e = intServerData.end(); i != e; ++i)
//    {
//        json_t* p = json_array();
//        json_array_append_new(p, json_integer((*i).second.paidValue));
//        json_array_append_new(p, json_integer((*i).second.freeValue));
//        json_object_set(parent, (*i).first.c_str(), p);
//        json_decref(p);
//    }
//#endif
//}

//void GameData::update_server_vars(json_t *items)
//{
//#ifndef WIN32
//    void *iter = json_object_iter(items);
//    IntServer is;
//    while (iter)
//    {
//        std::string name = json_object_iter_key(iter);
//        json_t *var = json_object_iter_value(iter);
        
//        is.paidValue = (int)json_integer_value(json_array_get(var, 0));
//        is.freeValue = (int)json_integer_value(json_array_get(var, 1));
        
//        intServerData[name] = is;
        
//        iter = json_object_iter_next(items, iter);
//    }
//#endif
//}

