#include<iostream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<fstream>
#include<vector>
#include<iterator>
#include<stack>
#include<map>

using namespace std;

#define SIZE 100

string rule_table[SIZE][SIZE];
int column_rule_table[SIZE] = { 0 };
int row = 0;
int row_count = 0;
string input;
string current_input;
bool Error;

stack<string> compare_stack;
map<string, map<string, string> > parse_table;

struct Rules
{
	string nonterminal;
	string terminal;
	string First[SIZE];
	int count;

	Rules()
	{
		count = 0;
	}
};

Rules rule[SIZE];

void input_rules();
void divide();
bool is_terminal(string s);
string find_first_word(string s);
void first_function(string var, Rules &t);
void trim(string &s);
void create_parse_table();
string read_input();
void match_function(stack<string> &x);



void trim(string &s)
{
	while (s[0] == ' ' && s.length() != 0)
		s.erase(s.begin());
	while (s[s.length() - 1] == ' ' && s.length() != 0)
		s.erase(s.end() - 1);
}

bool is_terminal(string s)
{
	for (int i = 0; i < row; i++)
	{
		if (s == rule_table[i][0])
            return false;
	}
	return true;
}

string find_first_word(string s)
{
	string w;
	for (int i = 0; s[i] != ' ' && i < s.length(); i++)
	{
		w += s[i];
	}
	return w;
}

string read_input()
{
	string s = find_first_word(input);
	if (s.length() < input.length())
		input.erase(input.begin(), input.begin() + s.length() + 1);
	return s;
}


void input_rules()
{
	string line;
	string single_rule;

	while (getline(cin, line) && line != "end")
	{
		int i;
		for (i = 0; line[i] != '-'; i++)
		{
			single_rule += line[i];
		}
		trim(single_rule);
		rule_table[row][column_rule_table[row]++] = single_rule; //Add Variable to table
		single_rule.clear();

		for (int j = i + 1; j < line.length(); j++) //Continuing after '-' to the end of line
		{
			if (line[j] == '|') // If delimiter is found
			{
				trim(single_rule);
				rule_table[row][column_rule_table[row]++] = single_rule; //Add each rule to table
				single_rule.clear();
			}
			else
				single_rule += line[j]; //Add each character to single_rule
		}
		trim(single_rule);
		rule_table[row][column_rule_table[row]++] = single_rule; //Add last rule to table
		single_rule.clear();
		row++; //Increase row
	}
}

void divide() //Divide the rules and add them to rule struct
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 1; j < column_rule_table[i]; j++)
		{
			rule[row_count].terminal = rule_table[i][j]; //Add terminals to rule
			rule[row_count].nonterminal = rule_table[i][0]; //Add variable to rule
			first_function(rule_table[i][j], rule[row_count]);//Find first function
			row_count++;
		}
	}
}




void create_parse_table()
{
	for (int i = 0; i < row_count; i++)
	{
		for (int j = 0; j < rule[i].count; j++)
		{
			parse_table[rule[i].nonterminal][rule[i].First[j]] = rule[i].terminal;	// parse_table['Variable']['First Function of that particualr rule'] = the rule
		}
	}
}


void first_function(string var, Rules &t) //Find first function
{
	int i;
	var = find_first_word(var);
	for (i = 0; i < row; i++)
	{
		if (rule_table[i][0] == var)
			break;  //If var is a non-terminal, break
	}

	if (i >= row) //If var is not a non-terminal, var is a terminal
	{
		t.First[t.count] = var; //Add var to first function
		t.count++;
	}
	else //If var is a terminal
	{
		for (int j = 1; j < column_rule_table[i]; j++)
		{
			first_function(rule_table[i][j], t); //Go to the production rule of that terminal and find first function, and so on
		}
	}
}



void match_function(stack<string> &x) //Continue to match input characters with top of stack
{
	while (x.top() == current_input) //While top of stack matches input characters
	{
		current_input = read_input(); //Continue to read from input
		x.pop();
		if (x.empty())
			break;
	}
}

int main()
{

	freopen("grammer.txt", "r", stdin);
	input_rules();
	divide();
	create_parse_table();
	cout << "Input: ";
	getline(cin, input);
	cout << input << endl;
	Error = false;
	compare_stack.push(rule_table[0][0]); //Push the start variable to stack
	current_input = read_input(); //Take the first word of input

	while (!compare_stack.empty()) //While the stack is not empty
	{
		string top_of_stack = find_first_word(compare_stack.top()); //The top word of the stack

		if (top_of_stack == current_input || top_of_stack == "e") //Here e means epsilon. If input matches top of stack or top of stack is epsilon
		{
			if (top_of_stack == "e") //If top of stack is epsilon
			{
				compare_stack.pop(); //pop epsilon from stack
			}
			else
				match_function(compare_stack);
		}
		else if (is_terminal(top_of_stack)) //If top of stack is a terminal and does not match input
		{
			Error = true; //incorrect, return error
			break;
		}
		else if (parse_table.find(top_of_stack) == parse_table.end() || parse_table[top_of_stack].find(current_input) == parse_table[compare_stack.top()].end()) //If top of stack is a non-terminal or input does not match the variable rule
		{
			if (parse_table[top_of_stack]["e"] == "e") //If there is an epsilon in the rule of the top word of stack
			{
				compare_stack.pop();
				compare_stack.push("e"); //Pop top word of stack and insert epsilon onto stack
			}
			else
			{
				Error = true; //incorrect, return error
				break;
			}
		}
		else
		{
			string temp = parse_table[top_of_stack][current_input]; //There is a matching rule for the top word of the stack and the current input read character
			compare_stack.pop(); //Pop top of stack

			vector<string>tmp;  //Take a string vector
			while (temp.length() > 0) //Until temp is empty
			{
				string w = find_first_word(temp); //Take first word of temp
				tmp.push_back(w); //Push first word into the vector
				if (temp.length() > w.length())
					temp.erase(temp.begin(), temp.begin() + w.length()); //Erase first word from temp
				else
				{
					temp.clear(); //we have reached end of temp, clear the temp variable
					break;
				}
				trim(temp);
			}

			while (!tmp.empty()) //Until tmp is not empty
			{
				compare_stack.push(tmp.back()); //vector is used here to push rule into stack in reverse order
				tmp.pop_back(); //pop from vector
			}
		}
	}

	if (Error) //If there is any error in the process of matching
		cout << "The input does not match the grammer" << endl;
	else
		cout << "The input matches the grammer" << endl; // The matching was a success.

	return 0;
}
