/* Folkos DLL Injector for POLHook, (C) Folko 2002-2003 */

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/POLHookSolution/Injector/Injector.c,v $.
    
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Injector.c,v 1.2 2003/06/21 13:38:43 folko23 Exp $
    </GENERATED>
*/


/*
   Note that this Program has bad coding style since it was copy'n'pasted together from various
   other projects I coded a while back.
   Maybe I'll recode it one day, but it is fully working right now.
*/

#include "PE.h"

int main(int argc, char **argv)
{
   FILE *Target=0, *Patched=0;
	unsigned long pointer=0;
	unsigned long imagesize, ImageBase, import_rva, iat_rva, import_length;
	unsigned char *FileBuffer, *newsect, *sectiondata[255];
   void *import_address, *iat_address;
   long OEP, NEP, lola, getpa;
   long imports=0, iat=0;
   int mysect, mysectsize, i;
	PIMAGE_DOS_HEADER doshdr;
	PIMAGE_FILE_HEADER filehdr;
	PIMAGE_OPTIONAL_HEADER opthdr;
	PIMAGE_SECTION_HEADER secthdrs[255];

   printf("Folko's DLL Injector for POLHook (C) Folko 2002\n");
   if(argc<2)
	{
      printf("Drag POL.exe on me..\n");
      system("pause");
		exit(EXIT_FAILURE);
	}

	Target=fopen(argv[1], "rb");
	if(!Target){ printf("Couldn't open target!\n");	exit(EXIT_FAILURE); }

	imagesize=FileSize(Target);
   FileBuffer=malloc(imagesize);
	fread(FileBuffer, sizeof(unsigned char), imagesize, Target);
	fclose(Target);

   /* Read headers */
   if(*(WORD*)FileBuffer!=IMAGE_DOS_SIGNATURE)
   {
      printf("File isn't a valid PE (exe) file!\n");
      free(FileBuffer);
      system("pause");
      exit(EXIT_FAILURE);
   }
   doshdr=(IMAGE_DOS_HEADER*)FileBuffer; pointer=doshdr->e_lfanew;
   if(*(DWORD*)(FileBuffer+pointer)!=IMAGE_NT_SIGNATURE)
   {
      printf("File isn't a valid PE (exe) file!\n");
      free(FileBuffer);
      system("pause");
      exit(EXIT_FAILURE);
   }pointer+=sizeof(IMAGE_NT_SIGNATURE);
   filehdr=(IMAGE_FILE_HEADER*)(FileBuffer+pointer); pointer+=sizeof(IMAGE_FILE_HEADER);
	opthdr=(IMAGE_OPTIONAL_HEADER*)(FileBuffer+pointer); pointer+=sizeof(IMAGE_OPTIONAL_HEADER);
   ImageBase=opthdr->ImageBase;
	
	/* Read section headers and make 'em read-/writeable */
   import_rva=opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
   iat_rva=opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress;
   for(i=0; i<filehdr->NumberOfSections; i++)
	{
		unsigned long sectstart, sectend;
      secthdrs[i]=(IMAGE_SECTION_HEADER*)(FileBuffer+pointer);
		secthdrs[i]->Characteristics|=IMAGE_SCN_MEM_WRITE;
		secthdrs[i]->Characteristics|=IMAGE_SCN_MEM_READ;
		sectstart=secthdrs[i]->VirtualAddress;
		sectend=secthdrs[i]->VirtualAddress+secthdrs[i]->Misc.VirtualSize;
		printf("Section %d ROffset: %.8X RSize: %.8X VOffset: %.8X VSize: %.8X", i, secthdrs[i]->PointerToRawData, secthdrs[i]->SizeOfRawData, secthdrs[i]->VirtualAddress, secthdrs[i]->Misc.VirtualSize);

      if(sectstart <= opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress && sectend >= opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress){
			printf(" *");
      }else if(sectstart <= opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && sectstart<secthdrs[i]->VirtualAddress+secthdrs[i]->SizeOfRawData){
         printf(" *");
      }else if(sectstart <= opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress && sectend >= opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress){
			printf(" #");
      }else if(secthdrs[i]->Characteristics&IMAGE_SCN_MEM_EXECUTE){
			printf(" C");
      }
      if(secthdrs[i]->VirtualAddress<=import_rva && import_rva<secthdrs[i]->VirtualAddress+secthdrs[i]->SizeOfRawData)
         imports=i;
      if(secthdrs[i]->VirtualAddress<=iat_rva && iat_rva<secthdrs[i]->VirtualAddress+secthdrs[i]->SizeOfRawData)
         iat=i;
      printf("\n");
		pointer+=sizeof(IMAGE_SECTION_HEADER);
	}
	printf("C = Code\n");
	printf("* = Imports\n");
	printf("# = Resources\n");
   /* Add a section */
	mysect=filehdr->NumberOfSections;
	mysectsize=opthdr->SectionAlignment;
	opthdr->SizeOfImage+=mysectsize;
	filehdr->NumberOfSections++;
	if(FileBuffer[pointer]!=0 || FileBuffer[pointer+sizeof(IMAGE_SECTION_HEADER)]!=0)
	{
      printf("No free space to add a new section! File appears to be packed.\nContinue anyway and overwrite (Not recommended)?\n");
		if(toupper(getch())=='N')
		{
			free(FileBuffer);
			exit(EXIT_FAILURE);
		}
	}
	secthdrs[mysect]=(IMAGE_SECTION_HEADER*)(FileBuffer+pointer);
	secthdrs[mysect]->Characteristics=IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE;
	strcpy((char*)secthdrs[mysect]->Name, "POLHOOK");
	secthdrs[mysect]->PointerToRawData=0; /* Later */
	secthdrs[mysect]->SizeOfRawData=mysectsize;
	secthdrs[mysect]->Misc.VirtualSize=mysectsize;
   /* Set pointer to beginning of first section */
	while(++pointer%opthdr->FileAlignment);

   /* Read all sections cept our new one since its not in the file yet */
	for(i=0; i<filehdr->NumberOfSections-1; i++)
	{
		sectiondata[i]=(unsigned char*)(FileBuffer+pointer);
		pointer+=secthdrs[i]->SizeOfRawData;
	}
   /* Save OEP */
	OEP=opthdr->AddressOfEntryPoint;

   /* Fill in missing data */
	secthdrs[mysect]->PointerToRawData=pointer;
	secthdrs[mysect]->VirtualAddress=secthdrs[mysect-1]->VirtualAddress+secthdrs[mysect-1]->Misc.VirtualSize;
   
   /* Correct alignment if neccessary */
	if(secthdrs[mysect]->VirtualAddress%opthdr->SectionAlignment!=0)
		while(++secthdrs[mysect]->VirtualAddress%opthdr->SectionAlignment);

   /* Find required functions */
   import_length = opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
   import_address=FileBuffer+secthdrs[imports]->PointerToRawData;
   iat_address=FileBuffer+secthdrs[iat]->PointerToRawData;

   lola=GetFunc("KERNEL32.dll", "LoadLibraryA", import_address, secthdrs[imports]->VirtualAddress,
      (IMAGE_IMPORT_DESCRIPTOR*)((char*)import_address+import_rva-secthdrs[imports]->VirtualAddress)
      );
   getpa=GetFunc("KERNEL32.dll", "GetProcAddress", import_address, secthdrs[imports]->VirtualAddress,
      (IMAGE_IMPORT_DESCRIPTOR*)((char*)import_address+import_rva-secthdrs[imports]->VirtualAddress)
      );
   if(!lola || !getpa)
   {
      printf("Program seems to be crypted / packed; can't find LoadLibraryA and GetProcAddress!\n");
      return -1;
   }
   printf("GetProcAddress@%.4X\n", getpa);
   printf("LoadLibraryA@%.4X\n", lola);

   /* Calculate new entrypoint */
	NEP=opthdr->AddressOfEntryPoint=secthdrs[mysect]->VirtualAddress;

   /* Build section */
	newsect=malloc(mysectsize);
	memset(newsect, 0x90, mysectsize); /* NOP */

   /* Fill new section */
   {
      char *DllName="POLHook\\POLHook.DLL"; /* argv[2]; */
      char *FuncName="Init"; /* argv[3]; */
      int DllLen=(int)strlen(DllName)+1; /* +1 for '\0' */
      int FuncLen=(int)strlen(FuncName)+1;
      unsigned char stub[]=
      {
         /*00*/0x68, 0x00, 0x00, 0x00, 0x00,       /* push dllname */
         /*05*/0xFF, 0x15, 0x00, 0x00, 0x00, 0x00, /* call LoadLibraryA */
         /*11*/0x68, 0x00, 0x00, 0x00, 0x00,       /* push funcname */
         /*16*/0x50,                               /* push eax */
         /*17*/0xFF, 0x15, 0x00, 0x00, 0x00, 0x00, /* call GetProcAddress */
         /*23*/0xFF, 0xD0,                         /* call eax */
         /*25*/0xE9, 0x00, 0x00, 0x00, 0x00        /* jmp OEP */

      };
		printf("Creating stub...\n");
      PutD(stub+1, NEP+ImageBase+sizeof(stub)+1); /* push dllname */
      PutD(stub+7,ImageBase+lola); /* call LoadLibraryA */
      PutD(stub+12,NEP+ImageBase+sizeof(stub)+DllLen+1); /* push funcname */
      PutD(stub+19,ImageBase+getpa);   /* call GetProcAddress */
      PutD(stub+26, CalcDist(NEP+25, OEP)); /* jmp OEP */
      memcpy(newsect, stub, sizeof(stub));
      memcpy(newsect+sizeof(stub)+1, DllName, DllLen);
      memcpy(newsect+sizeof(stub)+DllLen+1, FuncName, FuncLen);
  }
   /* Save */
   {
      char FileName[MAX_PATH];
      char *FilePtr=0;
      GetFullPathName(argv[1], sizeof(FileName), FileName, &FilePtr);

      printf("Saving...\n");
      FileName[((strrchr(FileName, '\\')+1)-FileName)]=0;
      strcat(FileName, "pol_hooked.exe");
      Patched=fopen(FileName, "wb");
	   fwrite(FileBuffer, sizeof(unsigned char), pointer, Patched);
	   fwrite(newsect, sizeof(unsigned char), mysectsize, Patched);
	   fclose(Patched);
      printf("Saved as %s...\n", FileName);
   }

   /* Clean */
	free(newsect);
	free(FileBuffer);
   system("pause");
	return 0;
}
