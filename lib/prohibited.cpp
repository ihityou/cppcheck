#include "prohibited.h"
#include "picojson.h"
#include "path.h"
#include <algorithm>
#include <cctype> // std::isdigit, std::isalnum, etc
#include <cstring>
#include <functional> // std::bind, std::placeholders
#include <sstream>
#include <utility>
#include <iostream>
#include <fstream>

std::string
prohibited::parseJsonFile(const char *filename)
{

    std::ifstream ifs(filename);
    std::string json_content{std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};

    picojson::value v;
    std::string err = picojson::parse(v, json_content);

    if (!err.empty())
    {
        return "JSON parse error: ";
    }
    if (!v.is<picojson::object>())
    {
        return "JSON is not an object";
    }
    picojson::object &obj = v.get<picojson::object>();
    picojson::array files_list;
    // 假设我们知道JSON对象有一个名为"name"的字符串键
    auto name_it = obj.find("files");
    if (name_it->second.is<picojson::array>())
    {
        files_list = name_it->second.get<picojson::array>();
        for (auto &files : files_list)
        {
            ProhibitedStr mProhibited_unit;
            if (files.is<picojson::object>())
            {

                auto &file_list = files.get("file");
                auto &prohibited_list = files.get("prohibited");
                auto &prohibit_contain = files.get("prohibitContain");
                if (prohibited_list.is<picojson::array>())
                {
                    for (auto &prohibitedWord : prohibited_list.get<picojson::array>())
                    {
                        mProhibited_unit.wholeWords.push_back(prohibitedWord.to_str());
                    }
                }
                if (prohibit_contain.is<picojson::array>())
                {
                    for (auto &prohibitedContain : prohibit_contain.get<picojson::array>())
                    {
                        mProhibited_unit.containWords.push_back(prohibitedContain.to_str());
                    }
                }

                if (file_list.is<picojson::array>())
                {
                    for (auto &file : file_list.get<picojson::array>())
                    {
                        if (Path::isAbsolute(Path::toNativeSeparators(file.to_str())))
                        {
                            mProhibited_unit.filenames.push_back(Path::toNativeSeparators(file.to_str()));
                        }
                        else
                        {
                            if (Path::isFile(Path::getAbsoluteFilePath(Path::toNativeSeparators(file.to_str()))))
                            {
                                mProhibited_unit.filenames.push_back(Path::getAbsoluteFilePath(Path::toNativeSeparators(file.to_str())));
                            }
                        }
                    }
                }
            }
            mProhibitedList.push_back(mProhibited_unit);
        }
    }
    else
    {
        std::cerr << "Name is not a string" << std::endl;
    }
    /**/

    return "";
}
