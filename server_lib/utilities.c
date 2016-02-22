#include "utilities.h"

int get_Config(char * conf){
  char line[70];
  sprintf(line, "grep '%s:' ../conf.txt | cut -d' ' -f2",conf);
  return system(line);
}
