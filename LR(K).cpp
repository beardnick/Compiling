#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <cstdio>
#include <stack>
#include <queue>

using namespace std;

// 定义` 为空
#define EP '`'
// 扩展文法识别符号
#define S 'S'
// 文法识别符号
#define E 'E'
//分隔符
#define DIV '.'

//文法存储的数据结构，EP表示空
map<string, vector<string>> syntax;
//项集规范族的节点表
set<map<string, vector<string>>> nodeSet;
// 生成式右部对应的左部的映射
map<string, vector<string>> rightSyntax;
//预测分析表
map<char, map<char, string>> predictTable;
//终结符表
set<char> terminal;

bool isNT(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool isT(char c)
{
    return !isNT(c) || c == EP;
}

string getNext(string value)
{
    int index = value.find(DIV);
    if (index == value.length() - 1)
    {
        return "";
    }
    else
    {
        return value.substr(index + 1, 1);
    }
}

void displayNode(map<string, vector<string>> node)
{
    for (auto pair : node)
    {
        for (auto right : pair.second)
        {
            cout << pair.first << " -> " << right << endl;
        }
    }
}

string shift(string key)
{
    int index = key.find(DIV);
    if (index == key.length() - 1)
    {
        return key;
    }
    else
    {
        char temp = key[index];
        key[index] = key[index + 1];
        key[index + 1] = temp;
        return key;
    }
}

map<string, vector<string>> _closure(map<string, vector<string>> node)
{
    map<string, vector<string>> res;
    for (auto key : node)
    {
        res[key.first] = key.second;
        for (auto value : key.second)
        {
            string next = getNext(value);
            if (next.length() > 0 && isNT(next[0]))
            {
                for (auto right : syntax[next])
                {
                    right = DIV + right;
                    res[next].push_back(right);
                }
            }
        }
    }
    return res;
}

map<string, vector<string>> _goto(map<string, vector<string>> node, string key)
{
    map<string, vector<string>> res;
    for (auto pair : node)
    {
        for (auto value : pair.second)
        {
            string next = getNext(value);
            if (next == key)
            {
                value = shift(value);
                res[pair.first].push_back(value);
            }
        }
    }
    res = _closure(res);
    // displayNode(res);
    return res;
}

bool visited(map<string, vector<string>> node)
{
    for(auto ni : nodeSet)
    {
        // cout<<"ni:"<<endl;
        // displayNode(ni);
        bool nodeEqual = true;
        for(auto pair : ni)
        {
            if(  node.count(pair.first) == 0 ||
                   node[pair.first] != pair.second ){
                       nodeEqual = false;
                       break;
                   }
        }
        for(auto pair : node)
        {
            if(  ni.count(pair.first) == 0 ||
                   ni[pair.first] != pair.second ){
                       nodeEqual = false;
                       break;
                   }
        }
        if(nodeEqual){
            return true;
        }
    }
    return false;
}

void createGraph()
{
    map<string, vector<string>> initNode;
    initNode["S"].push_back(".E");
    map<string, vector<string>> node0;
    node0 = _closure(initNode);
    nodeSet.insert(node0);
    queue<map<string, vector<string>>> q;
    q.push(node0);
    int cnt = 0;
    while (!q.empty())
    {
        cnt ++;
        map<string, vector<string>> currentNode = q.front();
        q.pop();
        cout<<"I"<<cnt - 1<<":"<<endl;
        displayNode(currentNode);
        for (auto pair : currentNode)
        {
            for (auto value : pair.second)
            {
                map<string, vector<string>> newNode;
                string next = getNext(value);
                if (next.length() > 0)
                {
                    newNode = _goto(currentNode, next);
                }else{
                    break;
                }
                if (!visited(newNode))
                {
                    q.push(newNode);
                    nodeSet.insert(newNode);
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    ifstream in(argv[1]);
    if (!in.is_open())
    {
        cout << "file is not open" << endl;
        return 1;
    }
    int cnt = 0;
    // 构建扩展文法
    syntax["S"].push_back("E");
    // 读取文件内容，生成文法
    while (!in.eof())
    {
        string key, value;
        in >> key;
        in >> value;
        if (key.length() < 1)
        {
            continue;
        }
        syntax[key].push_back(value);
        rightSyntax[value].push_back(key);
        // cout << "left syntax:" << endl;
        // cout << key << ":";
        // for (auto var : syntax[key])
        // {
        //     cout << var << " ";
        // }
        // cout << endl;
        // cout<<"right syntax:"<<endl;
        // cout << value << ":";
        // for (auto var : rightSyntax[value])
        // {
        //     cout << var << " ";
        // }
        // cout << endl;
        for (auto var : value)
        {
            if (isT(var))
            {
                terminal.insert(var);
            }
        }
    }
    map<string, vector<string>> node;
    map<string, vector<string>> node1;

//   test displayNode

    // node["S"].push_back(".E");
    // displayNode(_closure(node));
    // node["E"].push_back("a.A");
    // displayNode(_closure(node));
    // node["A"].push_back("d.");
    // displayNode(_closure(node));
    // node["S"].push_back(".E");
    // node["E"].push_back(".aA");
    // node["E"].push_back(".bB");
    // _goto(node, "a");

// test visited

    // node["S"].push_back(".E");
    // node["A"].push_back("C.a");
    // node1["S"].push_back(".E");
    // node1["A"].push_back("C.");
    // nodeSet.insert(node);
    // nodeSet.insert(node1);
    // cout<<visited(node)<<endl;
    // cout<<visited(node1)<<endl;
    createGraph();
    return 0;
}