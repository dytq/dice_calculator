#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

enum type_element_enum {
	number,
	stack,
	dice_stack,
	operator_bin
};

struct element_struct {
	type_element_enum type_element;
	void * element;
};

enum type_syntax_enum {
	integer,
	open_parenth,
	close_parenth,
	compare,
	ope,
	dice,
	dice_arg,
	syntax_error
};

struct dice_element_struct {
	int value;
	int occurence;
};

struct tree_rules {
	type_syntax_enum type_character;
	void (*ptr_add_stack)(vector<vector<element_struct *>*> * stacks,  char character, type_syntax_enum type_syntax);
	vector<tree_rules *> nodes;
};

// Signatures

// I- Init Main Stack
void init_main_stack(vector<element_struct *> * main_stack, string entry);

// - create automate (set predefined syntax)
struct tree_rules * create_tree_rules();
tree_rules * get_node_character(tree_rules * node, type_syntax_enum type_syntax);

// - functions to add elements in stack and recursive stack
void add_element(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax);
void set_size_dice(vector<vector<element_struct *> *> *stacks, char character, type_syntax_enum type_syntax);
void add_multiply_and_element(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax);
void add_decimal_number(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax);

// II- Reduct Main Stack
void apply_reduct_stack(vector<element_struct*> * stack);
void reduction(char operator_bin, vector<element_struct *> * stack);
struct element_struct * reduce_stack(struct element_struct * arg);

// - Evaluator 
struct element_struct * eval(char operator_bin, struct element_struct * arg1, struct element_struct * arg2);
vector<dice_element_struct*> * addition_dices(vector<dice_element_struct*> * dice_stack1, vector<dice_element_struct*> * dice_stack2); // suppr

// III- Print Functions 
void print_tab(int layer);
void print_stack(vector<element_struct *> * stack, int layer);
void print_res(vector<element_struct *> *stack);

// IV- Other Functions
bool compare_dice_element(struct dice_element_struct * dice_element1, struct dice_element_struct * dice_element2);

// Main Funtion
int main()
{
	string expr;
	getline(cin, expr);

	vector<element_struct *> main_stack;
	init_main_stack(&main_stack, expr);
	// print_stack(&main_stack,0);
	
	apply_reduct_stack(&main_stack);
	print_res(&main_stack);
	return 0;
}

// Functions
void init_main_stack(vector<element_struct *> * main_stack, string entry)
{
	vector<vector<element_struct *>*> stacks; // contains stacks
	stacks.push_back(main_stack);	
	
	tree_rules * node_tree_rules = nullptr;
	node_tree_rules = create_tree_rules();

	int index = 0;
	for (char c : entry) 
	{
		index++;
		type_syntax_enum type_syntax = type_syntax_enum::syntax_error;

		if(c>='0' && c <='9')
		{
			type_syntax = type_syntax_enum::integer;
		}

		if(c == '*' || c == '+' || c == '>' || c == '-')
		{
			type_syntax = type_syntax_enum::ope;	
		}

		if(c == '(') 
		{
			type_syntax = type_syntax_enum::open_parenth;
		}

		if(c == ')')
		{
			type_syntax = type_syntax_enum::close_parenth;
		}
		if(c == 'd')
		{
			type_syntax = type_syntax_enum::dice;
		}
		if(type_syntax == type_syntax_enum::syntax_error)
		{	
			cerr << "Parsing Error (" << index << "): " << "unrecognised character '" << c << "'" << endl;
			exit(EXIT_FAILURE);
		}
		
		node_tree_rules = get_node_character(node_tree_rules, type_syntax);
		if(node_tree_rules == nullptr)
		{
			cerr << "Parsing Error (" << index << "): " << "syntax error '" << c << "' follow by character where not allow" << endl;
			exit(EXIT_FAILURE);
		}
		node_tree_rules->ptr_add_stack(&stacks, c,type_syntax);
	}
}

void apply_reduct_stack(vector<element_struct*> * stack)
{
	reduction('*', stack);
	reduction('+', stack);
	reduction('-', stack);
	reduction('>', stack);
}


void add_element(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax)
{
	if(type_syntax == type_syntax_enum::integer)
	{
		element_struct * E = new element_struct;
		E->type_element = type_element_enum::number;
		int * integer = new int;
		*integer = character - '0';
		E->element = (void *) integer;
		stacks->back()->push_back(E);	
	}
	if(type_syntax == type_syntax_enum::ope)
	{
		element_struct * E = new element_struct;
		E->type_element = type_element_enum::operator_bin;
		char * ope = new char;
		*ope = character;
		E->element = (void *) ope;
		
		stacks->back()->push_back(E);
	}
	if(type_syntax == type_syntax_enum::open_parenth)
	{
		vector<element_struct *> * stack = new vector<element_struct *>;
		stacks->push_back(stack);
	}
	if(type_syntax == type_syntax_enum::close_parenth)
	{
		element_struct * E = new element_struct;
		E->type_element = type_element_enum::stack;
		E->element = stacks->back();
		stacks->pop_back();
		stacks->back()->push_back(E);
	}
	if(type_syntax == type_syntax_enum::dice)
	{
		element_struct * E = new element_struct;
		E->type_element = type_element_enum::dice_stack;
		vector<dice_element_struct*> * dice = new vector<dice_element_struct*>; 
		E->element = (void *) dice;
		stacks->back()->push_back(E);
	}
}

void set_size_dice(vector<vector<element_struct *> *> *stacks, char character, type_syntax_enum type_syntax)
{
	vector<dice_element_struct *> * dice = static_cast<vector<dice_element_struct*>*>(stacks->back()->back()->element);
	int size_stack = character - '0'; 
	size_stack = (dice->size()*10) + size_stack;
	for (int i = dice->size()+1; i <= size_stack; i++) {
		dice_element_struct * dice_element = new dice_element_struct;
		dice_element->value = i;
		dice_element->occurence = 1;
		dice->push_back(dice_element);
	}
}

void add_multiply_and_element(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax)
{
	add_element(stacks, '*', type_syntax_enum::ope);
	add_element(stacks, character, type_syntax);
}

void add_decimal_number(vector<vector<element_struct*>*> * stacks, char character, type_syntax_enum type_syntax)
{
	int * res = static_cast<int *>(stacks->back()->back()->element);
	*res = *res * 10;
	*res = *res + (character - '0');
}

vector<dice_element_struct*> * addition_dices(vector<dice_element_struct*> * dice_stack1, vector<dice_element_struct*> * dice_stack2)
{
	vector<dice_element_struct*> * dice_res = nullptr;
	dice_res = new vector<dice_element_struct*>;

	for (dice_element_struct * dice_element1 : *dice_stack1) {
		for (dice_element_struct * dice_element2 : *dice_stack2) {
			int value = dice_element1->value + dice_element2->value;
			bool found = false;
			for (dice_element_struct* dice_element_res : *dice_res)
			{
				if(dice_element_res->value == value)
				{
					dice_element_res->occurence = dice_element_res->occurence + dice_element1->occurence * dice_element2->occurence;
					found = true;
				}
			}
			if(!found)
			{
				struct dice_element_struct * dice_element_res = new struct dice_element_struct;
				dice_element_res->value = value;
				dice_element_res->occurence = dice_element1->occurence * dice_element2->occurence;	
				dice_res->push_back(dice_element_res);
			}
		}
	}
	return dice_res;
}

struct tree_rules * create_tree_rules()
{
	// all nodes
	// root 
	struct tree_rules * root = nullptr;
	root = new tree_rules;
	// node number add element 
	struct tree_rules * node_number_1 = nullptr;
	node_number_1 = new tree_rules;
	node_number_1->type_character = type_syntax_enum::integer;
	node_number_1->ptr_add_stack = &add_element;
	// node number add decimal number 
	struct tree_rules * node_number_2 = nullptr;
	node_number_2 = new tree_rules;
	node_number_2->type_character = type_syntax_enum::integer;
	node_number_2->ptr_add_stack = &add_multiply_and_element;
	// node number add multiply 
	struct tree_rules * node_number_3 = nullptr;
	node_number_3 = new tree_rules;
	node_number_3->type_character = type_syntax_enum::integer;
	node_number_3->ptr_add_stack = &add_decimal_number;
	// node ope 
	struct tree_rules * node_ope = nullptr;
	node_ope = new tree_rules;
	node_ope->type_character = type_syntax_enum::ope;
	node_ope->ptr_add_stack = &add_element;
	// node open_parenth 
	struct tree_rules * node_open_parenth_1 = nullptr;
	node_open_parenth_1 = new tree_rules;
	node_open_parenth_1->type_character = type_syntax_enum::open_parenth;
	node_open_parenth_1->ptr_add_stack = &add_element;
	// node node_node_open_parenth add multiply
	struct tree_rules * node_open_parenth_2 = nullptr;
	node_open_parenth_2 = new tree_rules;
	node_open_parenth_2->type_character = type_syntax_enum::open_parenth;
	node_open_parenth_2->ptr_add_stack = &add_multiply_and_element;
	// node node_close_parenth 
	struct tree_rules * node_close_parenth = nullptr;
	node_close_parenth = new tree_rules;
	node_close_parenth->type_character = type_syntax_enum::close_parenth;
	node_close_parenth->ptr_add_stack = &add_element;
	// node dice 
	struct tree_rules * node_dice = nullptr;
	node_dice = new tree_rules;
	node_dice->type_character = type_syntax_enum::dice;
	node_dice->ptr_add_stack = &add_element;
	// node dice arg 
	struct tree_rules * node_dice_arg = nullptr;
	node_dice_arg = new tree_rules;
	node_dice_arg->type_character = type_syntax_enum::integer;
	node_dice_arg->ptr_add_stack = &set_size_dice;

	// Link nodes 
	root->nodes.push_back(node_number_1);
	root->nodes.push_back(node_open_parenth_1);
	root->nodes.push_back(node_dice);	

	node_number_1->nodes.push_back(node_number_3);
	node_number_1->nodes.push_back(node_ope);
	node_number_1->nodes.push_back(node_open_parenth_2);
	node_number_1->nodes.push_back(node_close_parenth);
	node_number_1->nodes.push_back(node_dice);

	node_number_2->nodes.push_back(node_number_3);
	node_number_2->nodes.push_back(node_ope);
	node_number_2->nodes.push_back(node_open_parenth_2);
	node_number_2->nodes.push_back(node_close_parenth);
	node_number_2->nodes.push_back(node_dice);

	node_number_3->nodes.push_back(node_number_3);
	node_number_3->nodes.push_back(node_open_parenth_2);
	node_number_3->nodes.push_back(node_close_parenth);
	node_number_3->nodes.push_back(node_ope);
	node_number_3->nodes.push_back(node_dice);

	node_ope->nodes.push_back(node_number_1);
	node_ope->nodes.push_back(node_open_parenth_1);
	node_ope->nodes.push_back(node_dice);

	node_open_parenth_1->nodes.push_back(node_open_parenth_1);
	node_open_parenth_1->nodes.push_back(node_number_1);
	node_open_parenth_1->nodes.push_back(node_dice);

	node_open_parenth_2->nodes.push_back(node_number_1);
	node_open_parenth_2->nodes.push_back(node_open_parenth_1);
	node_open_parenth_2->nodes.push_back(node_dice);

	node_close_parenth->nodes.push_back(node_close_parenth);
	node_close_parenth->nodes.push_back(node_number_2);
	node_close_parenth->nodes.push_back(node_open_parenth_2);
	node_close_parenth->nodes.push_back(node_ope);
	
	node_dice->nodes.push_back(node_dice_arg);

	node_dice_arg->nodes.push_back(node_dice_arg);
	node_dice_arg->nodes.push_back(node_open_parenth_2);
	node_dice_arg->nodes.push_back(node_close_parenth);
	node_dice_arg->nodes.push_back(node_ope);

	return root;
}

void print_tab(int layer)
{
	for(int i = -1; i < layer; i++)
		cout << " ";
}

void print_stack(vector<element_struct *> * stack, int layer)
{
	print_tab(layer);	
	cout << ">>" << endl;
	if(stack == nullptr)
	{
		return;
	}

	for (element_struct * element : *stack) 
	{
		if(element->type_element == type_element_enum::stack)
		{
			vector<element_struct *> * vect_cast = static_cast<vector<element_struct *>*>(element->element);
			print_stack(vect_cast,layer + 1);
		}
		if(element->type_element == type_element_enum::number)
		{
			int * integer_cast = static_cast<int *>(element->element);
			print_tab(layer);
			cout << *integer_cast << endl;
		}
		if(element->type_element == type_element_enum::operator_bin)
		{
			char * char_cast = static_cast<char *>(element->element);
			print_tab(layer);
			cout << *char_cast << endl;
		}
		if(element->type_element == type_element_enum::dice_stack)
		{
			vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(element->element);
			print_tab(layer);
			cout << "dice:";
			for (dice_element_struct * dice_element : *dice_stack) {
				cout << "|" << dice_element->value << " " << dice_element->occurence << "|";
			}
			cout << " " << endl;
		}
	}
	print_tab(layer);
	cout << "<<" << endl;
}


bool compare_dice_element(struct dice_element_struct * dice_element1, struct dice_element_struct * dice_element2)
{
	return dice_element1->value < dice_element2->value;
}

void print_res(vector<element_struct *> *stack)
{
	if(stack->size() > 1)
	{
		cerr << "Reduction Error" << endl;
		print_stack(stack,0);
	}
	if(stack->back()->type_element == type_element_enum::number)
	{
		cout << *(static_cast<int *>(stack->back()->element)) << " 100.00" <<endl;
	}
	if(stack->back()->type_element == type_element_enum::dice_stack)
	{
		vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(stack->back()->element);
		int sum_element = 0;
		for (dice_element_struct * dice_element : *dice_stack) {
			sum_element += dice_element->occurence;
		}
		// cout << "sum_element: " << sum_element << endl;
		sort(dice_stack->begin(),dice_stack->end(),compare_dice_element);	
		for (dice_element_struct * dice_element : *dice_stack) {
			cout.precision(2);
			cout << dice_element->value << " " << std::fixed << (((double) dice_element->occurence)/((double) sum_element))*100 << endl;
			// cout << "dice_element->occurence: " << dice_element->occurence << endl;	
		}	
	}
}

struct element_struct * reduce_stack(struct element_struct * arg)
{
	if(arg->type_element == type_element_enum::stack)
	{
		vector<element_struct *> * stack = static_cast<vector<element_struct *>*>(arg->element);
		apply_reduct_stack(stack);
		if(stack->size() > 1)
		{
			cerr << "stack not reduce correctly" << endl;
		}
		arg = stack->back();
	}
	return arg;
}

struct element_struct * eval(char operator_bin, struct element_struct * arg1, struct element_struct * arg2)
{
	struct element_struct * res_arg1 = reduce_stack(arg1);
	struct element_struct * res_arg2 = reduce_stack(arg2);
	if(res_arg1->type_element == type_element_enum::stack || res_arg2->type_element == type_element_enum::stack)
	{
		cerr << "Error: stack not reduce correctly" << endl;
	}
	struct element_struct * res = nullptr;
	res = new element_struct;
	
	if(operator_bin == '-')
	{
		if(res_arg1->type_element == type_element_enum::number)
		{
			if(res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::number;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				int * res_int2 = static_cast<int*>(res_arg2->element);
				int * res_int = new int;
				*res_int = *res_int1 - *res_int2;
				res->element = res_int;
				return res;
			}
			if(res_arg2->type_element == type_element_enum::dice_stack)
			{

				res->type_element = type_element_enum::dice_stack;
				int * res_int = static_cast<int*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg2->element);
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = (*res_int) - dice_element->value;
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}
				res->element = dice_res;
				return res;
			}
		}
		if (res_arg1->type_element == type_element_enum::dice_stack) 
		{
			if(res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int = static_cast<int*>(res_arg2->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
				
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = dice_element->value - *res_int;
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}
				res->element = dice_res;
				return res;
			}
			if(res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;

				vector<dice_element_struct*> * dice_stack1 = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack2 = static_cast<vector<dice_element_struct*>*>(res_arg2->element);

				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;

				for (dice_element_struct * dice_element1 : *dice_stack1) {
					for (dice_element_struct * dice_element2 : *dice_stack2) {
						int value = dice_element1->value - dice_element2->value;
						bool found = false;
						for (dice_element_struct* dice_element_res : *dice_res)
						{
							if(dice_element_res->value == value)
							{
								dice_element_res->occurence = dice_element_res->occurence + dice_element1->occurence * dice_element2->occurence;
								found = true;
							}
						}
						if(!found)
						{
							struct dice_element_struct * dice_element_res = new struct dice_element_struct;
							dice_element_res->value = value;
							dice_element_res->occurence = 0;
							dice_element_res->occurence = dice_element1->occurence * dice_element2->occurence;
							dice_res->push_back(dice_element_res);
						}
					}
				}
				res->element = dice_res;
				return res;
			}
		
		}

	}

	if(operator_bin == '+')
	{
		if(res_arg1->type_element == type_element_enum::number)
		{
			if(res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::number;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				int * res_int2 = static_cast<int*>(res_arg2->element);
				int * res_int = new int;
				*res_int = *res_int1 + *res_int2;
				res->element = res_int;
				return res;
			}
			if(res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int = static_cast<int*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg2->element);
				
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = dice_element->value + *res_int;
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}
				res->element = dice_res;
				return res;
			}
		}
		if (res_arg1->type_element == type_element_enum::dice_stack) 
		{
			if(res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int = static_cast<int*>(res_arg2->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = dice_element->value + *res_int;
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}
				res->element = dice_res;
				return res;
			}
			if(res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;
				vector<dice_element_struct*> * dice_stack1 = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack2 = static_cast<vector<dice_element_struct*>*>(res_arg2->element);
				vector<dice_element_struct*> * dice_res = nullptr;

				dice_res = addition_dices(dice_stack1, dice_stack2);
				res->element = dice_res;
				return res;
			}
		
		}
	}
	if(operator_bin == '*')
	{
		if (res_arg1->type_element == type_element_enum::number)
		{
			if (res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::number;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				int * res_int2 = static_cast<int*>(res_arg2->element);
				int * res_int = new int;

				*res_int = (*res_int1) * (*res_int2); 
				res->element = res_int;
				return res;
			}
			if (res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg2->element);
				
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
					
				int i = *res_int1;
				if (i < 0) {
					cerr << "multiply by negative number not implemented" << endl;
				}
				if (i == 0)
				{
					res->type_element = type_element_enum::number;
					res->element = new int(0);
					return res;
				}
				
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = dice_element->value * i; 
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}	
				
				res->element = dice_res;
				return res;
			}
		}
		if (res_arg1->type_element == type_element_enum::dice_stack)
		{
			if (res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int2 = static_cast<int*>(res_arg2->element);
				vector<dice_element_struct*> * dice_stack = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				
				vector<dice_element_struct*> * dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;
					
				int i = *res_int2;
				if (i < 0) {
					cerr << "multiply by negative number not implemented" << endl;
				}
				if (i == 0)
				{
					res->type_element = type_element_enum::number;
					res->element = new int(0);
					return res;
				}
				
				for (dice_element_struct * dice_element : *dice_stack) {
					struct dice_element_struct * dice_element_res = new struct dice_element_struct;
					dice_element->value = dice_element->value * i; 
					dice_element->occurence = dice_element->occurence;
					dice_res->push_back(dice_element);
				}	
				
				res->element = dice_res;
				return res;
			}
			if (res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;
				vector<dice_element_struct*>* dice_stack1 = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				vector<dice_element_struct*>* dice_stack2 = static_cast<vector<dice_element_struct*>*>(res_arg2->element);

				vector<dice_element_struct*>* dice_res = nullptr;
				dice_res = new vector<dice_element_struct*>;

				for (dice_element_struct * dice_element1 : *dice_stack1) {
					for (dice_element_struct * dice_element2 : *dice_stack2) {
						int value = dice_element1->value * dice_element2->value;
						bool found = false;
						for (dice_element_struct* dice_element_res : *dice_res)
						{
							if(dice_element_res->value == value)
							{
								dice_element_res->occurence = dice_element_res->occurence + dice_element1->occurence * dice_element2->occurence;
								found = true;
							}
						}
						if(!found)
						{
							struct dice_element_struct * dice_element_res = new struct dice_element_struct;
							dice_element_res->value = value;
							dice_element_res->occurence = 0;
							dice_element_res->occurence = dice_element1->occurence * dice_element2->occurence;
							dice_res->push_back(dice_element_res);
						}
					}
				}
				res->element = dice_res;
				return res;
			}
		}
		cerr << "error type * : arg1 " << res_arg1->type_element << " or arg2 " << res_arg2->type_element << " not found" << endl;
		return nullptr;
	}
	if(operator_bin == '>')
	{
		if (res_arg1->type_element == type_element_enum::number)
		{
			if (res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::number;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				int * res_int2 = static_cast<int*>(res_arg2->element);
				int * res_int = new int;

				*res_int = (int) (*res_int1 > *res_int2); 
				res->element = res_int;
				return res; 
			}
			if (res_arg2->type_element == type_element_enum::dice_stack)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int1 = static_cast<int*>(res_arg1->element);
				vector<dice_element_struct*> * dice_stack2 = static_cast<vector<dice_element_struct*>*>(res_arg2->element);
				
				vector<dice_element_struct*> * dice_res = new vector<dice_element_struct*>;
			
				struct dice_element_struct * dice_element_res0 = new struct dice_element_struct;
				dice_element_res0->value = 0; 
				dice_element_res0->occurence = 0; 
				dice_res->push_back(dice_element_res0);
			
				struct dice_element_struct * dice_element_res1 = new struct dice_element_struct;
				dice_element_res1->value = 1;
				dice_element_res1->occurence = 0; 
				dice_res->push_back(dice_element_res1);

				for (dice_element_struct* dice_element : *dice_stack2)
				{
					bool found = false;
					for (dice_element_struct* dice_element_res : *dice_res)
					{
						if(dice_element_res->value == (*res_int1 > dice_element->value))
						{
							dice_element_res->occurence = dice_element_res->occurence + dice_element->occurence;
							found = true;
						}
					}
					if (!found)
					{
						cerr << "error compare two dice" << endl;
					}
				}
				res->element = dice_res;
				return res;
			}
		}
		if (res_arg1->type_element == type_element_enum::dice_stack)
		{
			if (res_arg2->type_element == type_element_enum::number)
			{
				res->type_element = type_element_enum::dice_stack;
				int * res_int2 = static_cast<int*>(res_arg2->element);
				vector<dice_element_struct*> * dice_stack1 = static_cast<vector<dice_element_struct*>*>(res_arg1->element);
				
				vector<dice_element_struct*> * dice_res = new vector<dice_element_struct*>;
			
				struct dice_element_struct * dice_element_res0 = new struct dice_element_struct;
				dice_element_res0->value = 0; 
				dice_element_res0->occurence = 0; 
				dice_res->push_back(dice_element_res0);
			
				struct dice_element_struct * dice_element_res1 = new struct dice_element_struct;
				dice_element_res1->value = 1;
				dice_element_res1->occurence = 0; 
				dice_res->push_back(dice_element_res1);
				for (dice_element_struct* dice_element : *dice_stack1)
				{
					for (dice_element_struct* dice_element_res : *dice_res)
					{
						if(dice_element_res->value == (dice_element->value > *res_int2))
						{
							dice_element_res->occurence = dice_element_res->occurence + dice_element->occurence;
						}
					}
				}
				res->element = dice_res;
				return res;

			}
			if (res_arg2->type_element == type_element_enum::dice_stack)
			{
				cerr << "not implemented compare dice stack with dice stack" << endl;	
			}
		}
	}
	cerr << "error operator " << operator_bin  << " not existed" << endl;
	return nullptr;
}

void reduction(char operator_bin, vector<element_struct *> * stack)
{
	int current_layer = 0;

	while(stack->size() > current_layer) 
	{
		element_struct * element = stack->at(current_layer);
		
		if(element->type_element == type_element_enum::stack)
		{
			vector<element_struct *> * stack_reduc = static_cast<vector<element_struct *>*>(element->element);
			apply_reduct_stack(stack_reduc);
			// delete memory
			struct element_struct * element_reduc = new element_struct;
			element_reduc->type_element = stack_reduc->back()->type_element; 
			element_reduc->element = stack_reduc->back()->element; 
			stack->at(current_layer) = element_reduc; 
		}
		if(element->type_element == type_element_enum::number || element->type_element == type_element_enum::dice_stack)
		{
			current_layer ++;
		}

		if(element->type_element == type_element_enum::operator_bin)
		{
			char * char_cast = static_cast<char *>(element->element);
			if(operator_bin == *char_cast)
			{
				struct element_struct * res_element = new element_struct;

				res_element = eval(operator_bin, stack->at(current_layer-1),stack->at(current_layer+1));

				int delete_index = current_layer + 1;
				stack->erase(stack->begin() + delete_index);
				// delete memory
				delete_index --;
				stack->erase(stack->begin() + delete_index);
				// delete memory
				delete_index --;
				stack->erase(stack->begin() + delete_index);
				// delete mememory

				stack->emplace(stack->begin() + delete_index, res_element);
				current_layer --;
			}
			else {
				current_layer ++;
			}
		}
	}
	
}

tree_rules * get_node_character(tree_rules * node, type_syntax_enum type_syntax)
{
	for (tree_rules * node : node->nodes) {
		if(node->type_character == type_syntax)
		{
			return node;
		}
	}
	return nullptr;
}
