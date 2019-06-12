#include <functional>
#include <map>
#include <string>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals;

#include <iostream>
#include "simdjson.cpp"
#include "simdjson.h"

void value2list(ParsedJson::iterator &pjh, py::list &list) {
  switch (pjh.get_type()) {
    case 'l':
      list.append(pjh.get_integer());
      break;
    case 'd':
      list.append(pjh.get_double());
      break;
    case '"':
      list.append(pjh.get_string());
      break;
    case 't':
      list.append(true);
      break;
    case 'f':
      list.append(false);
      break;
    case 'n':
      list.append(py::none());
      break;
  }
}

py::list array2list(ParsedJson::iterator &pjh) {
  py::list list;
  if (pjh.is_array()) {
    pjh.down();
    value2list(pjh, list);
    while (pjh.next()) {
      value2list(pjh, list);
    }
    pjh.up();
  }
  return list;
}

void getkey(ParsedJson::iterator &pjh, const char **key) {
  switch (pjh.get_type()) {
    case '"':
      *key = pjh.get_string();
      break;
    default:
      std::cout << "Not define type: " << pjh.get_type() << std::endl;
      break;
  }
}

void value2dict(ParsedJson::iterator &pjh, py::dict &dict, const char *key) {
  if (pjh.next()) {
    switch (pjh.get_type()) {
      case 'l':
        dict[key] = pjh.get_integer();
        break;
      case 'd':
        dict[key] = pjh.get_double();
        break;
      case '"':
        dict[key] = pjh.get_string();
        break;
      case 't':
        dict[key] = true;
        break;
      case 'f':
        dict[key] = false;
        break;
      case 'n':
        dict[key] = py::none();
        break;
      case '[':
        dict[key] = array2list(pjh);
        break;
    }
  }
}

py::dict pjh2dict(ParsedJson::iterator &pjh) {
  py::dict dict;
  if (pjh.is_object()) {
    const char *key = "";
    pjh.down();
    getkey(pjh, &key);
    value2dict(pjh, dict, key);
    if (pjh.is_object()) {
      dict[key] = pjh2dict(pjh);
    }
    while (pjh.next()) {
      getkey(pjh, &key);
      value2dict(pjh, dict, key);
      if (pjh.is_object()) {
        dict[key] = pjh2dict(pjh);
      }
    };
    pjh.up();
  }
  return dict;
}

py::dict loads(std::string json_string) {
  py::dict d;
  ParsedJson pj = build_parsed_json(json_string);
  if (!pj.isValid()) {
    std::cout << "Error: json string isn't valid." << std::endl;
  }
  ParsedJson::iterator pjh(pj);
  d = pjh2dict(pjh);
  return d;
}

/*
int main() {

  std::string mystring = "{\"A\": 1}";
  // std::string may not overallocate so a copy will be needed
  ParsedJson pj = build_parsed_json(mystring);  // do the parsing
  if (!pj.isValid()) {
    // something went wrong
  }
  ParsedJson::iterator pjh(pj);
  compute_dump(pjh);
}
 */

PYBIND11_MODULE(sjson, m) {
  m.doc() = R"sjson(
        Python sjson binding 
        -----------------------
        .. currentmodule::sjson

    )sjson";

  m.def("loads", &loads, R"sjson(
        
    )sjson");
}