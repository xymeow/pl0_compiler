# pl0_compiler

编译原理大作业。。。

##  实现功能：

* 注释在/\* 和 \*/中
* 布尔类型
* 一维数组类型
* repeat until语句
* else 子句
* for语句
* 分离解释器与编译器
* 增加错误处理
* ++、--、+=、-=运算

##  新增指令
- tra：将数组下标的范围入栈
- jug：判断数组下标范围是否合法
- wta：写数组数据
- rda：读数组数据
- opr 0 14: not指令，对bool型取反
- opr 0 15: and指令
- opr 0 16: or指令


##  错误代号
1.  "Found ':=' when expexting '='.",
2. "There must be a number to follow '='.",
3. "There must be an '=' to follow the identifier.",
4. "There must be an identifier to follow 'const', 'var', or 'procedure'.", 
5. "Missing ',' or ';'.",
6. "Incorrect procedure name.",
7. "Statement expected.",
8.  "Follow the statement is an incorrect symbol.",
9.  "'.' expected.",
10.  "';' expected.",
11.  "Undeclared identifier.",
12.  "Illegal assignment.",
13.  "':=' expected.",
14.  "There must be an identifier to follow the 'call'.",
15.  "A constant or variable can not be called.",
16.  "'then' expected.",
17.  "';' or 'end' expected.",
18.  "'do' expected.",
19.  "Incorrect symbol.",
20.  "Relative operators expected.",
21.  "Procedure identifier can not be in an expression.",
22.  "Missing ')'.",
23.  "The symbol can not be followed by a factor.",
24.  "The symbol can not be as the beginning of an expression.",
25.  "The number is too great.",
26.  "Redeclear identifier",
27.  "The array index should be constant.",
28.  "Missing ']'.",
29.  "The array index should be > 0 .",
30.  "Array index Illegal.",
31.  "There are too many levels."

##  保留字
    "and       ",
    "begin     ",
    "bool      ",
    "call      ",
    "const     ",
    "do        ",
    "downto    ",
    "else      ",
    "end       ",
    "false     ",
    "for       ",
    "if        ",
    "not       ",
    "odd       ",
    "or        ",
    "procedure ",
    "repeat    ",
    "then      ",
    "to        ",
    "true      ",
    "until     ",
    "var       ",
    "while     "
    
##  使用&测试
分别用gcc编译编译器pl0.c和解释器interpreter.c
使用测试代码example.pl0:

    const a = 0;
    var x,y,b[5];
    bool i, j;

    begin
	  i := true;
	  j := i or false and false;
	  x := y;
	  y := 3;
      y := (x--);
      b[3] := 1;
      b[1] := x;
      b[2] := x;
      /*call aaa;*/
      /*edrmalfuei*/
    end.
    
运行编译器生成中间代码,默认生成的中间代码文件名为“out.txt”：


中间代码：

```sh
         0  jmp  0    1
         1  int  0    7
         2  lit  0    1
         3  sto  0    5
         4  lod  0    5
         5  lit  0    0
         6  opr  0   16
         7  lit  0    0
         8  opr  0   15
         9  sto  0    6
        10  lod  0    4
        11  sto  0    3
        12  lit  0    3
        13  sto  0    4
        14  lod  0    3
        15  lit  0    1
        16  opr  0    3
        17  sto  0    3
        18  lod  0    3
        19  lit  0    1
        20  opr  0    2
        21  sto  0    4
        22  lit  0    1
        23  wta  0    5
        24  lod  0    3
        25  wta  0    5
        26  lod  0    3
        27  wta  0    5
        28  opr  0    0
```


#＃  存在的问题
使用pl0.pdf中提供的测试程序时，解释器会多输出一段内容,编译器没问题

解释器输出：发现后面多了一段内容



