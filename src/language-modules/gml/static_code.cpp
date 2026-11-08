#pragma once
#include <string>

const std::string BES_INTERNAL_STRUCT_CODE = R"(
///@ignore Internal BES base struct
function __BES_Struct(_name, _uid) constructor {
    self.__bes_name = _name;
    self.__bes_uid = _uid;
}
)";

const std::string TYPE_CHECKERS_CODE = R"(
///@desc This function returns wether a given variable is a uint8 (true or false) or not.
///@param {Any} n The argument to check. 
function is_uint8(n){
	return is_numeric(n) && round(n) && n >= 0 && n <= 0xff;
}

///@desc This function returns wether a given variable is a uint16 (true or false) or not.
///@param {Any} n The argument to check. 
function is_uint16(n){
	return is_numeric(n) && round(n) == n && n >= 0 && n <= 0xffff;
}

///@desc This function returns wether a given variable is a uint32 (true or false) or not.
///@param {Any} n The argument to check. 
function is_uint32(n){
	return is_numeric(n) && round(n) == n && n >= 0 && n <= 0xffffffff;
}

///@desc This function returns wether a given variable is a uint64 (true or false) or not.
///@param {Any} n The argument to check. 
function is_uint64(n){
	return is_numeric(n) && round(n) == n && n >= 0 && n <= 0xffffffffffffffff;
}


///@desc This function returns wether a given variable is a int8 (true or false) or not.
///@param {Any} n The argument to check. 
function is_int8(n){
	return is_numeric(n) && round(n) == n && n >= -0x80 && n < 0x80;
}

///@desc This function returns wether a given variable is a int16 (true or false) or not.
///@param {Any} n The argument to check. 
function is_int16(n){
	return is_numeric(n) && round(n) == n && n >= -0x8000 && n < 0x8000;
}

///@desc This function returns wether a given variable is a int32 (true or false) or not.
///@param {Any} n The argument to check. 
function is_int32(n){
	return is_numeric(n) && round(n) == n && n >= -0x80000000 && n < 0x80000000;
}

///@desc This function returns wether a given variable is a float16 (true or false) or not.
///@param {Any} n The argument to check. 
function is_float16(n){
	return is_numeric(n) && n >= -0xFFE0 && n <= 0xFFE0;
}

///@desc This function returns wether a given variable is a float32 (true or false) or not.
///@param {Any} n The argument to check. 
function is_float32(n){
	return is_numeric(n) && abs(n) < 3.40282346638528859811704183484516925440 * power(10, 38);
}

///@desc This function returns wether a given variable is a float64 (true or false) or not.
///@param {Any} n The argument to check. 
function is_float64(n){
	return is_numeric(n) && abs(n) < 1.797693134862315708145274237317043567981 * power(10, 308);
}

///@desc This function returns wether a given variable is a BES struct (true or false) or not.
///@param {Any} n The argument to check.
///@param {Enum.BES_UID} struct_uid The enum UID value of the struct type to check for, or undefined for any. 
function is_bes_struct(n, struct_uid = undefined){
	if (!is_struct(n)) return false;
	if (struct_uid == undefined) return true;
	return (n.__bes_uid == struct_uid);
}
)";
