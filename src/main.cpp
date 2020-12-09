#include <iostream>
#include <string>
#include <map>
#include <cstring>
// #include "Objects/box.h"

using namespace std;

#define DEFAULT_FLAG 0

// /usr/bin/clang++ --std=c++11 main.cpp -D'LANG=C' -o main && ./main > output.json
// #define LANG FR

#define PROGMEM

#define PROGMEM_LATE

#define LANG DE
#include "../../bsb_lan/BSB_lan_defs.h"


/* add feature to load all languages in one round */
/* this works only in different cpp file -> compile to different .o files and than include them */
// #define LANG EN
// #define ENUM_CAT ENUM_CAT_EN
// #define cmdtbl1 cmdtbl1_en
// #include "../../bsb_lan/BSB_lan_defs.h"
// #define ENUM_CAT ENUM_CAT
// #define cmdtbl1 cmdtbl1


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

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1)
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

int main()
{
    cout << "[" << endl;

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

        cout << "{" << endl;
        cout << "  \"index\" : " << std::to_string(i) << "," << endl;
        cout << "  \"command\" : \"0x" << n2hexstr(data.cmd) << "\"," << endl;
        cout << "  \"category\" : {" << endl;
        cout << "    \"name\": \"" << categories[data.category] << "\"," << endl;
        cout << "    \"min\" : " << std::to_string(ENUM_CAT_NR[data.category * 2]) << "," << endl;
        cout << "    \"max\" : " << std::to_string(ENUM_CAT_NR[data.category * 2 + 1]) << "" << endl;
        cout << "  }," << endl;

        cout << "  \"type\" : {" << endl;
        cout << "    \"name\" : \"" << optbl[data.type].type_text << "\"," << endl;
        std::string unit = optbl[data.type].unit;
        replace(unit, "&#037;", "%");
        replace(unit, "&#181;", "µ");
        replace(unit, "&deg;", "°");

        cout << "    \"unit\" : \"" << unit << "\"," << endl;
        cout << "    \"datatype\" : \"" << dt_types_text[optbl[data.type].data_type].type_text << "\"," << endl;
        cout << "    \"factor\" : \"" << optbl[data.type].operand << "\"," << endl;
        cout << "    \"precision\" : \"" << std::to_string(optbl[data.type].precision) << "\"," << endl;
        cout << "    \"enable_byte\" : \"" << to_string((optbl[data.type].enable_byte)) << "\"" << endl;
        cout << "  }," << endl;

        cout << "  \"parameter\" : " << std::to_string(data.line) << "," << endl;
        cout << "  \"description\" : \"" << data.desc << "\""  << "," << endl;
      //  cout << "  \"description_en\" : \"" << data_en.desc << "\""
       

        if (data.enumstr_len > 0)
        {
            cout << "  \"enum\" : {" << endl;

            std::string enumstr = string(data.enumstr, data.enumstr_len);
            std::map<int, std::string> enumData = readENUM(enumstr);

            for (std::map<int, std::string>::iterator it = enumData.begin(); it != enumData.end(); ++it)
            {
                if (it != enumData.begin())
                    cout << "," << endl;

                cout << "    \"0x" << n2hexstr(it->first, 4) << "\" :";
                cout << "\"" << it->second << "\"";
            }
            cout << endl;

            cout << "    }," << endl;
        }
        cout << "  \"flag\" : " << std::to_string(data.flags) << "," << endl;
        cout << "  \"device\" : {" << endl;
        cout << "    \"family\": " << std::to_string(data.dev_fam) << "," << endl;
        cout << "    \"var\" : " << std::to_string(data.dev_var) << "" << endl;
        cout << "  }" << endl;
        cout << "}," << endl;
    }

    cout << "{}]" << endl;
}