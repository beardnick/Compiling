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

// 定义O为空
#define EP '`'

//文法存储的数据结构，O表示空
map<string, vector<string>> syntax;
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

set<char> merge_set(set<char> s1, set<char> s2)
{
    s1.insert(s2.begin(), s2.end());
    return s1;
}

string toString(stack<char> s)
{
    // cout<<"stack.toString"<<endl;
    stack<char> s1;
    string res;
    while (!s.empty())
    {
        s1.push(s.top());
        s.pop();
    }
    while (!s1.empty())
    {
        res += s1.top();
        s1.pop();
    }
    return res;
}

string toString(queue<char> q)
{
    // cout<<"queue.toString"<<endl;
    string res;
    while (!q.empty())
    {
        res += q.front();
        q.pop();
    }
    return res;
}

void display(set<char> s)
{
    for (auto var : s)
    {
        cout << var << " ";
    }
    cout << endl;
}

void display(vector<string> s)
{
    for (auto var : s)
    {
        cout << var << " ";
    }
    cout << endl;
}

set<char> first(string key)
{
    cout << "FIRST(" << key << "):" << endl;
    // cout<<"key:"<<key<<endl;
    set<char> res;
    if (key.length() < 1)
    {
        return res;
    }
    if (isT(key[0]))
    {
        res.insert(key[0]);
        // cout<<"T:"<<key<<endl;
        // display(res);
        return res;
    }
    else
    {
        for (auto var : syntax[key.substr(0, 1)])
        {
            set<char> tmp = first(var);
            res.insert(tmp.begin(), tmp.end());
        }
        // cout<<"NT:"<<key<<endl;
        // display(res);
        if (res.count(EP))
        {
            set<char> tmp = first(key.substr(1, key.length() - 1));
            if (key.length() == 1)
            {
                return res;
            }
            // cout<<"first of key[1:]"<<endl;
            // display(tmp);
            // res.insert(tmp.begin(), tmp.end());
            res = merge_set(res, tmp);
            if (!tmp.count(EP))
            {
                res.erase(EP);
            }
        }
        return res;
    }
}

vector<string> whoContains(char key)
{
    vector<string> res;
    map<string, vector<string>>::iterator iter;
    for (iter = rightSyntax.begin(); iter != rightSyntax.end(); iter++)
    {
        // cout<<iter->first<<endl;
        if (iter->first.find(key) != string::npos)
        {
            res.push_back(iter->first);
        }
    }
    return res;
}

bool canBeNull(string key)
{
    // cout << "can be null" << endl;
    if (key[0] == EP)
    {
        return true;
    }
    for (auto var1 : key)
    {
        if (isT(var1))
        {
            return false;
        }
        bool containsEP = false;
        stringstream ss;
        ss << var1;
        for (auto var2 : syntax[ss.str()])
        {
            //    cout<<"var2:"<<var2;
            if (var2[0] == EP)
            {
                containsEP = true;
                break;
            }
        }
        // cout << var1 << " contains null " << containsEP << endl;
        if (!containsEP)
        {
            return false;
        }
    }
    return true;
}

set<char> follow(char key)
{
    cout << "follow(" << key << "):" << endl;
    set<char> res;
    if (isT(key))
    {
        return res;
    }
    if (key == 'S' || key == 'E')
    {
        // cout << "func 3" << endl;
        res.insert('#');
    }
    vector<string> targets = whoContains(key);
    string beta;
    for (auto var : targets)
    {
        int index = var.find(key);
        // cout << "index:" << index << endl;
        if (index == var.length() - 1)
        {
            beta = "";
        }
        else
        {
            beta = var.substr(index + 1, var.length() - index - 1);
        }
        // cout << "beta:" << beta << endl;
        if (beta.length() == 0 || canBeNull(beta))
        {
            // cout << "func 3" << endl;
            for (auto e : rightSyntax[var])
            {
                if (e[0] != key)
                {
                    res = merge_set(res, follow(e[0]));
                }
            }
        }
        if (beta.length() > 0)
        {
            // cout << "func 2" << endl;
            res = merge_set(res, first(beta));
            res.erase(EP);
        }
    }
    return res;
}

set<char> select(char left, string right)
{
    cout << left << " -> " << right << ":" << endl;
    set<char> res;
    if (canBeNull(right))
    {
        cout << right << " can be null" << endl;
        set<char> tmp = follow(left);
        tmp.erase(EP);
        res = merge_set(first(right), tmp);
    }
    else
    {
        res = first(right);
    }
    return res;
}

bool isValid(string target)
{
    cout << "analyze " << target << endl;
    stack<char> s;
    queue<char> q;
    for (auto var : target + "#")
    {
        q.push(var);
    }
    s.push('#');
    s.push('E');
    printf("%-20s", toString(s).c_str());
    printf("%20s", toString(q).c_str());
    cout << endl;
    while (s.top() != '#' || q.front() != '#')
    {
        if (s.top() != q.front())
        {
            string replace;
            if (predictTable[s.top()].count(q.front()) != 0)
            {
                replace = predictTable[s.top()][q.front()];
                // cout << "replace:" << replace << endl;
            }
            else
            {
                return false;
            }
            s.pop();
            if (replace[0] != EP)
            {
                for (int i = replace.length() - 1; i >= 0; i--)
                {
                    s.push(replace[i]);
                }
            }
        }
        else
        {
            s.pop();
            q.pop();
        }
        printf("%-20s", toString(s).c_str());
        printf("%20s", toString(q).c_str());
        cout << endl;
    }
    return true;
}

bool creatPredictTable(){
    map<string, vector<string>>::iterator iter;
    for (iter = syntax.begin(); iter != syntax.end(); iter++)
    {
        char left = iter->first[0];
        for (auto var : iter->second)
        {
            //    display(select(iter->first[0], var));
            set<char> selectResult = select(left, var);
            for (auto i : selectResult)
            {
                if(predictTable.count(left) != 0 && predictTable[left].count(i) != 0){
                    return false;
                }
                predictTable[left][i] = var;
            }
        }
    }
    return true;
}

void printPredictTable(){
    map<string, vector<string>>::iterator iter;
    printf("%-10s", " ");
    for (auto i : terminal)
    {
        printf("%-10c", i);
    }
    cout << endl;
    for (iter = syntax.begin(); iter != syntax.end(); iter++)
    {
        //    display(select(iter->first[0], var));
        printf("%-10c", iter->first[0]);
        for (auto i : terminal)
        {
            if (predictTable[iter->first[0]].count(i) != 0)
            {
                printf("%-10s", predictTable[iter->first[0]][i].c_str());
                //    cout<<predictTable[iter->first[0]][i]<<"\t";
            }
            else
            {
                printf("%-10s", " ");
            }
        }
        cout << endl;
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
    in.close();
    terminal.insert('#');
    terminal.insert(EP);
    map<string, vector<string>>::iterator iter;
    // // 所有first集
    // for(iter = syntax.begin() ; iter != syntax.end() ; iter ++){
    //     display(first(iter->first));
    // }

    // // 所有follow集
    // for(iter = syntax.begin() ; iter != syntax.end() ; iter ++){
    //     display(follow(iter->first[0]));
    // }

    // 所有select集
    // for(iter = syntax.begin() ; iter != syntax.end() ; iter ++){
    //     for(auto var : iter->second)
    //     {
    //        display(select(iter->first[0], var));
    //     }
    // }
    if(creatPredictTable()){
        cout<<"该文法是LL(1)文法"<<endl;
    }else{
        cout<<"该文法不是LL(1)文法"<<endl;
        return 1;
    }
    printPredictTable();
    // isLL1();
    // isValid("i+i*i");
    // string test = "(i+i*i)*i+(i+i)";
    string test = "(i+i*i)*i+((i+i)";
    // string test = "(i+i*i)(*i+(i+i)";
    // string test = "(i+i*i)(*i+-(i+i)";
    if (isValid(test))
    {
        cout <<test 
             << "是该文法的句子" << endl;
    }
    else
    {
        cout << test
             << "不是该文法的句子" << endl;
    }
    return 0;
}