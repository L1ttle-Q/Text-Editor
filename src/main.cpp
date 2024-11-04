#include <cstdio>
#include <algorithm>
#include <cstring>
#include <iostream>
#include "globals.h"
#include "text-editor.h"
#include "SearchAlgorithm.h"

bool Term = false,
     EmptyBuf = true,
     NoEcho = false,
     NoOutputFile = true,
     ShowLineNumber = true,
     ShowCurrent = true;

char input_file[MaxFileNameLength],
     output_file[MaxFileNameLength];

const char HelpMessage[] = \
"\
Guidance:\n\
h/H: help               i/I: go up                j/J: go left\n\
k/K: go down            l/L: go right             o: insert character(s)\n\
O: insert line          q/Q: print full text      t/T: print current line\n\
d: delete character(s)  D: delete line            v/V: (dis)able echo\n\
f: find pattern string   fp: replace all pattern string as new ones\n\
b/B: (dis)able line number display\n\
w/W: write to the output filee/E: exit\n\
tips:\n\
1. for some instructions you can input a number first as repeat count like:\n\
\"5 i\" \"3 j\" \"10l\"\n\
2. for those instructions with string input, input content with a new line:\n\
\"\n\
o\n\
abc\n\
\"\n\
";

void PrintHelp();
void Init();
void Process();
void Output();
void getOutputFile();
bool getY_N();

TextEditor *Text;

void PrintHelp()
{
    fprintf(stdout, "%s", HelpMessage);
}

void getOutputFile()
{
    bool Err = false;
    NoOutputFile = true;

    fprintf(stderr, "Output to file? ");
    if (!getY_N()) return ;

    do
    {
        Err = false;
        fprintf(stdout, "Input file name(output file):");
        scanf("%s", output_file);
        FILE* fp = fopen(output_file, "w");
        NoOutputFile = false;
        if (!fp)
        {
            Err = true;
            fprintf(stderr, "File to write this file!\n");
            fprintf(stderr, "Continue with no output file?");
            if (getY_N()) {Err = false; NoOutputFile = true;}
        }
        else fclose(fp);
    }while(Err);
}

bool getY_N()
{
    fprintf(stderr, "([Y]/[N]):");
    char c; scanf(" %c", &c);
    while (c != 'Y' && c != 'y' && c != 'N' && c != 'n')
    {
        fprintf(stderr, "([Y]/[N]):");
        scanf(" %c", &c);
    }
    return c == 'Y' || c == 'y';
}

void Init()
{
    fprintf(stderr, "Fill buffer with input file? ");
    if (!getY_N())
    {
        EmptyBuf = true; Text = new TextEditor();
        return;
    }

    Istream in;
    bool Err = false;
    EmptyBuf = true;
    do
    {
        Err = false;
        fprintf(stdout, "Input file name(input file):");
        scanf("%s", input_file);
        if (!in.DirectFile(input_file))
        {
            Err = true;
            fprintf(stderr, "Fail to read this file!\n");
            fprintf(stderr, "Continue with empty buffer?");
            if (getY_N()) {Err = false; EmptyBuf = true;}
        }
        EmptyBuf = false;
    }while(Err);

    if (!EmptyBuf)
    {
        Text = new TextEditor(in);
        fprintf(stdout, "Succeed to read!\n%s:\n", input_file);
    }
    else Text = new TextEditor();
}

int op_cnt;

void Process()
{
    char op = '\0';
    fprintf(stdout, "\nInput instruction(h for help, e for exit):\n");
    scanf(" %c", &op);
    switch (op)
    {
        case 'h': // help
        case 'H':
            PrintHelp();
            break;

        case 'i': // go up
        case 'I':
            while (Text->MoveRow(0) && (--op_cnt > 0));
            Text->PrintCurrent(stdout);
            break;

        case 'j': // go left
        case 'J':
            while (Text->MoveCol(0) && (--op_cnt > 0));
            Text->PrintCurrent(stdout);
            break;

        case 'k': // go down
        case 'K':
            while (Text->MoveRow(1) && (--op_cnt > 0));
            Text->PrintCurrent(stdout);
            break;

        case 'l': // go right
        case 'L':
            while (Text->MoveCol(1) && (--op_cnt > 0));
            Text->PrintCurrent(stdout);
            break;

        case 'o': // insert character(s)
            Text->AddChar(getString());
            Text->PrintCurrent(stdout);
            break;

        case 'O': // insert line
            while (Text->NewRow() && (--op_cnt > 0));
            Text->PrintCurrent(stdout);
            break;

        case 'q': // print full text
        case 'Q':
            Text->PrintWholeText(stdout); fprintf(stdout, "\n");
            break;

        case 't': // print line
        case 'T':
            Text->PrintCurrent(stdout);
            break;

        case 'd': // delete character(s)
            Text->DeleteChar(op_cnt);
            Text->PrintCurrent(stdout);
            break;

        case 'D': // delete line
            Text->DeleteRow(op_cnt);
            Text->PrintCurrent(stdout);
            break;

        case 'w':
        case 'W': // write to file
            if (NoOutputFile)
                getOutputFile();
            Output();
            break;

        case 'e':
        case 'E':
            Term = true;
            if (getchar() == '!') exit(0);
            break;

        case 'f': // find pattern string
            op_cnt = (getchar() == 'p');
            fprintf(stdout, "Input pattern string:\n");
            Text->Find<KMP>(getString(), op_cnt);
            break;

        case 'b': // (dis)able line number display
        case 'B':
            ShowLineNumber ^= 1;
            fprintf(stdout, "Line number display has been %sabled!\n",
                    !ShowLineNumber ? "dis" : "");
            break;
        
        case 'v':
        case 'V':
            NoEcho ^= 1;
            fflush(stdout);
            if (NoEcho) freopen(NULL_DEVICE, "w", stdout);
            else freopen(DEVICE_OUT, "w", stdout);
            fprintf(stdout, "Output echo has been %sabled!\n",
                    NoEcho ? "dis" : "");
            break;

        default:
            if (digit(op))
            {
                op_cnt = 0;
                while (digit(op))
                    op_cnt = (op_cnt << 3) + (op_cnt << 1) + op - '0',
                    op = getchar();
                ungetc(op, stdin);
                return ;
            }
            else fprintf(stderr, "Wrong instruction, input again!\n");
    }
    op_cnt = 0;
    Pair<int> p(Text->GetCurrentPos());
}

void Output()
{
    if (NoOutputFile) return ;
    FILE* fp = fopen(output_file, "w");
    Text->PrintWholeText(fp, 0, 0);
    fclose(fp);
}

int main()
{
    fprintf(stdout, "\nWelcome to high efficient text editor!(h for guidance)\n");
    Term = false;
    Init();
    Text->PrintWholeText(stdout, ShowLineNumber); fprintf(stdout, "\n");
    if (Term) return 0;
    while (!Term) Process();
    if (NoOutputFile) getOutputFile();
    if (!NoOutputFile) Output();
    return 0;
}