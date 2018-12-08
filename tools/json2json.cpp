#include <unistd.h>

#include "simdjson/jsonparser.h"
#include "simdjson/jsonioutil.h"

using namespace std;

void compute_dump(ParsedJson::iterator & pjh) {
    bool inobject = (pjh.get_type() == '{');
    bool inarray = (pjh.get_type() == '{');
printf("got this: \n");
pjh.print(std::cout); 
printf(" \n");
    if((!inobject) && (!inarray)) {
      pjh.print(std::cout);// just print the lone value 
      pjh.up() && pjh.next();
      return; // we are done
    }
    printf("going to object/array\n");

    // we have either an array or an object
    pjh.down();
    if(inobject) {
          std::cout <<"{";
          pjh.print(std::cout); // must be a string
          std::cout <<":";
          pjh.next();
          compute_dump(pjh);// let us recurse
          while (pjh.next()) {
              std::cout <<",";
              pjh.print(std::cout);
              std::cout <<":";
              pjh.next();
              compute_dump(pjh);// let us recurse
          }
          std::cout <<"}";
    } else {
          std::cout <<"[";
          compute_dump(pjh);// let us recurse
          while(pjh.next()) { 
              std::cout <<",";
              compute_dump(pjh);// let us recurse
          }
          std::cout <<"]";
    }
    pjh.up() && pjh.next();
}
 

int main(int argc, char *argv[]) {
  int c;
  bool rawdump = false;
  bool apidump = false;

  while ((c = getopt (argc, argv, "da")) != -1)
    switch (c)
      {
      case 'd':
        rawdump = true;
        break;
      case 'a':
        apidump = true;
        break;
      default:
        abort ();
      }
  if (optind >= argc) {
    cerr << "Reads json in, out the result of the parsing. " << endl;  
    cerr << "Usage: " << argv[0] << " <jsonfile>" << endl;
    exit(1);
  }
  const char * filename = argv[optind];
  if(optind + 1 < argc) {
    cerr << "warning: ignoring everything after " << argv[optind  + 1] << endl;
  }
  std::string_view p;
  try {
    p = get_corpus(filename);
  } catch (const std::exception& e) { // caught by reference to base
    std::cout << "Could not load the file " << filename << std::endl;
    return EXIT_FAILURE;
  }
  ParsedJson pj;
  bool allocok = pj.allocateCapacity(p.size(), 1024);
  if(!allocok) {
    std::cerr << "failed to allocate memory" << std::endl;
    return EXIT_FAILURE;
  }
  bool is_ok = json_parse(p, pj); // do the parsing, return false on error
  if (!is_ok) {
    std::cerr << " Parsing failed. " << std::endl;
    return EXIT_FAILURE;
  }
  if(apidump) {
      ParsedJson::iterator pjh(pj);
      if(!pjh.isOk()) {
        std::cerr << " Could not iterate parsed result. " << std::endl;
        return EXIT_FAILURE;      
      }
      compute_dump(pjh);
  } else {
    is_ok = rawdump ? pj.dump_raw_tape() : pj.printjson();
    if(!is_ok) {
      std::cerr << " Could not print out parsed result. " << std::endl;
      return EXIT_FAILURE;      
    }
  }
  return EXIT_SUCCESS;
}