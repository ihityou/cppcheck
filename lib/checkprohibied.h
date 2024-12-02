/* -*- C++ -*-
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
#ifndef checkprohibiedH
#define checkprohibiedH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"
#include "tokenize.h"
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <iostream>

class ErrorLogger;
class Function;
class Settings;
class Tokenizer;

/** @brief Check for functions never called */
/// @{

class CPPCHECKLIB CheckProhibied : public Check
{
public:
    CheckProhibied() : Check(myName()) {}

    std::string analyzerInfo() const;

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const override
    {
        // prohibitedWholeWordsError(errorLogger, emptyString, 0, 0, "funcName");
    }

    // Return true if an error is reported.
    bool check(const Settings &settings, ErrorLogger &errorLogger) const;
    std::string classInfo() const override
    {
        return "Detect disabled functions:\n";
    }

private:
    void checkWholeWords(const std::vector<std::string> &words, const Token *tok);
    void checkContainWords(const std::vector<std::string> &words, const Token *tok);
    /** @brief This constructor is used when running checks. */
    CheckProhibied(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {}
    void prohibitedWholeWordsError(const Token *tok, const std::string &word);
    void prohibitedContainWordsError(const Token *tok, const std::string &word);
    void prohibitCheck(void);
    void runChecks(const Tokenizer &tokenizer, ErrorLogger *errorLogger) override
    {
        CheckProhibied checkProhibied(&tokenizer, &tokenizer.getSettings(), errorLogger); //
        checkProhibied.prohibitCheck();
    }
    static std::string myName()
    {
        return "prohibied";
    }
    struct CPPCHECKLIB FunctionUsage
    {
        std::string filename;
        unsigned int lineNumber{};
        unsigned int fileIndex{};
    };

    std::unordered_map<std::string, FunctionUsage> mFunctions;

    std::unordered_map<std::string, bool>
        prohibitions;
    class CPPCHECKLIB FunctionDecl
    {
    public:
        explicit FunctionDecl(const Function *f);
        std::string functionName;
        unsigned int lineNumber;
    };
};

/// @}
//---------------------------------------------------------------------------
#endif // checkunusedfunctionsH
