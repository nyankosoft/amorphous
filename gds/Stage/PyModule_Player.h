#ifndef __PYMODULE_PLAYER_H__
#define __PYMODULE_PLAYER_H__


#include <string>
using namespace std;

#include <Python.h>


extern PyMethodDef g_PyModulePlayerMethod[];

class CPlayerInfo;
extern CPlayerInfo PlayerEntity;

#endif  /*  __PYMODULE_PLAYER_H__  */