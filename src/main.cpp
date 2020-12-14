#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include "../../bsb_lan/bsb-version.h"

using namespace std;

#define DEFAULT_FLAG 0

#define PROGMEM

#define PROGMEM_LATE

// #define LANG DE
//
// #define ONLYTEXT
#ifndef LANGS
#define LANGS "DE"
#endif

#include "../../bsb_lan/BSB_lan_defs.h"

std::string string_to_hex(const std::string &input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

bool replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

template <typename I>
std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1)
{
    static const char *digits = "0123456789ABCDEF";
    std::string rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(w >> j) & 0x0f];
    return rc;
}

std::map<int, std::string> readENUM(std::string enumstr)
{
    std::map<int, std::string> result;

    int pos = 0;
    int enumcount = 0;

    while (pos < enumstr.length() - 2 && enumcount < 1000)
    {
        int space = enumstr.find(' ', pos + 1);
        int end = enumstr.find('\0', space);

        std::string keys = enumstr.substr(pos, space - pos);
        std::string desc = enumstr.substr(space + 1, end - space - 1);
        int key = 0;

        char charBuf[1024];
        strcpy(charBuf, keys.c_str());

        std::memcpy(&key, charBuf, keys.length());
        result.insert(std::make_pair(key, desc));
        pos = end + 1;

        enumcount++;
    }

    return result;
}

#define LANGSTR str(LANG);

int main()
{
    cout << "{" << endl;
#ifndef ONLYTEXT
    cout << "  \"Version\": \"" << MAJOR << "." << MINOR << "." << PATCH << "\"," << endl;
    cout << "  \"Compiletime\": \"" << COMPILETIME << "\"," << endl;
#endif

    cout << "  \"commands\": {" << endl;
    cmd_t *cmdtbl;

    int size1 = sizeof(cmdtbl1) / sizeof(cmdtbl1[0]);
    int size2 = sizeof(cmdtbl2) / sizeof(cmdtbl1[0]);
    int size3 = sizeof(cmdtbl3) / sizeof(cmdtbl1[0]);

    cmdtbl = new cmd_t[size1 + size2 + size3];

    std::copy(cmdtbl1, cmdtbl1 + size1, cmdtbl);
    std::copy(cmdtbl2, cmdtbl2 + size2, cmdtbl + size1);
    //  std::copy(cmdtbl3, cmdtbl3 + size3, cmdtbl + size2);

    std::string enumcat = std::string(ENUM_CAT, sizeof(ENUM_CAT));
    std::map<int, std::string> categories = readENUM(enumcat);

    int allSize = size1 + size2;

    //  allSize = 50;

    for (int i = 0; i < allSize; i++)
    {
        cmd_t data = cmdtbl[i];

        cout << "" << endl;
        cout << "  \"" << std::to_string(i) << "\": {" << endl;
#ifndef ONLYTEXT
        cout << "  \"parameter\" : " << std::to_string(data.line) << "," << endl;
        cout << "  \"command\" : \"0x" << n2hexstr(data.cmd) << "\"," << endl;
#endif
        cout << "  \"category\" : {" << endl;
        cout << "    \"name\": {" << endl;
        cout << "      \"" << LANGS "\": \"" << categories[data.category] << "\"" << endl;
        cout << "    }";
#ifndef ONLYTEXT
        cout << "," << endl;
        cout << "    \"min\" : " << std::to_string(ENUM_CAT_NR[data.category * 2]) << "," << endl;
        cout << "    \"max\" : " << std::to_string(ENUM_CAT_NR[data.category * 2 + 1]) << "" << endl;
#else
        cout << endl;
#endif
        cout << "  }," << endl;

        cout << "  \"type\" : {" << endl;
        std::string unit = optbl[data.type].unit;
        replace(unit, "&#037;", "%");
        replace(unit, "&#181;", "µ");
        replace(unit, "&deg;", "°");
        cout << "    \"unit\" : {" << endl;
         cout << "      \"" << LANGS "\": \"" << unit << "\"" << endl;
         cout << "    }";
#ifndef ONLYTEXT        
        cout << "," << endl;
        cout << "    \"name\" : \"" << optbl[data.type].type_text << "\"," << endl;
        cout << "    \"datatype\" : \"" << dt_types_text[optbl[data.type].data_type].type_text << "\"," << endl;
        cout << "    \"factor\" : " << optbl[data.type].operand << "," << endl;
        cout << "    \"payload_length\" : " << std::to_string(optbl[data.type].payload_length) << "," << endl;
        cout << "    \"precision\" : " << std::to_string(optbl[data.type].precision) << "," << endl;
        cout << "    \"enable_byte\" : " << to_string((optbl[data.type].enable_byte)) << "" << endl;
#else
        cout << endl;
#endif
        cout << "  }," << endl;
        cout << "  \"description\": {" << endl;
        cout << "      \"" << LANGS "\": \"" << data.desc << "\"" << endl;
        cout << "    }," << endl;

        if (data.enumstr_len > 0)
        {
            cout << "  \"enum\" : {" << endl;

            std::string enumstr = string(data.enumstr, data.enumstr_len);
            std::map<int, std::string> enumData = readENUM(enumstr);

            for (std::map<int, std::string>::iterator it = enumData.begin(); it != enumData.end(); ++it)
            {
                if (it != enumData.begin())
                    cout << "," << endl;

                cout << "    \"0x" << n2hexstr(it->first, 4) << "\" : {" << endl;
                cout << "      \"" << LANGS "\": \"" << it->second << "\"" << endl;
                cout << "    }";
            }
            cout << endl;

            cout << "    }," << endl;
        }
#ifndef ONLYTEXT
        cout << "  \"flags\" : [" << endl;
        if ((data.flags & FL_RONLY) == FL_RONLY)
            cout << "    \"READONLY\"," << endl;
        if ((data.flags & FL_NO_CMD) == FL_NO_CMD)
            cout << "    \"NO_CMD\"," << endl;
        if ((data.flags & FL_OEM) == FL_OEM)
            cout << "    \"OEM\"," << endl;
        if ((data.flags & FL_SPECIAL_INF) == FL_SPECIAL_INF)
            cout << "    \"SPECIAL_INF\"," << endl;
        if ((data.flags & FL_SW_CTL_RONLY) == FL_SW_CTL_RONLY)
            cout << "    \"SW_CTL_READONLY\"," << endl;
        cout << "    \"\"" << endl;
        cout << "  ]," << endl;
        cout << "  \"device\" : [{" << endl;
        cout << "    \"family\": " << std::to_string(data.dev_fam) << "," << endl;
        cout << "    \"var\" : " << std::to_string(data.dev_var) << "" << endl;
        cout << "  }]" << endl;
#else
        cout << "    \"dummy\" : 0" << endl;
#endif
        cout << "  }," << endl;
    }

    cout << "  \"END\" : {} }" << endl;
    cout << "}" << endl;
}