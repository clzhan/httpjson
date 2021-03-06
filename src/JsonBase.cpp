﻿#include "JsonBase.h"
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;
enum ReturnType
{
    RET_ERROR = -1,
    RET_OK
};

//声明
void ConvertStr(string& str);
void TrimQuotationMarks(string& str);
void RemoveSpaceAndLineFeed(string& strSource);
bool ContainObject(string strValue, Json::Value &jsonObj);

string ReturnJsonIndexString(Json::Value jObejct, string strValue);
string&   replace_all(string&   str,const   string&   old_value,char new_value);
vector<string> SplitObject(string strNode);
vector<string> GetChildNodList(Json::Value jObejct, string strValue);
vector<string> split(const string& src, string delimit,string null_subst);

vector<string> split(const string& src, string delimit,string null_subst="")
{
    typedef basic_string<char>::size_type S_T;
    static const S_T npos = -1;

    if( src.empty() || delimit.empty() )
        throw "split: empty string\0";

    vector<string> v;
    S_T deli_len = delimit.size();
    unsigned int index = npos, last_search_position = 0;
    while( (index=src.find(delimit, last_search_position))!=npos )
    {
        if(index==last_search_position)
            v.push_back(null_subst);
        else
            v.push_back( src.substr(last_search_position, index-last_search_position) );
        last_search_position = index + deli_len;
    }
    string last_one = src.substr(last_search_position);
    v.push_back( last_one.empty()? null_subst:last_one );
    return v;
}
vector<string> SplitObject(string strNode)
{
    vector<string> vecRet;
    for (size_t i=0; i<strNode.size(); i++)
    {
        if (strNode[i] == '.')
        {
            vecRet.push_back(strNode.substr(0, i));
        }
        else if (strNode[i] == '[')
        {
            string strTemp = strNode.substr(0,i);
            if (strTemp != "")
            {
                vecRet.push_back(strTemp);
            }
        }
    }
    if (strNode != "")
    {
        vecRet.push_back(strNode);
    }
    return vecRet;
}
vector<string> GetChildNodList(Json::Value jObejct, string strValue)
{
    //查找该节点
    vector<string> vecArray = split(strValue,".");

    bool bFlag = true;
    Json::Value jsonObj = jObejct;
    for (size_t i=0; i<vecArray.size(); i++)
    {
        if (!jsonObj.isMember(vecArray[i]))
        {
            bFlag = false;
            break;
        }
        if (i<vecArray.size())
        {
            jsonObj = jsonObj.get(vecArray[i],NULL);
        }
    }
    Json::Value jsonChildNode= jsonObj[vecArray[vecArray.size()-1]];
    return jsonChildNode.getMemberNames();
}
bool JsonBase::ReadJson(string jObect, Json::Value& root)
{
    Json::Reader jsonReader;

    if (!jsonReader.parse(jObect, root))
    {
        cout<<"解析Json数据结构失败!\n";
        return false;
    }
    return true;
}
void RemoveSpaceAndLineFeed(string& strSource)
{
    string strRet;
    for (size_t i=0; i< strSource.size(); i++)
    {
        if (strSource[i] != ' ' && strSource[i] != '\n')
        {
            strRet += strSource[i];
        }
    }
    strSource = strRet;
}

bool JsonBase::GetJsonObject(Json::Value &jsonObj, string strValue)
{
    if (strValue == "")
    {
        return true;
    }

    vector<string> vecArray = split(strValue,".");
    for (size_t i=0; i<vecArray.size(); i++)
    {
        string strKey = vecArray[i];

        int nStart = strKey.find('[');
        int nEnd   = strKey.find(']');

        if (-1 != nStart && -1 != nEnd)
        {
            size_t nPos = -1;
            sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);

            string strTempKey = "";
            if (nStart != 0)
            {
                strTempKey = strKey.substr(0,nStart);
            }

            if (strTempKey != "")
            {
                if (jsonObj.type() != Json::objectValue)
                {
                    cout<<"当前节点不是对象!\n";
                    return false;
                }
                if (!jsonObj.isMember(strTempKey))
                {
                    cout<<"节点不存在!\n";
                    return false;
                }
                if (i<vecArray.size())
                {
                    jsonObj = jsonObj.get(strTempKey,NULL);
                }
            }

            do
            {
                Json::ValueType valueType = jsonObj.type();
                if (valueType == Json::arrayValue)
                {
                    if (nPos < 0)
                    {
                        int nSize = jsonObj.size();
                        //负数下标不能超过size大小
                        if (nPos*(-1) > nSize)
                        {
                            cout<<"节点下标越界!\n";
                            return false;
                        }
                        nPos = nPos % nSize;
                        if (nPos != 0)
                        {
                            nPos += nSize;
                        }
                    }
                    else if (nPos > jsonObj.size()-1)
                    {
                        cout<<"节点下标越界!\n";
                        return false;
                    }

                    jsonObj = jsonObj.get(nPos,NULL);
                }
                else
                {
                    cout<<"节点不是数组!\n";
                    return false;
                }

                strKey = strKey.substr(nEnd+1);
                nStart = strKey.find('[');
                nEnd   = strKey.find(']');

                sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
            } while (-1 != nStart && -1 != nEnd);
        }
        else
        {
            if (jsonObj.type() != Json::objectValue)
            {
                cout<<"节点不是数组!\n";
                return false;
            }
            if (!jsonObj.isMember(strKey))
            {
                cout<<"节点不存在!\n";
                return false;
            }
            if (i<vecArray.size())
            {
                jsonObj = jsonObj.get(strKey,NULL);
            }
        }
    }
    return true;
}
JsonBase::JsonBase(void)
{

}
JsonBase::~JsonBase(void)
{

}
int JsonBase::SetJsonObejct(string& jsonObj, string sNode, string strValue)
{
    Json::Value root;
    if (!ReadJson(jsonObj, root))
    {
        return RET_ERROR;
    }

    string strPreNode; //前一个节点
    string strNode;    //当前节点
    Json::Value obj = root; //当前节点对象

    //需先将值转化为对象
    Json::Reader jsonReader;
    Json::Value newObject;
    jsonReader.parse(strValue, newObject);

    //判断输入，字符串必须以引号括起来
    string strVal = strValue;
    int nStart = strVal.find('\"');
    int nEnd   = strVal.find_last_of('\"');
    if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
    {
        if (!(0 == nStart && strVal.size()-1 == nEnd))
        {
            cout<<"值参数输入错误!\n";
            return RET_ERROR;
        }
        else
        {
            TrimQuotationMarks(strVal);
        }
    }

    string strKey = sNode;
    vector<string> vecObject = SplitObject(strKey);

    obj = newObject;
    if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
    {
        obj = strVal;
    }
    for (size_t i=vecObject.size()-1; i>=0; i--)
    {
        //节点为空，代表改root
        if (vecObject.size() == 0)
        {
            if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
            {
                root = strVal;
            }
            else
            {
                root = newObject;
            }
            obj = root;
            break;
        }

        strNode = vecObject[i];
        //最后一条
        if (i == 0)
        {
            int nStart = strNode.find('[');
            int nEnd   = strNode.find(']');

            if (-1 != nStart && -1 != nEnd)
            {
                int nPos = -1;
                sscanf(strNode.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
                if (!root.isArray())
                {
                    return RET_ERROR;
                }

                if (nPos < 0)
                {
                    int nSize = root.size();
                    //负数下标不能超过size大小
                    if (nPos*(-1) > nSize)
                    {
                        cout<<"节点下标越界!\n";
                        return RET_ERROR;
                    }
                    nPos = nPos % nSize;
                    if (nPos != 0)
                    {
                        nPos += nSize;
                    }
                }
                else if (nPos > root.size()-1)
                {
                    cout<<"节点下标越界!\n";
                    return RET_ERROR;
                }
                if (obj.type() == Json::stringValue || obj.type() == Json::nullValue)
                {
                    root[nPos] = strVal;
                }
                else
                {
                    root[nPos] = obj;
                }
            }
            else
            {
                if (root.type() != Json::objectValue)
                {
                    cout<<"当前节点不是对象!\n";
                    return RET_ERROR;
                }
                if (obj.type() == Json::stringValue || obj.type() == Json::nullValue)
                {
                    root[strNode] = strVal;
                }
                else
                {
                    root[strNode] = obj;
                }
            }
            obj = root;
            break;
        }

        strPreNode = vecObject[i-1];
        Json::Value tempObject = root;

        if (!GetJsonObject(tempObject, strPreNode))
        {
            cout<<"节点不存在!\n";
            return RET_ERROR;
        }

        //获取Key
        if (strNode.substr(strPreNode.size(),1) == ".")
        {
            strKey = strNode.substr(strPreNode.size()+1);
        }
        else
        {
            strKey = strNode.substr(strPreNode.size());
        }
        //key为数组下标
        int nStart = strKey.find('[');
        int nEnd   = strKey.find(']');

        if (-1 != nStart && -1 != nEnd)
        {
            int nPos = -1;
            sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
            if (!tempObject.isArray())
            {
                cout<<"节点不是数组!\n";
                return RET_ERROR;
            }
            if (nPos < 0)
            {
                int nSize = tempObject.size();
                //负数下标不能超过size大小
                if (nPos*(-1) > nSize)
                {
                    cout<<"节点下标越界!\n";
                    return RET_ERROR;
                }
                nPos = nPos % nSize;
                if (nPos != 0)
                {
                    nPos += nSize;
                }
            }
            else if (nPos >tempObject.size()-1)
            {
                cout<<"节点下标越界!\n";
                return RET_ERROR;
            }
            tempObject[nPos] = obj;
        }
        else
        {
            tempObject[strKey] = obj;
        }
        obj = tempObject;
    }
    root = obj;
    jsonObj = root.toStyledString();
    return RET_OK;
}

string ReturnJsonIndexString(Json::Value jObejct, string strValue)
{
    string strKey = strValue;
    int nStart = strKey.find('[');
    int nEnd   = strKey.find(']');
    if (-1 != nStart && -1 != nEnd)
    {
        int nPos = -1;
        sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
        if (nPos < 0)
        {
            cout<<"数组下标不正确!\n";
            return "";
        }
        string strTempKey = strKey.substr(0,nStart);
        vector<string> retVec = GetChildNodList(jObejct,strTempKey);
        cout<<"members.size()="<<retVec.size()<<endl;
        for (size_t k=0; k<retVec.size(); k++)
        {
            cout<<"members["<<k<<"]="<<retVec[k]<<endl;
        }
        string strIndex = retVec[nPos];
        string strRet = strKey.substr(0,nStart) + "." + strIndex + strKey.substr(nEnd+1);
        strValue = strRet;
        return ReturnJsonIndexString(jObejct, strValue);
    }
    else
    {
        return strValue;
    }
}
string&   replace_all(string&   str,const   string&   old_value,char new_value)
{
    while(true)   {
        string::size_type   pos(0);
        if(   (pos=str.find(old_value))!=string::npos   )
        {
            string strTemp;
            strTemp += str.substr(0,pos);
            strTemp += new_value;
            strTemp += str.substr(pos+old_value.size());
            str = strTemp;
        }
        else   break;
    }
    return   str;
}
void TrimQuotationMarks(string& str)
{
    if (0 == str.find('\"'))
    {
        str = str.substr(1);
    }
    if ( str.size()-1 == str.find_last_of('\"'))
    {
        str = str.substr(0,str.size()-1);
    }
}
void ConvertStr(string& str)
{
    string strVal = str;
    strVal = replace_all(strVal,"\\b",'\b');
    strVal = replace_all(strVal,"\\f",'\f');
    strVal = replace_all(strVal,"\\n",'\n');
    strVal = replace_all(strVal,"\\r",'\r');
    strVal = replace_all(strVal,"\\t",'\t');
    strVal = replace_all(strVal,"\\\"",'\"');
    strVal = replace_all(strVal,"\\\'",'\'');
    strVal = replace_all(strVal,"\\\\",'\\');

    str = strVal;
}
int JsonBase::GetJsonValue(string jsonObj,string strNode,string& strOut)
{
    Json::Value root;
    if (!ReadJson(jsonObj, root))
    {
        return RET_ERROR;
    }

    bool bFlag = true;
    Json::Value jObject = root;
    if (strNode != "")
    {
        bFlag  = GetJsonObject(jObject, strNode);
    }

    string strRet = "";
    if (!bFlag)
    {
       strOut = strRet;
       return RET_ERROR;
    }
    else
    {
        strRet = jObject.toStyledString();
        strRet = strRet.substr(0,strRet.size()-1);
        if (jObject.type() != Json::objectValue)
        {
            ConvertStr(strRet);
            TrimQuotationMarks(strRet);
        }
        strOut = strRet;
        return jObject.type();
    }
    return RET_OK;
}

int JsonBase::GetJsonChildNodeArray(string jObject, string strNode,vector<string>& vecChildNode)
{
    Json::Value root;
    if (!ReadJson(jObject, root))
    {
        return RET_ERROR;
    }
    Json::Value jsonObj = root;

    //查找该节点
    if (!GetJsonObject(jsonObj, strNode))
    {
        cout<<"节点不存在!\n";
        return RET_ERROR;
    }

    string strRet,strTemp;
    //清空返回值内容
    if (!vecChildNode.empty())
    {
        vecChildNode.clear();
    }

    if (jsonObj.type() == Json::objectValue)
    {
        Json::Value::Members members = jsonObj.getMemberNames();
        Json::Value::Members::iterator it = members.begin();
        while (it != members.end())
        {
            strTemp = *it;
            strTemp = strTemp.substr(0,strTemp.size());
            vecChildNode.push_back(strTemp);
            it++;
        }
    }
    else if (jsonObj.type() == Json::arrayValue)
    {
        for (size_t i=0; i<jsonObj.size(); i++)
        {
            strTemp = jsonObj[i].toStyledString();
            strTemp = strTemp.substr(0,strTemp.size()-1);
            if (jsonObj[i].type() != Json::objectValue)
            {
                ConvertStr(strTemp);
                TrimQuotationMarks(strTemp);
            }
            vecChildNode.push_back(strTemp);
        }
    }
    else
    {
        return RET_ERROR;
    }
    return RET_OK;
}

bool ContainObject(string strValue, Json::Value &jsonObj)
{
    if (!jsonObj.isMember(strValue))
    {
        return false;
    }
    jsonObj = jsonObj[strValue];
    return true;
}
bool JsonBase::IsNodeExisted(string jObject, string strNode)
{
    Json::Value root;
    if (!ReadJson(jObject, root))
    {
        return false;
    }

    string strValue =strNode;
    if (strValue == "")
    {
        cout<<"输入字符串为空!\n";
        return false;
    }

	vector<string> vecArray = /*SplitObject*/split(strValue,".");

	for(size_t i=0; i<vecArray.size();i++){

		Json::Value jsonObj = root;
		if (!GetJsonObject(jsonObj, vecArray[i])){

			return false;
		}
	}
	return true;
}

int JsonBase::AppendArrayObject(string& jsonObj, string sNode, string strValue)
{
    Json::Value root;
    if (!ReadJson(jsonObj,root))
    {
        return RET_ERROR;
    }

    string strPreNode;   //前一个节点
    string strNode;      //当前节点
    Json::Value obj = root; //当前节点对象

    //需先将值转化为对象
    Json::Reader jsonReader;
    Json::Value newObject;
    jsonReader.parse(strValue, newObject);

    string strKey = sNode;
    vector<string> vecObject;

    //append root节点
    if (strKey == "")
    {
        if (!root.isArray())
        {
            cout<<"节点不是数组!\n";
            return RET_ERROR;
        }
        if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
        {
            root.append(strValue);
        }
        else
        {
            root.append(newObject);
        }
        obj = root;
    }
    else
    {
        vecObject = SplitObject(strKey);

        //只有单个节点，操作root
        if (vecObject.size() == 0)
        {
            int nStart = strKey.find('[');
            int nEnd   = strKey.find(']');
            if (-1 != nStart && -1 != nEnd)
            {
                int nPos = -1;
                sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);

                if (nPos < 0)
                {
                    int nSize = root.size();
                    //负数下标不能超过size大小
                    if (nPos*(-1) > nSize)
                    {
                        cout<<"节点下标越界!\n";
                        return RET_ERROR;
                    }
                    nPos = nPos % nSize;
                    if (nPos != 0)
                    {
                        nPos += nSize;
                    }
                }
                else if (nPos > root.size()-1)
                {
                    cout<<"节点下标越界!\n";
                    return RET_ERROR;
                }
                if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
                {
                    root[nPos].append(strValue);
                }
                else
                {
                    root[nPos].append(newObject);
                }
            }
            else
            {
                if (newObject.type() == Json::stringValue || newObject.type() == Json::nullValue)
                {
                    root[strKey].append(strValue);
                }
                else
                {
                    root[strKey].append(newObject);
                }
            }
            obj = root;
        }
        else
        {
            //插入点
            Json::Value jObejct = root;
            if (!GetJsonObject(jObejct, sNode))
            {
                return RET_ERROR;
            }
            if (!jObejct.isArray())
            {
                cout<<"节点不是数组!\n";
                return RET_ERROR;
            }

             if (newObject.type() == Json::nullValue)
            {
                jObejct.append(strValue);
            }
            else
            {
                jObejct.append(newObject);
            }
            obj = jObejct;
        }
    }
    for (size_t i=vecObject.size()-1; i>=0; i--)
    {
        if (vecObject.size() == 0)
        {
            break;
        }

        strNode = vecObject[i];
        //最后一条
        if (i == 0)
        {
            int nStart = strNode.find('[');
            int nEnd   = strNode.find(']');

            if (-1 != nStart && -1 != nEnd)
            {
                int nPos = -1;
                sscanf(strNode.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
                if (!root.isArray())
                {
                    return RET_ERROR;
                }

                if (nPos < 0)
                {
                    int nSize = root.size();
                    //负数下标不能超过size大小
                    if (nPos*(-1) > nSize)
                    {
                        cout<<"节点下标越界!\n";
                        return RET_ERROR;
                    }
                    nPos = nPos % nSize;
                    if (nPos != 0)
                    {
                        nPos += nSize;
                    }
                }
                else if (nPos > root.size()-1)
                {
                    cout<<"节点下标越界!\n";
                    return RET_ERROR;
                }
                root[nPos] = obj;
            }
            else
            {
                root[strNode] = obj;
            }
            obj = root;
            break;
        }

        strPreNode = vecObject[i-1];
        Json::Value tempObject = root;

        if (!GetJsonObject(tempObject, strPreNode))
        {
            return RET_ERROR;
        }

        //获取Key
        if (strNode.substr(strPreNode.size(),1) == ".")
        {
            strKey = strNode.substr(strPreNode.size()+1);
        }
        else
        {
            strKey = strNode.substr(strPreNode.size());
        }
        //key为数组下标
        int nStart = strKey.find('[');
        int nEnd   = strKey.find(']');

        if (-1 != nStart && -1 != nEnd)
        {
            int nPos = -1;
            sscanf(strKey.substr(nStart+1,nEnd-nStart-1).c_str(),"%d", &nPos);
            if (!tempObject.isArray())
            {
                return RET_ERROR;
            }
            if (nPos < 0)
            {
                int nSize = tempObject.size();
                //负数下标不能超过size大小
                if (nPos*(-1) > nSize)
                {
                    cout<<"节点下标越界!\n";
                    return RET_ERROR;
                }
                nPos = nPos % nSize;
                if (nPos != 0)
                {
                    nPos += nSize;
                }
            }
            else if (nPos >= tempObject.size())
            {
                cout<<"节点下标越界!\n";
                return RET_ERROR;
            }
            tempObject[nPos] = obj;
        }
        else
        {
            tempObject[strKey] = obj;
        }
        obj = tempObject;
    }
    root = obj;
    jsonObj = root.toStyledString();
    return RET_OK;
}

