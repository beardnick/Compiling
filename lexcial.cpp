#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;


enum STATE {
    START,
    WORD_OR_DIGIT,
    VAR_OR_KEY,
    OPERATOR_OR_DELIMITER,
    OPERATOR_OR_DELIMITER_TERMINAL
};

class state {
public:
    state(STATE name, bool is_terminal) : name(name), is_terminal(is_terminal) {}

    STATE getName() const {
        return name;
    }

    bool isIs_terminal() const {
        return is_terminal;
    }

    void setName(STATE name) {
        state::name = name;
    }

    const string &getResutl() const {
        return resutl;
    }

    void setResutl(const string &resutl) {
        state::resutl = resutl;
    }

private:
    enum STATE name;
    string resutl;
    bool is_terminal;
};

class arc {
public:
    arc(string chars): chars(chars) {
    }

    arc(){
        chars = "";
    }

    const string &getChars() const {
        return chars;
    }


private:
    string chars;
};

void analyze_word(ifstream input, ofstream output) {
    map<string,int> words_list;
    words_list["main"] = 1;
    words_list["if"] = 2;
    words_list["else"] = 3;
    words_list["wile"] = 4;
    words_list["do"] = 5;
    words_list["for"] = 6;
    words_list["return"] = 7;
    words_list["letter(letter|digit)*"] = 8 ;
    words_list["digit*"] = 9 ;
    words_list["+"] = 10;
    words_list["—"] = 11;
    words_list["*"] = 12;
    words_list["/"] = 13;
    words_list["%"] = 14;
    words_list[">"] = 15;
    words_list[">="] = 16;
    words_list["<"] = 17;
    words_list["<="] = 18;
    words_list["=="] = 19;
    words_list["!="] = 20;
    words_list["="] = 21;
    words_list["；"] = 22;
    words_list["("] = 23;
    words_list[")"] = 24;
    words_list["{"] = 25;
    words_list["}"] = 26;
    words_list["int"] = 27;
    words_list["float"] = 28;
    words_list["double"] = 29;
    words_list["char"] = 30;
    state start(START, false);
    state word_or_digit(WORD_OR_DIGIT, false);
    state operator_or_delimiter(OPERATOR_OR_DELIMITER, false);
    state var_or_key(VAR_OR_KEY, true);
    state operator_or_delimiter_terminal(OPERATOR_OR_DELIMITER_TERMINAL, true);
    string delimiters = "{}();";
    string logic_operators = "=<>!";
    string digit_operators = "+-*/%";
    string lower_words = "abcdefghijklmnopqrstuvwxyz";
    string upper_words = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string digits = "0123456789";
    arc word_or_digit_arc(lower_words + upper_words + digits);
    arc delimiters_and_operators(digit_operators + delimiters);
    arc dfa[100][100];
    dfa[start.getName()][word_or_digit.getName()] = word_or_digit_arc;
    dfa[start.getName()][operator_or_delimiter_terminal.getName()] = delimiters_and_operators;
    dfa[start.getName()][operator_or_delimiter.getName()] = logic_operators;
    dfa[word_or_digit.getName()][word_or_digit.getName()] = word_or_digit_arc;
    enum STATE current_state = start.getName();
    char last_char = '\0';
    string current_result = "";
    while (!input.eof()) {
        char current_char;
        input>>current_char;
        bool match = false;
        for (int i = 0; i < 6; ++i) {
            if (dfa[current_state][i].getChars().find(current_char) != string::npos) {
                current_result += current_char;
                current_state = static_cast<STATE>(i);
                match = true;
                break;
            }
        }
        if(! match){
        }
    }
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        cout << "Usage" << endl << "cmd input_file output_file" << endl;
        return 1;
    }
    ifstream in(argv[1]);
    ofstream out(argv[2]);
    if (!in.is_open()) {
        cout << "file is not open" << endl;
//        exit(1);
        return 1;
    }
    while (!in.eof()) {
//        string buffer;
    char c;
        in>>c;
         cout<<c<<endl;
         out<<c<<endl;
    }
    in.close();
    out.close();
//    char delimiters[] = {'{','}','(',')',';'};
//    char operators[] = {'=','+','-','*','/','%','<','>','!'} ;
    return 0;
}