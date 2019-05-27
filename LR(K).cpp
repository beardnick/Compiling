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
#include <utility>

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
vector<map<string, vector<string>>> nodeSet;
// 生成式右部对应的左部的映射
map<string, vector<string>> rightSyntax;
//预测分析表
map<char, map<char, string>> predictTable;
//终结符表
set<char> terminal;
//产生式表
vector<pair<string, string>> syntaxVector;

//分析表
map<string, map<string, string>> analyzeTable;

// // 最终的分析表的GOTO部分
// map<string, map<string, string> > gotoTable;

string charToString(char c)
{
    stringstream ss;
    ss << c;
    return ss.str();
}

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
    for (auto mpair : node)
    {
        for (auto right : mpair.second)
        {
            cout << mpair.first << " -> " << right << endl;
        }
    }
}

void displayNodeSet()
{
    int cnt = 0;
    for (auto var : nodeSet)
    {
        cout << "I" << cnt++ << ":" << endl;
        displayNode(var);
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
    for (auto mpair : node)
    {
        for (auto value : mpair.second)
        {
            string next = getNext(value);
            if (next == key)
            {
                value = shift(value);
                res[mpair.first].push_back(value);
            }
        }
    }
    res = _closure(res);
    // displayNode(res);
    return res;
}

int visited(map<string, vector<string>> node)
{
    int cnt = -1;
    for (auto ni : nodeSet)
    {
        // cout<<"ni:"<<endl;
        // displayNode(ni);
        cnt++;
        bool nodeEqual = true;
        for (auto mpair : ni)
        {
            if (node.count(mpair.first) == 0 ||
                node[mpair.first] != mpair.second)
            {
                nodeEqual = false;
                break;
            }
        }
        for (auto mpair : node)
        {
            if (ni.count(mpair.first) == 0 ||
                ni[mpair.first] != mpair.second)
            {
                nodeEqual = false;
                break;
            }
        }
        if (nodeEqual)
        {
            return cnt;
        }
    }
    return -1;
}

int findGen(string first, string second)
{
    // cout<<"find "<<first<<" ->"<<second<<endl;
    int cnt = 0;
    for (auto var : syntaxVector)
    {
        if (var.first == first && var.second == second)
        {
            return cnt;
        }
        cnt++;
    }
    return -1;
}

void createGraph()
{
    map<string, vector<string>> initNode;
    initNode["S"].push_back(".E");
    map<string, vector<string>> node0;
    node0 = _closure(initNode);
    nodeSet.push_back(node0);
    queue<map<string, vector<string>>> q;
    q.push(node0);
    int cnt = 0;
    while (!q.empty())
    {
        cnt++;
        map<string, vector<string>> currentNode = q.front();
        q.pop();
        cout << "I" << cnt - 1 << ":" << endl;
        displayNode(currentNode);
        for (auto mpair : currentNode)
        {
            for (auto value : mpair.second)
            {
                map<string, vector<string>> newNode;
                string next = getNext(value);
                if (next.length() > 0)
                {
                    newNode = _goto(currentNode, next);
                }
                else
                {
                    if (value == "E.")
                    {
                        analyzeTable["S" + to_string(cnt - 1)]["#"] = "acc";
                        printf("[S%d][#] = acc\n", cnt - 1);
                        break;
                    }
                    for (auto t : terminal)
                    {
                        if (isT(t))
                        {
                            printf("[S%d][%c] = r%d\n", cnt - 1, t, findGen(mpair.first, value.substr(0, value.length() - 1)));
                            analyzeTable["S" + to_string(cnt - 1)][charToString(t)] = "r" + to_string(findGen(mpair.first, value.substr(0, value.length() - 1)));
                        }
                    }
                    break;
                }
                int nodeIndex = visited(newNode);
                if (nodeIndex == -1)
                {
                    q.push(newNode);
                    nodeSet.push_back(newNode);
                    if (isT(next[0]))
                    {
                        printf("[S%d][%s] = S%d\n", cnt - 1, next.c_str(), nodeSet.size() - 1);
                        analyzeTable["S" + to_string(cnt - 1)][next] = "S" + to_string(nodeSet.size() - 1);
                    }
                    else
                    {
                        printf("[S%d][%s] = %d\n", cnt - 1, next.c_str(), nodeSet.size() - 1);
                        analyzeTable["S" + to_string(cnt - 1)][next] = to_string(nodeSet.size() - 1);
                    }
                }
                else
                {
                    if (isT(next[0]))
                    {
                        printf("[S%d][%s] = S%d\n", cnt - 1, next.c_str(), nodeIndex);
                        analyzeTable["S" + to_string(cnt - 1)][next] = "S" + to_string(nodeIndex);
                    }
                    else
                    {
                        printf("[S%d][%s] = %d\n", cnt - 1, next.c_str(), nodeSet.size() - 1);
                        analyzeTable["S" + to_string(cnt - 1)][next] = to_string(nodeIndex);
                    }
                }
            }
        }
    }
}

string toString(stack<string> s, string split)
{
    // cout<<"stack.toString"<<endl;
    stack<string> s1;
    string res;
    while (!s.empty())
    {
        s1.push(s.top());
        s.pop();
    }
    if (!s1.empty())
    {
        res += s1.top();
        s1.pop();
    }
    while (!s1.empty())
    {
        res += (split + s1.top());
        s1.pop();
    }
    return res;
}

string toStringReverse(stack<string> s, string split)
{
    string res;
    if (!s.empty())
    {
        res += s.top();
        s.pop();
    }
    while (!s.empty())
    {
        res += (split + s.top());
        s.pop();
    }
    return res;
}

bool isValid(string target)
{
    stack<string> state, sign, input;
    state.push("0");
    sign.push("#");
    input.push("#");
    for (int i = target.length() - 1; i >= 0; i--)
    {
        input.push(charToString(target[i]));
    }
    while (true)
    {
        printf("%-20s", toString(state, " ").c_str());
        printf("%-20s", toString(sign, "").c_str());
        printf("%20s\n", toStringReverse(input, "").c_str());
        if (analyzeTable.count("S" + state.top()) == 0 ||
            analyzeTable["S" + state.top()].count(input.top()) == 0 )
        {
            if(analyzeTable["S" + state.top()].count("`") != 0){
                input.push("`");
            }else{
            return false;
            }
        }
        string res = analyzeTable["S" + state.top()][input.top()];
        if (res[0] == 'S')
        {
            state.push(res.substr(1, res.length() - 1));
            sign.push(input.top());
            input.pop();
        }
        else if (res[0] == 'r')
        {
            pair<string, string> mpair = syntaxVector[stoi(res.substr(1, res.length() - 1))];
            for (auto var : mpair.second)
            {
                state.pop();
                sign.pop();
            }
            input.push(mpair.first);
        }
        else if (res[0] >= '0' && res[0] <= '9')
        {
            state.push(res);
            sign.push(input.top());
            input.pop();
        }
        else if (res == "acc")
        {
            return true;
        }
    }
}

void displayAnalyzeTable()
{
    // map<string, map<string, string>> analyzeTable;
    for (auto pair1 : analyzeTable)
    {
        cout << pair1.first + ":" << endl;
        for (auto pair2 : pair1.second)
        {
            cout << pair2.first + ":" + pair2.second << endl;
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
    terminal.insert('#');
    map<string, vector<string>> node;
    map<string, vector<string>> node1;
    for (auto mpair : syntax)
    {
        for (auto value : mpair.second)
        {
            syntaxVector.push_back(pair<string, string>(mpair.first, value));
        }
    }
    for (auto var : syntaxVector)
    {
        cout << var.first << " -> " << var.second << endl;
    }
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
    // displayNodeSet();
    string target = "bccccd";
    // string target = "aed";
    // string target = "abab";
    // string target = "(";
    // string target = "(i+i*i)*i+(i+i)";
    // string target = "(i+i)";
    if (isValid(target))
    {
        cout << target + "是该文法的句子" << endl;
    }
    else
    {
        cout << target + "不是该文法的句子" << endl;
    }
    // displayAnalyzeTable();
    return 0;
}
