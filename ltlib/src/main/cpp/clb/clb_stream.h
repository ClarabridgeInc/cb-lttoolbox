#pragma once

#include "ltt.pb.h"

#include "simple_cvt.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>

using namespace ltt_service;

class clb_stream_stdio
{
private:
    FILE *pF;
public:
    clb_stream_stdio(const char *file_name, const char *mode)
    {
        pF = fopen(file_name, mode);
    }

    ~clb_stream_stdio()
    {
        fclose(pF);
    }

    bool
    eof()
    {
        return feof(pF);
    }

    wchar_t
    getWC()
    {
        wchar_t result = static_cast<wchar_t>(fgetwc_unlocked(pF));
        //std::wcout << L": " << result << std::endl;
        return result;
    }
};

class clb_stream_wstring
{
private:
    std::wstringstream s; 
public:
    clb_stream_wstring(const wchar_t *str)
    :
        s(str)
    {
    }

    clb_stream_wstring(const std::wstring &str)
    :
        s(str)
    {
    }

    bool
    eof()
    {
        return s.eof();
    }

    wchar_t
    getWC()
    {
        wchar_t result;
        // >>-operator can't be used - it eats spaces
        s.get(result); //std::wcout << L": " << result << std::endl;
        return result;
    }
};

class clb_stream_wif
{
private:
    wchar_t *pData;
    clb_stream_wstring *pS;
public:
    clb_stream_wif(const char *file_name)
    :
        pData(NULL),
        pS(NULL)
    {
        std::wifstream s(file_name, std::wifstream::binary);

        // std::locale loc("en_US.UTF8");
        std::locale loc(std::locale(std::locale::classic(), "", std::locale::ctype)); 
        s.imbue(loc);

        s.seekg(0, s.end);
        std::wifstream::pos_type file_size = s.tellg();
        s.seekg(0, s.beg);

        pData = new wchar_t[static_cast<std::size_t>(file_size) + 1];
        s.get(pData, file_size);
        pData[file_size] = 0;

        // (*pContents) << s.rdbuf();
        pS = new clb_stream_wstring(pData);
    }

    ~clb_stream_wif()
    {
        delete pS;
        delete pData;
    }

    clb_stream_wstring &
    getStrStream()
    {
        return *pS;
    }
};

typedef clb_stream_wstring &clb_stream_t;

class clb_empty_writer
{
public:
    void done() {}
    void ch(wchar_t ch) {}
    void unknownWord(std::wstring const &sf) {}
    void word(std::wstring const &sf) {}
    void beginLF(double w) {}
    void chLF(wchar_t ch) {}
    void tagLF(std::wstring tag, int tag_id) {}
    void endLF() {}
};

class clb_writer_wcout
{
private:
    int pos;
public:
    clb_writer_wcout()
    :
        pos(0)
    {
    }

    void
    done()
    {
        std::wcout << L"done" << std::endl;
    }

    void
    ch(wchar_t ch)
    {
        std::wcout << L"pos: " << pos << L" ch: \'" << ch << L"\'" << std::endl;
        pos += 1;
    }

    // Prints an unknown word
    // @param sf surface form of the word
    void
    unknownWord(std::wstring const &sf)
    {
        // sf - escaped originally
        std::wcout << L"unkword pos: " << pos << L" sf: \'" << sf << L"\'" << std::endl;
        pos += sf.size();
    }

    // Prints a word
    // @param sf surface form of the word
    // @param lf lexical form of the word
    void
    word(std::wstring const &sf)
    {
        // sf - escaped originally
        std::wcout << L"word pos: " << pos << L" sf: \'" << sf << L"\'" << std::endl;
        pos += sf.size();
    }

    void
    beginLF(double w)
    {
        std::wcout << L"/";
    }

    void
    chLF(wchar_t ch)
    {
        std::wcout << ch;
    }

    void
    tagLF(std::wstring tag, int tag_id)
    {
        std::wcout << tag;
    }

    void
    endLF()
    {
        std::wcout << L' ';
    }
};

class clb_writer_proto
{
private:
    LTTResponse lttResponse;
    Token *pToken;
    LexForm *pLexForm;

    std::wstring curLF;
    std::wstring curChars;

    int pos;

    std::string serialized_data;

    void flushChars()
    {
        if (pToken != NULL)
        {
            std::string sf("");
            if (!simple_cvt::utf16_to_utf8(curChars, sf))
            {
                throw std::runtime_error("clb_writer_proto::flushChars: utf16_to_utf8: invalid utf-16 input found");
            }
            pToken->set_surface_form(sf);
#ifdef PRINT_WRITER_PROTO
            if (sf.size() == 0)
            {
                std::wcout << "!save empty token!" << std::endl;
            }
#endif
            curChars.clear();
            pToken = NULL;
        }
    }

public:
    clb_writer_proto()
    :
        pToken(NULL),
        pLexForm(NULL),
        curLF(L""),
        curChars(L""),
        pos(0),
        serialized_data("")
    {
    }

    bool isLFOpen()
    {
        return pLexForm != NULL;
    }

    void
    done()
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"done" << std::endl;
#endif
        if (pToken != NULL && curChars.size() > 0)
        {
            flushChars();
        }

        serialized_data.clear();
        lttResponse.SerializeToString(&serialized_data);
        lttResponse.Clear();

        pToken = NULL;
        pLexForm = NULL;
        curLF.clear();
        curChars.clear();
        pos = 0;
    }

    void
    ch(wchar_t ch)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"pos: " << pos << L" ch: \'" << ch << L"\'" << std::endl;
#endif
        if (pToken == NULL)
        {
            pToken = lttResponse.add_tokens();
            pToken->set_start(pos);
        }

        curChars.append(1, ch);

        pos += 1;
    }

    // Prints an unknown word
    // @param sf surface form of the word
    void
    unknownWord(std::wstring const &sf)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"unkword pos: " << pos << L" sf: \'" << sf << L"\'" << std::endl;
#endif
        flushChars();

        pToken = lttResponse.add_tokens();
        pToken->set_token_type(TokenType::UnkWord);

        std::string converted_sf("");
        if (!simple_cvt::utf16_to_utf8(sf, converted_sf))
        {
            throw std::runtime_error("clb_writer_proto::unknownWord: utf16_to_utf8: invalid utf-16 input found");
        }

        pToken->set_surface_form(converted_sf);
        pToken->set_start(pos);

        pos += sf.size();

        pToken = NULL;
    }

    // Prints a word
    // @param sf surface form of the word
    // @param lf lexical form of the word
    void
    word(std::wstring const &sf)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"word pos: " << pos << L" sf: \'" << sf << L"\'" << std::endl;
#endif
        pToken->set_token_type(TokenType::Word);

        std::string converted_sf("");
        if (!simple_cvt::utf16_to_utf8(sf, converted_sf))
        {
            throw std::runtime_error("clb_writer_proto::word: utf16_to_utf8: invalid utf-16 input found");
        }

        pToken->set_surface_form(converted_sf);
        pToken->set_start(pos);

        pos += sf.size();

        pToken = NULL;
        pLexForm = NULL;
        curLF.clear();
    }

    void
    beginLF(double w)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"beginLF" << std::endl;
#endif
        if (pToken != NULL && curChars.size() > 0)
        {
            flushChars();
        }

        if (pToken == NULL)
        {
            pToken = lttResponse.add_tokens();
        }
        pLexForm = pToken->add_lex_forms();
        curLF.clear();
    }

    void
    chLF(wchar_t ch)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"chLF: " << ch << std::endl;
#endif
        curLF.append(1, ch);
    }

    void
    tagLF(std::wstring tag, int tag_id)
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"tagLF: " << tag << std::endl;
#endif
        pLexForm->add_tag_ids(tag_id);
    }

    void
    endLF()
    {
#ifdef PRINT_WRITER_PROTO
        std::wcout << L"endLF" << std::endl;
#endif
        std::string converted_lf("");
        if (!simple_cvt::utf16_to_utf8(curLF, converted_lf))
        {
            throw std::runtime_error("clb_writer_proto::endLF: utf16_to_utf8: invalid utf-16 input found");
        }

        pLexForm->set_lf(converted_lf);
    }

    std::string &
    get_serialized()
    {
        return serialized_data;
    }
};

// typedef clb_empty_writer &clb_writer_t;
// typedef clb_writer_wcout &clb_writer_t;
typedef clb_writer_proto &clb_writer_t;
