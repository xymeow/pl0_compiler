// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include "pl0.h"

void error(int n){
    long i;

    printf(" ****");
    //printf("%u\n", type);
    for (i = 1; i <= cc - 1; i++){
	printf(" ");
    }
    printf("^Error %d: %s\n",n, err_msg[n]);
    err++;
}

void getch() {
    if(cc==ll){
		if(feof(infile)){
		    printf("************************************\n");
		    printf("      program incomplete\n");
		    printf("************************************\n");
		    exit(1);
		}
		ll = 0; cc = 0;
		printf("%5d ", cx);
		while((!feof(infile)) && ((ch = getc(infile)) != '\n')){
		    printf("%c",ch);
		    ll = ll + 1; line[ll] = ch;
		}
		printf("\n");
		ll = ll + 1; line[ll] = ' ';
    }
    cc = cc + 1; ch = line[cc];
}

void getsym(){
    long i,j,k;

    while(ch == ' '||ch == '\t'){
    	// if (ch == '/') //notes
    	// {
    	// 	getch();
    	// 	if(ch == '*'){
    	// 		while(1){
    	// 			while(ch != '*')
    	// 				getch();
	    // 			getch();
	    // 			if(ch == '/')
	    // 				break;
    	// 		}
    	// 		getch();
    	// 	}
    	// 	else
    	// 		sym = slash;
    	// }
    	// else
			getch();
    }
    if(isalpha(ch)){ 	// identified or reserved
		k = 0;
		do{
		    if(k < al){
			a[k] = ch; 
			k = k + 1;
		    }
		    getch();
		}while(isalpha(ch)||isdigit(ch));

		if(k >= kk){
		    kk = k;
		}
		else{
		    do{
			kk = kk - 1; 
			a[kk] = ' ';
		    }while(k < kk);
		}
		strcpy(id, a);
		i = 0; 
		j = norw - 1;
		do{
		    k = (i+j)/2;
		    if(strcmp(id, word[k]) <= 0){
				j = k - 1;
		    }
		    if(strcmp(id, word[k]) >= 0){
				i = k + 1;
		    }
		}while(i <= j);
		if(i - 1 > j){
		    sym = wsym[k];
		}
		else{
		    sym = ident;
		}
    }
    else if(isdigit(ch)){ // number
		k = 0; 
		num = 0; 
		sym = number;
		do{
		    num = num * 10 + (ch - '0');
		    k = k + 1; 
		    getch();
		}while(isdigit(ch));
		if(k > nmax){
		    error(31);
		}
    }
    else if(ch == ':'){
		getch();
		if(ch == '='){
		    sym = becomes; 
		    getch();
		}else{
		    sym = nul;
		}
    }
    else if (ch == '/') //notes
	{
		getch();
		if(ch == '*'){
			while(1){
				while(ch != '*')
					getch();
    			getch();
    			if(ch == '/')
    				break;
			}
			getch();
		}
		else
			sym = slash;
	}
    else if(ch == '<'){
		getch();
		if(ch == '='){
		    sym = leq; 
		    getch();
		}
		else if(ch == '>'){
		    sym = neq; 
		    getch();
		}
		else{
		    sym = lss;
		}
    }
    else if(ch == '>'){
		getch();
		if(ch == '='){
		    sym = geq; 
		    getch();
		}
		else{
		    sym = gtr;
		}
	}
	else if(ch == '+'){
		getch();
		if (ch == '+')
		{
			sym = inc;
			getch();
		}
		else if(ch == '='){
			sym = pluseq;
			getch();
		}
		else{
			sym = plus;
		}
	}
	else if(ch == '-'){
		getch();
		if(ch == '-'){
			sym = dec;
			getch();
		}
		else if(ch == '='){
			sym = minuseq;
			getch();
		}
		else{
			sym = minus;
		}
	}
	else if(ch == '['){
		getch();
		getsym();
		sum = num;
		if(ch == ']'){
			getch();
			sym = arraysym;
		}
		else
			error(28);
		if(sum < 1)
			error(29);
	}
	else{
		sym = ssym[(unsigned char)ch]; 
		getch();
    }
}

void gen(enum fct x, long y, long z){	//generates an instruction
    if(cx > cxmax){
		printf("program too long\n");
		exit(1);
    }
    code[cx].f = x; 
    code[cx].l = y; 
    code[cx].a = z;
    cx = cx + 1;
}

// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(unsigned long s1, unsigned long s2, long n){
    if (!(sym & s1)){
		error(n);
		s1 = s1|s2;
		while(!(sym & s1))
		    getsym();	
    }
}

int position(char* id);
void enter(enum object k){		// enter object into table
	long i;
	i = position(id);
	if (i > 0 && table[i].level == lev)
	{
		error(26);
		return;
	}
    tx = tx + 1;
    strcpy(table[tx].name, id);
    table[tx].kind = k;
    switch(k){
		case constant:
		    if(num > amax){
				error(31);
				num = 0;
		    }
		    table[tx].val = num;
		    break;
		case variable:
		    table[tx].level = lev; 
		    table[tx].addr = dx; 
		    dx = dx + 1;
		    table[tx].size = 0;
		    break;
		case proc:
		    table[tx].level = lev;
		    break;
		case bool:
			table[tx].level = lev;
			table[tx].addr = dx;
			dx ++;
			break;
		case array:
			table[tx].level = lev;
			table[tx].addr = dx - arraysize;
			table[tx].size = arraysize;
			break;
    }
}

int position(char* id){	// find identifier id in table
    int i;

    strcpy(table[0].name, id);
    i = tx;
    while(strcmp(table[i].name, id) != 0){
		i = i - 1;
    }
    if(ch == '[')
    	getsym();
    if(sym == rsparen){
    	if(table[i].size > sum)
    		i = i + sum + 1;
    	else
    		error(29);
    }
    sum = 0;
    return i;
}

void constdeclaration(){
    if(sym == ident){
		getsym();
		if(sym == eql||sym == becomes){
		    if(sym == becomes){
				error(1);
		    }
		    getsym();
		    if(sym == number){
				enter(constant); 
				getsym();
		    }
		    else{
			error(2);
		    }
		}
		else{
		    error(3);
		}
    }
    else{
		error(4);
    }
}

void vardeclaration(){
	int i;
	char temp[al+1];
    if(sym == ident){
    	strcpy(temp, id);
    	getsym();
    	if (sym == lsparen)
    	{
    		getsym();
    		if (sym == number)
    		{
    			dx += num;
    			arraysize = num;
    		}
    		else{
    			if (sym == ident)
    			{
    				i = position(id);
    				if (i == 0)
    				{
    					error(11);
    				}
    				else{
    					if (table[i].kind == constant)
    					{
    						dx += table[i].val;
    						arraysize = table[i].val;
    					}
    					else
    						error(27);
    				}
    			}
    			else
    				error(27);
    		}
    		strcpy(id, temp);
    		enter(array);
    		getsym();
    		if (sym != rsparen)
    		{
    			error(28);
    		}
    		else
    			getsym();
    	}
    	else if(sym == comma||sym == semicolon){
			enter(variable); 
    	}
    }
    else
		error(4);
}

void booldeclaration(){
	if (sym == ident)
	{
		enter(bool);
		getsym();
	}
	else
		error(4);
}

void listcode(long cx0){	// list code generated for this block
    int i;

    for(i = cx0; i <= cx - 1; i++){
		fprintf(midfile, "%10d%5s%3ld%5ld\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
    }

}

void expression(unsigned long);
void boolexp(unsigned long);
void factor(unsigned long fsys){
    long i;
	
    test(facbegsys, fsys, 24);
    while(sym & facbegsys){
		if(sym == ident){
		    i = position(id);
		    if(i == 0){
				error(11);
		    }
		    else{
		    	type1 = table[i].kind;
				switch(type1){
				    case constant:
				    	if(type == bool)
				    		error(12);
				    	else
							gen(lit, 0, table[i].val);
						break;
				    case variable:
				    	if(type == bool)
				    		error(12);
				    	else
							gen(lod, lev - table[i].level, table[i].addr);
						break;
				    case proc:
						error(21);
						break;
					case bool:
						if(type != bool)
							error(12);
						else
							gen(lod, lev - table[i].level, table[i].addr);
						break;
					case array:
						getsym();
						if (sym == lsparen)
						{
							getsym();
							expression(fsys|rsparen);
							gen(tra, 0, table[i].size);
							gen(jug, 0, 0);
							gen(rda, lev - table[i].level, table[i].addr);
							if (sym != rsparen)
							{
								error(28);
							}
							else
								getsym();

						}
						break;
				}
		    }
		    getsym();
			if (sym == inc)
		    {
		    	gen(lit, lev - table[i].level, 1);
		    	gen(opr, lev - table[i].level, 2);
		    	if(type1 == array)
		    		gen(wta, lev - table[i].level, table[i].addr);
		    	else
		    		gen(sto, lev - table[i].level, table[i].addr);
		    	if(type1 == array)
		    		gen(rda, lev - table[i].level, table[i].addr);
		    	else
					gen(lod, lev - table[i].level, table[i].addr);
				gen(lit, 0, 1);
				gen(opr, 0, 3);
				getsym();
		    }
		    else if(sym == dec){
		    	gen(lit, lev - table[i].level, 1);
		    	gen(opr, lev - table[i].level, 3);
		    	if(type1 == array)
		    		gen(wta, lev - table[i].level, table[i].addr);
		    	else
		    		gen(sto, lev - table[i].level, table[i].addr);
		    	if(type1 == array)
		    		gen(rda, lev - table[i].level, table[i].addr);
		    	else
					gen(lod, lev - table[i].level, table[i].addr);
				gen(lit, 0, 1);
				gen(opr, 0, 2);
				getsym();
		    }
		}
		else if(sym == inc){
			getsym();
			if(sym == ident){
				getsym();
				i = position(id);
				type1 = table[i].kind;
				if (i == 0)
				{
					error(11);
				}
				else{
					if (type1 != variable && type1 != array)
					{
						error(12);
					}
					else{
						if(type1 == array)
		    				gen(rda, lev - table[i].level, table[i].addr);
				    	else
							gen(lod, lev - table[i].level, table[i].addr);
						gen(lit, 0, 1);
						gen(opr, 0, 2);
			    		if(type1 == array)
				    		gen(wta, lev - table[i].level, table[i].addr);
				    	else
				    		gen(sto, lev - table[i].level, table[i].addr);
				    	if(type1 == array)
				    		gen(rda, lev - table[i].level, table[i].addr);
				    	else
							gen(lod, lev - table[i].level, table[i].addr);
					}
				}
			}
		}
		else if(sym == dec){
			getsym();
			if(sym == ident){
				getsym();
				i = position(id);
				type1 = table[i].kind;
				if (i == 0)
				{
					error(11);
				}
				else{
					if (table[i].kind != variable)
					{
						error(12);
					}
					else{
			    		if(type1 == array)
				    		gen(rda, lev - table[i].level, table[i].addr);
				    	else
							gen(lod, lev - table[i].level, table[i].addr);
						gen(lit, 0, 1);
						gen(opr, 0, 3);
			    		if(type1 == array)
				    		gen(wta, lev - table[i].level, table[i].addr);
				    	else
				    		gen(sto, lev - table[i].level, table[i].addr);
				    	if(type1 == array)
				    		gen(rda, lev - table[i].level, table[i].addr);
				    	else
							gen(lod, lev - table[i].level, table[i].addr);
					}
				}
			}
		}
		else if(sym == number){
			if(type != variable && type != array)
				error(12);
			else{
				if(num > amax){
					error(31); 
					num = 0;
			    }
			    gen(lit, 0, num);
			    getsym();
			}
		}
		else if(sym == truesym){
			if(type != bool)
				error(12);
			else{
				gen(lit, 0, 1);
				getsym();
			}
		}
		else if(sym == falsesym){
			if(type != bool)
				error(12);
			else{
				gen(lit, 0, 0);
				getsym();
			}
			
		}
		else if(sym == lparen){
			    getsym();
			    if(type == bool)
			    	boolexp(rparen|fsys);
			    else
			    	expression(rparen|fsys);
		    if(sym == rparen){
				getsym();
		    }
		    else{
				error(22);
		    }
		}
		test(fsys, lparen, 23);
    }
}

void term(unsigned long fsys){
    unsigned long mulop;

    factor(fsys|times|slash);
    while(sym == times||sym == slash){
		mulop = sym; 
		getsym();
		factor(fsys|times|slash);
		if(mulop == times){
		    gen(opr, 0, 4);
		}
		else{
		    gen(opr, 0, 5);
		}
    }
}

void expression(unsigned long fsys){
	unsigned long addop;
	//type = variable;
	if(sym == plus||sym == minus){
		addop = sym; 
		getsym();
		term(fsys|plus|minus);
		if(addop == minus){
		    gen(opr, 0, 1); //neg
		}
	}
	else{
		term(fsys|plus|minus);
	}
	while(sym == plus||sym == minus){
		addop = sym; 
		getsym();
		term(fsys|plus|minus);
		if(addop == plus){
		    gen(opr,0,2); //+
		}
		else{
		    gen(opr,0,3); //-
		}
	}
}

// void boolterm(unsigned long fsys){

// }

void boolexp(unsigned long fsys){
	//type = bool;
	unsigned long op;
	if (sym == notsym)
	{	
		getsym();
		factor(fsys|notsym|andsym|orsym);
		gen(opr, 0, 14);
	}
	else
		factor(fsys|notsym|andsym|orsym);
	while(sym == andsym || sym == orsym || sym == notsym){
		op = sym;
		getsym();
		factor(fsys|notsym|andsym|orsym);
		switch(op){
			case andsym:
				gen(opr, 0, 15); break;
			case orsym:
				gen(opr, 0, 16); break;
			case notsym:
				gen(opr, 0, 14); break;
		}
	}
}

void condition(unsigned long fsys){
    unsigned long relop;

    if(sym == oddsym){
		getsym(); 
		expression(fsys);
		gen(opr, 0, 6);
    }
    else{
		expression(fsys|eql|neq|lss|gtr|leq|geq);
		if(!(sym&(eql|neq|lss|gtr|leq|geq))){
		    error(20);
		}
		else{
		    relop = sym; 
		    getsym();
		    expression(fsys);
		    switch(relop){
				case eql:
				    gen(opr, 0, 8);
				    break;
				case neq:
				    gen(opr, 0, 9);
				    break;
				case lss:
				    gen(opr, 0, 10);
				    break;
				case geq:
				    gen(opr, 0, 11);
				    break;
				case gtr:
				    gen(opr, 0, 12);
				    break;
				case leq:
				    gen(opr, 0, 13);
				    break;
		    }
		}
    }
}

void statement(unsigned long fsys){
    long i, cx1, cx2, i2;

    if(sym == ident){
		i = position(id);
		if(i == 0){
		    error(11);
		}
		else if(table[i].kind != variable && table[i].kind != bool && table[i].kind != array){	// assignment to non-variable
		    error(12); 
		    i = 0;
		}
		type = table[i].kind;
		getsym();
		if (sym == lsparen)
		{
			getsym();
			expression(fsys|rsparen);
			gen(tra, 0, table[i].size);
			gen(jug, 0, 0);
			if (sym != rsparen)
			{
				error(28);
			}
			getsym();
		}
		if(sym == becomes){
		    getsym();
		    if(type == variable)
		    	expression(fsys);
		    else 
		    	boolexp(fsys);
			if(i != 0){
				if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
			}
		}
		else if(sym == inc){
    		getsym();
    		gen(lit, 0, 1);
			gen(lod, lev - table[i].level, table[i].addr);
    		gen(opr, 0, 2);
	    	if(i != 0){
	    		if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
	    	}
	    }
	    else if(sym == dec){
    		getsym();
    		gen(lit, 0, -1);
			gen(lod, lev - table[i].level, table[i].addr);
    		gen(opr, 0, 2);
	    	if(i != 0){
	    		if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
	    	}
	    }
	    else if(sym == pluseq){
	    	getsym();
	    	expression(fsys);
	    	if(i != 0){
	    		gen(opr, 0, 2);
	    		if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
	    	}
	    }
	    else if(sym == minuseq){
	    	getsym();
	    	expression(fsys);
	    	if(i != 0){
	    		gen(opr, 0, 3);
	    		if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
	    	}
	    }
		else{
		    error(13);
		}
    }
    else if(sym == inc){
    	getsym();
    	if(sym == ident){
    		i = position(id);
    		if(i == 0)
    			error(11);
    		else{
    			if(table[i].kind != variable){    				
    				error(12);
    				i = 0;
    			}
    			else{
    				gen(lod, lev - table[i].level, table[i].addr);
    				gen(lit, 0, 1);
    				gen(opr, 0, 2);
    			if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
    				getsym();
    			}
    		}
    	}
    }
    else if(sym == dec){
    	getsym();
    	if(sym == ident){
    		i = position(id);
    		if (i == 0)
    		{
    			error(11);
    		}
    		else{
    			if (table[i].kind != variable)
    			{
    				error(12);
    				i = 0;
    			}
    			else{
    				gen(lod, lev - table[i].level, table[i].addr);
    				gen(lit, 0, 1);
    				gen(opr, 0, 3);
    			if (type == array)
					gen(wta, lev - table[i].level, table[i].addr);
				else
			    	gen(sto, lev - table[i].level, table[i].addr);
    				getsym();
    			}
    		}
    	}
    }
    else if(sym == callsym){
		getsym();
		if(sym != ident){
		    error(14);
		}
		else{
		    i = position(id);
		    if(i == 0){
				error(11);
		    }
		    else if(table[i].kind == proc){
				gen(cal, lev - table[i].level, table[i].addr);
		    }else{
				error(15);
		    }
		    getsym();
		}
    }
    else if(sym == ifsym){
		getsym(); 
		condition(fsys|thensym|dosym);
		if(sym == thensym){
		    getsym();
		}
		else{
		    error(16);
		}
		cx1=cx;	
		gen(jpc, 0, 0);
		statement(fsys);
		code[cx1].a = cx + 1;	
		cx2 = cx;
		gen(jmp, 0, cx + 1);
		if (sym == semicolon)
		{
			getsym();
		}
		if (sym == elsesym)
		{
			getsym();
			statement(fsys);
			code[cx2].a = cx;
		}
		else
			statement(fsys);
    }
    else if(sym == beginsym){
		getsym(); 
		statement(fsys|semicolon|endsym);
		while(sym == semicolon||(sym & statbegsys)){
		    if(sym == semicolon){
				getsym();
		    }
		    else{
			error(10);
		    }
		    statement(fsys|semicolon|endsym);
		}
		if(sym == endsym){
		    getsym();
		}else{
		    error(17);
		}
    }
    else if(sym == whilesym){
		cx1 = cx; 
		getsym();
		condition(fsys|dosym);
		cx2 = cx;	
		gen(jpc, 0, 0);
		if(sym == dosym){
		    getsym();
		}
		else{
		    error(18);
		}
		statement(fsys); 
		gen(jmp, 0, cx1);
		code[cx2].a = cx;
    }
    else if (sym == repsym)
    {
    	cx1 = cx;
    	getsym();
    	while(sym != untsym){
    		statement(fsys|semicolon|untsym);
    		if (sym == semicolon)
    		{
    			getsym();
    		}
    	}
		if (sym == untsym)
		{
			getsym();
			condition(fsys|semicolon);
			gen(jpc, 0, cx1);
		}
		else{
			error(8);
		}
    }
    else if(sym == forsym){
    	getsym();
    	if (sym == ident)
    	{
    		i = position(id);
    		if (i == 0)
    		{
    			error(11);
    		}
    		else{
    			if (table[i].kind != variable)
    			{
    				error(12);
    				i = 0;
    			}
    			else{
    				getsym();
    				if (sym != becomes)
    					error(13);
    				else
    					getsym();
    				expression(fsys|tosym|downtosym);
    				gen(sto, lev - table[i].level, table[i].addr);
    				if (sym == tosym)
    				{
    					getsym();
    					cx1 = cx;
    					gen(lod, lev - table[i].level, table[i].addr);
    					expression(fsys|dosym);
    					gen(opr, 0, 13);
    					cx2 = cx;
    					gen(jpc, 0, 0);
    					if (sym == dosym)
    					{
    						getsym();
    						statement(fsys);
    						gen(lod, lev - table[i].level, table[i].addr);
    						gen(lit, 0, 1);
    						gen(opr, 0, 2);
    						gen(sto, lev - table[i].level, table[i].addr);
    						gen(jmp, 0, cx1);
    						code[cx2].a = cx;
    					}
    					else
    						error(18);
    				}
					else if(sym == downtosym){
						getsym();
						cx1 = cx;
						gen(lod, lev - table[i].level, table[i].addr);
						expression(fsys|dosym);
						gen(opr, 0, 11);
						cx2 = cx;
						gen(jpc, 0, 0);
						if (sym == dosym)
						{
							getsym();
							statement(fsys);
							gen(lod, lev - table[i].level, table[i].addr);
							gen(lit, 0, 1);
							gen(opr, 0, 3);
							gen(sto, lev - table[i].level, table[i].addr);
							gen(jmp, 0, cx1);
							code[cx2].a = cx;
						}
						else
							error(18);
					
					}
    			}
    		}
    	}
    }
    test(fsys, 0, 19);
}
			
void block(unsigned long fsys){
    long tx0;		// initial table index
    long cx0; 		// initial code index
    long tx1;		// save current table index before processing nested procedures
    long dx1;		// save data allocation index

    dx = 3; 
    tx0 = tx; 
    table[tx].addr = cx; 
    gen(jmp, 0, 0);
    if(lev > levmax){
		error(32);
    }
    do{
		if(sym == constsym){
		    getsym();
		    do{
				constdeclaration();
				while(sym == comma){
				    getsym(); 
				    constdeclaration();
				}
				if(sym == semicolon){
				    getsym();
				}
				else{
				    error(5);
				}
		    }while(sym == ident);
		}
		if(sym == varsym){
		    getsym();
		    do{
				vardeclaration();
				if(sym == arraysym){
					enter(array);
					getsym();
				}
				while(sym == comma){
				    getsym(); 
				    vardeclaration();
				    if (sym == arraysym)
				    {
				    	enter(array);
				    	getsym();
				    }
				}
				if(sym == semicolon){
				    getsym();
				}
				else{
				    error(5);
				}
		    }while(sym == ident);
		}
		if (sym == boolsym)
		{
			getsym();
			do{
				booldeclaration();
				while(sym == comma){
					getsym();
					booldeclaration();
				}
				if (sym == semicolon)
				{
					getsym();
				}
				else
					error(5);
			}while(sym == ident);
		}
		while(sym == procsym){
		    getsym();
		    if(sym == ident){
				enter(proc); 
				getsym();
		    }
		    else{
				error(4);
		    }
		    if(sym == semicolon){
				getsym();
		    }else{
				error(5);
		    }
		    lev ++;
		    tx1 = tx;  
		    dx1 = dx;
		    block(fsys|semicolon);
		    lev++; 
		    tx = tx1; 
		    dx = dx1;
		    if(sym == semicolon){
				getsym();
				test(statbegsys|ident|procsym, fsys, 6);
		    }
		    else{
			error(5);
		    }
		}
		test(statbegsys|ident, declbegsys, 7);
    }while(sym & declbegsys);
    code[table[tx0].addr].a = cx;
    table[tx0].addr = cx;		// start addr of code
    cx0 = cx; 
    gen(Int, 0, dx);
    statement(fsys|semicolon|endsym);
    gen(opr, 0, 0); // return
    test(fsys, 0, 8);
    //listcode(cx0);
}
long base(long b, long l){
    long b1;

    b1 = b;
    while (l > 0){	// find base l levels down
		b1 = s[b1]; 
		l = l - 1;
    }
    return b1;
}

void interpret(){
    long b,t,p;		// program-, base-, topstack-registers
    instruction i;	// instruction register

    printf("start PL/0\n");
    t = 0; b = 1;p = 0;
    s[1] = 0; s[2] = 0; s[3] = 0;
    do{

	i = code[p];
	p ++;

	switch(i.f){
	    case lit:
			t++;
			s[t] = i.a;
			break;
	    case opr:
			switch(i.a){ 	// operator
		    	case 0:	// return
					t = b - 1; p = s[t+3]; b = s[t+2];
					break;
			    case 1:
					s[t] = -s[t];
					break;
			    case 2:
					t = t - 1; s[t] = s[t] + s[t+1];
					break;
			    case 3:
					t = t - 1; s[t] = s[t] - s[t+1];
					break;
			    case 4:
					t = t - 1; s[t] = s[t] * s[t+1];
					break;
			    case 5:
					t = t - 1; 
					if (s[t+1] == 0)
					{
						fprintf(stderr, "Runtime error: divided by zero.\n");
					}
					s[t] = s[t] / s[t+1];
					break;
			    case 6:
					s[t] = s[t] % 2;
				 	break;
			    case 8:
				 	t = t - 1; s[t] = (s[t] == s[t+1]);
					break;
			    case 9:
					t = t - 1; s[t] = (s[t] != s[t+1]);
					break;
			    case 10:
					t = t - 1; s[t] = (s[t] < s[t+1]);
					break;
			    case 11:
					t = t - 1; s[t] = (s[t] >= s[t+1]);
					break;
			    case 12:
				 	t = t - 1; s[t] = (s[t] > s[t+1]);
					break;
			    case 13:
					t = t - 1; s[t] = (s[t] <= s[t+1]);
					break;
				case 14: //not
					s[t] = !s[t];
					break;
				case 15: //and
					t = t - 1; s[t] = s[t] & s[t+1];
					break;
				case 16: //or
					t = t - 1; s[t] = s[t] | s[t+1];
					break;
			}
			break;
	    case lod:
			t = t + 1; s[t] = s[base(b,i.l) + i.a];
			break;
	    case sto:
			s[base(b,i.l) + i.a] = s[t]; printf("%10ld\n", s[t]); t = t - 1;
			break;
	    case cal:		// generate new block mark
			s[t+1] = base(b,i.l); s[t+2] = b; s[t+3] = p;
			b = t + 1; p = i.a;
			break;
	    case Int:
			t=t+i.a;
			break;
	    case jmp:
			p=i.a;
			break;
	    case jpc:
			if(s[t] == 0){
				    p = i.a;
				}
			t = t-1;
			break;
		case tra:
			s[t] = i.a;
			t ++;
			break;
		case rda:
			s[t] = s[base(b, i.l) + i.a + s[t-1]]; t++;
			break;
		case wta:
			s[base(b, i.l) + i.a + s[t-2]] = s[t-1]; printf("%10ld\n", s[t]); t--; 
			break;
		case jug:
			t --;
			if (s[t-1] < 0 || s[t-1] >= s[t])
			fprintf(stderr, "Runtime error: array index overflow.\n");
			break;
		default:
			//printf("%u\n", i.f);
			fprintf(stderr, "Runtime error: unexpected instruction.\n");
			}
    }while(p != 0);
    printf("end PL/0\n");
}

void init_ssym(){
	for(int i = 0; i < 256; i++){
		ssym[i] = nul;
    }

    ssym['+'] = plus;
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
    ssym['('] = lparen;
    ssym[')'] = rparen;
    ssym['='] = eql;
    ssym[','] = comma;
    ssym['.'] = period;
    ssym[';'] = semicolon;
    ssym['['] = lsparen;
    ssym[']'] = rsparen;
}

int main(){
	init_ssym();
    printf("please input source program file name: ");
    scanf("%s",infilename);
    printf("\n");
    if((infile = fopen(infilename,"r")) == NULL){
		printf("File %s can't be opened.\n", infilename);
		exit(1);
    }
    midfile = fopen("out.txt", "w");

    err = 0;
    cc = 0; cx = 0; ll = 0; ch=' '; kk = al; 
    getsym();
    lev = 0; tx = 0;
    block(declbegsys|statbegsys|period);
    if(sym != period)
		error(9);
    if(err == 0) 	
    	{
    		printf("compile completed.\n");
    		listcode(0);
    		//interpret();
    	}
		
    else
		printf("There are %d errors in PL/0 program\n", err);
	fclose(midfile);
    fclose(infile);
    return 0;
}
