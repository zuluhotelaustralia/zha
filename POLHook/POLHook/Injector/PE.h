#ifndef PE_H_INCLUDED
#define PE_H_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define CalcDist(from, to) ((to)-(from)-5)
void PutD(unsigned char *Buffer, unsigned long val);
long GetFunc(char *DllName, char *FuncName, void *section_base, DWORD section_rva, IMAGE_IMPORT_DESCRIPTOR *imp);
unsigned long FileSize(FILE *File);

#endif /* PE_H_INCLUDED */
