#pragma once
#include <string>

const std::string STATIC_BES_STRUCT_CODE = R"(
function __BES_Struct(_name, _uid) constructor {
    self.__bes_name = _name;
    self.__bes_uid = _uid;
}

)";