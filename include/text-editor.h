#ifndef _TEXT_EDITOR_H_
#define _TEXT_EDITOR_H_

#include "globals.h"
#include "linkedlist.h"

class TextEditor : public LinkedList<LinkedList<char> >
{
private:
    std::string getLineText(const LinkedList<char>* ptr) const
    {
        std::string res;
        for (auto it = ptr->begin()++; it != ptr->end(); it++)
            res += *it;
        return res;
    }

    void PrintLine(FILE* out, const int& cur_col, const LinkedList<char>* ptr,
                   bool withColnum, bool withCur, const std::vector<int>& multicur, bool withNewline = true) const
    {
        char colnum[10] = "";
        if (withColnum)
        {
            snprintf(colnum, sizeof(colnum), "%d |", cur_col);
            fprintf(out, "%s", colnum);
        }
        std::string textContent = getLineText(ptr);
        for (auto c: textContent)
            fprintf(out, "%c", c);

        if (withCur || withNewline) fprintf(out, "\n");
        if (!withCur) return ;
        for (int i = 1; i <= withColnum * strlen(colnum); i++) fprintf(out, "~");
        for (int i = 0; i < multicur.size(); i++)
        {
            int j = !i ? 0 : multicur[i - 1];
            for (; j < multicur[i] - 1; j++) fprintf(out, "~");
            fprintf(out, "^");
        }
        if (withNewline) fprintf(out, "\n");
    }
    void PrintLine(FILE* out, int cur_col, const LinkedList<char>* ptr,
                   bool withColnum, bool withCur, bool withNewline = true) const
    {
        std::vector<int> multicur; multicur.emplace_back(ptr->cur);
        PrintLine(out, cur_col, ptr, withColnum, withCur, multicur, withNewline);
    }
public:
    TextEditor() {AddElem();}
    TextEditor(Istream &input): TextEditor()
    {
        char c = input.NextChar();
        while (c != EOF)
        {
            if (c != '\n') now->data.AddElem(c);
            else AddElem();
            c = input.NextChar();
        }
    }

    bool NewRow(char *s)
    {
        for (int i = 0; i < strlen(s); i++)
            now->data.AddElem(s[i]);
        return true;
    }
    bool NewRow()
    {
        AddElem();
        return true;
    }
    bool AddChar(const char *s, bool prev = true)
    {
        bool fir = prev;
        for (int i = 0; i < strlen(s); i++)
        {
            if (fir) now->data.AddElemPrev(s[0]), fir = false;
            else now->data.AddElem(s[i]);
        }
        return true;
    }
    bool AddChar(const std::string& s, bool prev = true)
    {return AddChar(s.c_str(), prev);}
    bool DeleteRow(int op = 1)
    {
        return (DeleteElem(op) && (!isEmpty() || AddElem()));
    }
    bool DeleteChar(int op = 1)
    {
        return now->data.DeleteElem(op);
    }
    bool MoveRow(bool op)
    {
        int l_cur = now->data.getCur();
        if (!Move(op)) return false;
        while (now->data.getCur() > l_cur && MoveCol(0));
        while (now->data.getCur() < l_cur && MoveCol(1));
        return true;
    }
    bool MoveCol(bool op)
    {
        return now->data.Move(op);
    }
    Pair<int> GetCurrentPos() const
    {
        return Pair<int>(cur, now->data.getCur());
    }

    void PrintCurrent(FILE* out, bool withColnum = ShowLineNumber, bool withCur = ShowCurrent) const
    {
        if (now->last != head) PrintLine(out, cur - 1, &(now->last->data), withColnum, 0);
        PrintLine(out, cur, &(now->data), withColnum, withCur);
        if (now->next != tail) PrintLine(out, cur + 1, &(now->next->data), withColnum, 0);
    }
    void PrintWholeText(FILE* out, bool withColnum = ShowLineNumber, bool withCur = ShowCurrent) const
    {
        int i = 1;
        bool fir = true;
        for (auto it = begin()++; it != end(); it++)
        {
            if (fir) fir = false;
            else fprintf(out, "\n");
            PrintLine(out, i, &*it, withColnum, withCur && i == cur, 0);
            i++;
        }
    }

    template<class U>
    void Find(const char *s, bool replace = false)
    {
        std::string new_str;
        if (replace)
        {
            fprintf(stdout, "input replace string:\n");
            new_str = getString(true);
        }
        fprintf(stdout, "\n");
        U search(s);
        int i = 1, s_len = strlen(s);
        bool noOutput = true;
        auto save_now = now;
        for (now = head->next; now != tail; now = now->next, i++)
        {
            std::vector<int> V (search.find(getLineText(&(now->data))));
            if (V.empty()) continue;
            if (replace) fprintf(stdout, "origin:\n");
            PrintLine(stdout, i, &(now->data), ShowLineNumber, 1, V);
            if (replace)
            {
                fprintf(stdout, "replace:\n");
                for (int j = 0; j < V.size(); j++)
                {
                    int dst = V[j] + j * (new_str.size() - s_len);
                    now->data.MoveTo(dst);
                    for (int k = 1; k <= s_len; k++) DeleteChar();
                    AddChar(new_str, now->data.now->next != now->data.tail);
                }
                PrintLine(stdout, i, &(now->data), ShowLineNumber, ShowCurrent);
            }
            noOutput = false;
        }
        now = save_now;
        if (noOutput) fprintf(stdout, "No substring found!");
        fprintf(stdout, "\n");
    }

    template<class U>
    void Find(const std::string& s, bool replace = false)
    {return Find<U>(s.c_str(), replace);}

    int getRow() const {return length;}
    int getCol() const {return now->data.getLength();}
};

#endif