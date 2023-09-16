#include <stdio.h>

#define TOK_END 0
#define TOK_INT 1
#define TOK_OP 2

#define OP_PLUS 0
#define OP_MUL 1

// 7 + 2 + 3 * 3
int expression(int rbp);

int pos = 0;

struct TOKEN {
	int Type;
	int Val;
} test[] = {
	{ TOK_INT, 7 },
	{ TOK_OP, OP_PLUS },
	{ TOK_INT, 7 },
	{ TOK_OP, OP_PLUS },
	{ TOK_INT, 3 },
	{ TOK_OP, OP_MUL },
	{ TOK_INT, 3 }
};

struct TOKEN tok;

int Op_Add(int left, int right)
{
	return left + right;
}

int Op_Mul(int left, int right)
{
	return left * right;
}

struct OPERATOR {
	int Precedence;
	void* func;
} Ops[] = {
	{ 30, &Op_Add },
	{ 40, &Op_Mul }
};

int lbp(struct TOKEN token)
{
	if (token.Type == TOK_OP)
		return Ops[token.Val].Precedence;
	return 0;
}

int led(struct TOKEN token, int left)
{
	int right = expression(lbp(token));
	int (*fun)(int, int) = Ops[token.Val].func;
	return fun(left, right);
}

int nud(struct TOKEN token)
{
	if (token.Type == TOK_INT)
		return token.Val;
	return -1;
}

int expression(int rbp)
{
	struct TOKEN token = tok;
	int left = nud(tok);
	tok = test[++pos];

	while (lbp(tok) > rbp) {
		token = tok;
		tok = test[++pos];
		left = led(token, left);
	}

	return left;
}

int main(int argc, char** argv)
{
	tok = test[0];

	printf("%i\n", expression(0));
	return 0;
}