#include <stdlib.h>
#include <string.h>
#include "pl0.h"

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
    long p,b,t;		// program-, base-, topstack-registers
    instruction i;	// instruction register

    printf("start PL/0\n");
    t = 0; b = 1; p = 0;
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
		default:
			printf("%u\n", i.f);
			fprintf(stderr, "Runtime error: unexpected instruction.\n");
			}
    }while(p != 0);
    printf("end PL/0\n");
}

enum fct str2fct(char* temp, int i){
	if(strcmp(temp, mnemonic[0]) == 0)
	 	 return lit;
	else if(strcmp(temp, mnemonic[1]) == 0)
		 return opr;
	else if(strcmp(temp, mnemonic[2]) == 0)
		 return lod;
	else if(strcmp(temp, mnemonic[3]) == 0)
		 return sto;
	else if(strcmp(temp, mnemonic[4]) == 0)
		 return cal;
	else if(strcmp(temp, mnemonic[5]) == 0)
		 return Int;
	else if(strcmp(temp, mnemonic[6]) == 0)
		 return jmp;
	else if(strcmp(temp, mnemonic[7]) == 0)
		 return jpc;
}

int main(int argc, char const *argv[])
{
	midfile = fopen("out.txt", "r");
	int i, j;
	i = 0;
	char temp[4];
	while(fscanf(midfile, "%d%s%ld%ld\n", &j, temp, &code[i].l, &code[i].a) == 4){
		code[i].f = str2fct(temp, i);
		printf("%d %u %ld %ld\n", i, code[i].f, code[i].l, code[i].a);
		i++;
	}
	interpret();
	return 0;
}