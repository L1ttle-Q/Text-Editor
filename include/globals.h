#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define digit(x) (x >= '0' && x <= '9')

// const int MaxRow = 2000;
// const int MaxCol = 2000;
const int MaxFileNameLength = 50;

template<typename T>
struct Pair
{
    T fir, sec;
    Pair(){}
    Pair(const T& a, const T& b): fir(a), sec(b){}
};

class Istream
{
private:
    static const int MaxBuf = 1000;
    char buf[MaxBuf];
    char* p;
    char* end;
    FILE* fp;
public:
    Istream(): p(buf), end(buf), fp(nullptr){};
    Istream(const char* file_name): p(buf), end(buf), fp(nullptr)
    {DirectFile(file_name);}
    ~Istream(){fclose(fp);}

    bool DirectFile(const char* file_name)
    {
        if (fp && fp != stdin) fclose(fp);
        fp = fopen(file_name, "r");
        return fp != NULL;
    }
    char NextChar()
    {
        if (p == end) end = (p = buf) + fread(buf, sizeof(char), MaxBuf, fp);
        return p == end ? EOF : *p++;
    }
};

static std::string getString(bool enableEmpty = false)
{
    std::string s;
    do{ std::getline(std::cin, s); } while(s.empty() && !enableEmpty);
    return s;
}

extern const int MaxFileNameLength;

extern bool Term, EmptyBuf, NoOutputFile, ShowLineNumber, ShowCurrent;

extern char input_file[],
            output_file[];

#endif