#include "PE.h"

void PutD(unsigned char *Buffer, unsigned long val)
{
   *(Buffer+0)=(unsigned char)(val>>0);
   *(Buffer+1)=(unsigned char)(val>>8);
   *(Buffer+2)=(unsigned char)(val>>16);
   *(Buffer+3)=(unsigned char)(val>>24);
}


/* based on Luevelsmeyer's 'dump_import_directory()' */
long GetFunc(char *DllName, char *FuncName, void *section_base, DWORD section_rva, IMAGE_IMPORT_DESCRIPTOR *imp)
{
    #define adr(rva) ((const void*)((char*)section_base+((DWORD)(rva))-section_rva))
    while(!IsBadReadPtr(imp, sizeof(*imp)) && imp->Name)
      {
          const IMAGE_THUNK_DATA *import_entry;
          if(strcmp((char*)adr(imp->Name), DllName)==0)
          {
            long thunk=(long)imp->FirstThunk;
            import_entry = (IMAGE_THUNK_DATA*)(adr(imp->OriginalFirstThunk));
            while(import_entry->u1.Ordinal)
            {
               const IMAGE_IMPORT_BY_NAME *name_import =(IMAGE_IMPORT_BY_NAME*)( adr(import_entry->u1.AddressOfData));
               if(strcmp((char*)name_import->Name, FuncName)==0)
                  return thunk;
               import_entry++;
               thunk+=sizeof(IMAGE_THUNK_DATA);
            }
          }
          imp++;
      }
    #undef adr
    return 0;
}

unsigned long FileSize(FILE *File)
{
	unsigned long size, old;
   old=ftell(File);
	fseek(File, 0, SEEK_END);
	size=ftell(File);
	fseek(File, old, SEEK_SET);
	return size;
}
