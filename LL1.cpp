#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <set>

using namespace std;

// 定义O为空
#define EP 'O'

//文法存储的数据结构，O表示空
map<string, vector<string>> syntax;

bool isNT(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool isT(char c)
{
    return ! isNT(c) || c == EP;
}


// void merge_set(set<char>& s1, set<char>& s2){
//     s1.insert(s2.begin(), s2.end());
// }

void display(set<char> s){
    for(auto var : s)
    {
        cout<<var<<" ";
    }
    cout<<endl;
}

set<char> first(string key)
{
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
            if(key.length() == 1){
                return res;
            }
            // cout<<"first of key[1:]"<<endl;
            // display(tmp);
            res.insert(tmp.begin(), tmp.end());
            if (! tmp.count(EP))
            {
                res.erase(EP);
            }
        }
        return res;
    }
}

// set<char> follow(char* key){

// }

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
        // cout << key << ":";
        // for (auto var : syntax[key])
        // {
        //     cout << var << " ";
        // }
        // cout << endl;
    }
    in.close();
    cout<<"first ED:"<<endl;
    display(first("ED"));
    cout<<"first S:"<<endl;
    display(first("S"));
    cout<<"first B:"<<endl;
    display(first("B"));
    cout<<"first C:"<<endl;
    display(first("C"));
    cout<<"first A:"<<endl;
    display(first("A"));
    cout<<"first E:"<<endl;
    display(first("E"));
    cout<<"first D"<<endl;
    display(first("D"));
    cout<<"first EB"<<endl;
    display(first("EB"));
    return 0;
}