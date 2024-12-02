/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2024 Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//---------------------------------------------------------------------------
#include "checkprohibied.h"
#include "astutils.h"
#include "errortypes.h"
#include "mathlib.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>
#include "xml.h"
#include <string>
namespace
{
    CheckProhibied instance;
}
//---------------------------------------------------------------------------
using namespace std;
static const CWE CWE800(800U); // prohibited function call

/*void CheckProhibied::checkWholeWords( const std::vector<std::string> &words,const Token *tok)
{
    // 遍历每个类别中的路径
    for (std::size_t i = 0; i < words.size(); i++)
    {
        const std::string &word = words[i];
        // 检查路径中是否包含要匹配的字符串
        if (strcmp(word.c_str(), tok->str().c_str()) == 0)
        {
            prohibitedWholeWordsError(tok);
        }
    }
}
void CheckProhibied::checkContainWords( const std::vector<std::string> &contains,const Token *tok)
{
    // 禁止字符串包含某些字符
    for (std::size_t i = 0; i < contains.size(); i++)
    {
        const std::string &contain = contains[i];
        // 检查路径中是否包含要匹配的字符串
        if (strstr(tok->str().c_str(), contain.c_str()) != NULL)
        {
            prohibitedWholeWordsError(tok);
        }
    }
}*/
void CheckProhibied::checkWholeWords(const std::vector<std::string> &words, const Token *tok)
{
    const std::string &tokenStr = tok->str();
    for (const std::string &word : words)
    {
        if (word == tokenStr)
        {
            prohibitedWholeWordsError(tok, word);
        }
    }
}
void CheckProhibied::checkContainWords(const std::vector<std::string> &contains, const Token *tok)
{
    // Check if the token string contains any of the prohibited substrings
    const std::string &tokenStr = tok->str();
    for (const std::string &contain : contains)
    {
        if (tokenStr.find(contain) != std::string::npos)
        {
            prohibitedContainWordsError(tok, contain);
        }
    }
}
void CheckProhibied::prohibitCheck()
{
    if (!mSettings->severity.isEnabled(Severity::warning))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope *scope : symbolDatabase->functionScopes)
    {
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next())
        {
            // 遍历 mProhibitedList
            for (const auto &prohibited_pair : mSettings->mProhibited.mProhibitedList)
            {
                const std::vector<std::string> &paths = prohibited_pair.filenames;
                const std::vector<std::string> &words = prohibited_pair.wholeWords;
                const std::vector<std::string> &contains = prohibited_pair.containWords;

                for (std::size_t i = 0; i < paths.size(); i++)
                {
                    const std::string &path = paths[i];
                    // 判断路径是否匹配
                    if (strcmp(path.c_str(), Path::toNativeSeparators(mTokenizer->list.getOrigFile(tok)).c_str()) == 0)
                    {
                        // eString eName eFunction
                        if (tok->tokType() == Token::eName || tok->tokType() == Token::eFunction)
                        {
                            checkWholeWords(words, tok);
                        }
                        else if (tok->tokType() == Token::eString)
                        {
                            checkContainWords(contains, tok);
                        }
                        else
                        {
                            checkWholeWords(words, tok);
                        }
                    }
                }
            }
        }
    }

    return;
}

void CheckProhibied::prohibitedWholeWordsError(const Token *tok, const std::string &word)
{
    std::string messageInfo = "The " + word + " is prohibited.";
    reportError(tok, Severity::warning, "CheckProhibied",
                messageInfo, CWE800, Certainty::normal);
}
void CheckProhibied::prohibitedContainWordsError(const Token *tok, const std::string &word)
{
    std::string messageInfo = "The " + word + " is prohibited from being included.";
    reportError(tok, Severity::warning, "CheckProhibied",
                messageInfo, CWE800, Certainty::normal);
}
CheckProhibied::FunctionDecl::FunctionDecl(const Function *f)
    : functionName(f->name()), lineNumber(f->token->linenr())
{
}

std::string CheckProhibied::analyzerInfo() const
{
    std::ostringstream ret;
    return ret.str();
}

namespace
{
    struct Location
    {
        Location() : lineNumber(0) {}
        Location(std::string f, const int l) : fileName(std::move(f)), lineNumber(l) {}
        std::string fileName;
        int lineNumber;
    };
}
