#ifndef OPEN_TEST_H_
#define OPEN_TEST_H_


#include <yaml.h>
#include <stdlib.h>

#include "Rte_Type.h"
#include "tsm_data.h"

void ReadFromYamlAndSetData(const char* filename, SimulinkData* simulink_data);

void SimulinkDataSet(const char* key_str, const char* value_str, SimulinkData* simulink_data);

#endif