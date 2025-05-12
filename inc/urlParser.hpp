#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

std::map<std::string, std::string>  parseHttpRequest(const std::string& request);