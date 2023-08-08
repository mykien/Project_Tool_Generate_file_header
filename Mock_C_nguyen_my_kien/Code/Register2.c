#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/* Hàm Nhập đường dẫn cho file Input */
void Reg_GetInputFilePath(char* p_InputFilePath_s)
{
  printf ("Enter input file path :");
  scanf("%s",p_InputFilePath_s);
  getchar();
}   

/* Hàm nhập đường dẫn cho file Output */
void Reg_GetOutputFilePath(char* p_OutputFilePath_s)
{
  printf ("Enter Output file path :");
  scanf("%s",p_OutputFilePath_s);
  getchar();
}

/* Hàm xác định đường dẫn file Input */
void Reg_GetTxtAndCsvFilePath(const char* p_FolderPath_s, char* p_TxtFilePath_s, char* p_CsvFilePath_s, char* p_TxtFileName_s, char* p_CsvFileName_s) {
    DIR* f_Directory_ptr = opendir(p_FolderPath_s);
    if (f_Directory_ptr == NULL) {
        printf("can't open folder!\n");
        return;
    }

    struct dirent* f_Entry_st;
    while ((f_Entry_st = readdir(f_Directory_ptr)) != NULL) {
        if (f_Entry_st->d_type == DT_REG) {
            char f_FilePath_aa[100];
            snprintf(f_FilePath_aa, sizeof(f_FilePath_aa), "%s/%s", p_FolderPath_s, f_Entry_st->d_name);
            if (strstr(f_Entry_st->d_name, ".txt") != NULL) {
                strcpy(p_TxtFilePath_s, f_FilePath_aa);
                strncpy(p_TxtFileName_s, f_Entry_st->d_name, strlen(f_Entry_st->d_name) - 4);
                /* Đảm bảo chuỗi kết thúc bằng null */
                p_TxtFileName_s[strlen(f_Entry_st->d_name) - 4] = '\0';
            } else if (strstr(f_Entry_st->d_name, ".csv") != NULL) {
                strcpy(p_CsvFilePath_s, f_FilePath_aa);
                strncpy(p_CsvFileName_s, f_Entry_st->d_name, strlen(f_Entry_st->d_name) - 4);
                /* Đảm bảo chuỗi kết thúc bằng null */
                p_CsvFileName_s[strlen(f_Entry_st->d_name) - 4] = '\0';
            }
        }
    }
    closedir(f_Directory_ptr);
}

/* Hàm tạo file .h dựa trên đường dẫn Output đã nhập và tên file .csv */
void Reg_GenerateHeaderFile(const char* p_FolderPath_s, const char* p_FileName_s, char* P_HeaderFilePath_s) {
    /* Tạo tên file .h dựa trên tên file được truyền vào */
    snprintf(P_HeaderFilePath_s, 100, "%s/%s.h", p_FolderPath_s, p_FileName_s);

    /* Tạo file .h */
    FILE* f_HeaderFile_ptr = fopen(P_HeaderFilePath_s, "w");
    if (f_HeaderFile_ptr != NULL) {
        printf("create header file path successfully: %s", P_HeaderFilePath_s);
    } else {
        printf("create header file path failed: %s", P_HeaderFilePath_s);
    }
}

/* Hàm thay thế chuỗi "<NOW>" bằng thời gian hiện tại */
void Reg_ReplaceTime(char* p_str_s) {
    /* Trả về thời gian hiên tại */
    time_t f_Now_time = time(NULL);
    struct tm* f_CurrentTime_st = localtime(&f_Now_time);
    char f_CurrentTime_aa[20];
    strftime(f_CurrentTime_aa, sizeof(f_CurrentTime_aa), "%d/%m/%Y", f_CurrentTime_st);

    const char* f_NowMarker_s = "<NOW>";
    unsigned int f_NowMarkerLen_u32 = strlen(f_NowMarker_s);
    unsigned int f_CurrentTimeLen_u32 = strlen(f_CurrentTime_aa);

    char* f_MarkerPtr_ptr = strstr(p_str_s, f_NowMarker_s);
    while (f_MarkerPtr_ptr != NULL) {
        memmove(f_MarkerPtr_ptr + f_CurrentTimeLen_u32, f_MarkerPtr_ptr + f_NowMarkerLen_u32, strlen(f_MarkerPtr_ptr + f_NowMarkerLen_u32) + 1);
        memcpy(f_MarkerPtr_ptr, f_CurrentTime_aa, f_CurrentTimeLen_u32);
        f_MarkerPtr_ptr = strstr(f_MarkerPtr_ptr + f_CurrentTimeLen_u32, f_NowMarker_s);
    }
}

/* Hàm thay thế chuỗi "<NAME_FLIE>" bằng tên file trong một chuỗi */
void Reg_ReplaceFileName(char* p_Str_s, const char* p_FileName_s) {
    const char* f_FileMarker_s = "<NAME_FILE>";
    unsigned int f_FileMarkerLen_u32 = strlen(f_FileMarker_s);
    unsigned int f_FileNameLen_u32 = strlen(p_FileName_s);

    char* f_MarkerPtr_ptr = strstr(p_Str_s, f_FileMarker_s);
    if (f_MarkerPtr_ptr != NULL) {
        unsigned int remainingLen = strlen(f_MarkerPtr_ptr + f_FileMarkerLen_u32);

        memmove(f_MarkerPtr_ptr + f_FileNameLen_u32, f_MarkerPtr_ptr + f_FileMarkerLen_u32, remainingLen + 1);
        memcpy(f_MarkerPtr_ptr, p_FileName_s, f_FileNameLen_u32);
    }
}

/* Hàm trợ giúp để tách chuỗi từ vị trí start cho đến vị trí end */
void Reg_ExtractSubstring(const char* p_Source_s, char* p_Destination_s, int p_start_u32, int p_end_u32) {
    int j = 0;
    for (int i = p_start_u32; i < p_end_u32; i++) {
        p_Destination_s[j++] = p_Source_s[i];
    }
    p_Destination_s[j] = '\0';
}

/* Hàm trợ giúp để tìm vị trí của kí tự delimiter trong chuỗi */
int Reg_FindDelimiter(const char* p_Source_s, char p_delimiter_s, int p_Start_u32) {
    int i = p_Start_u32;
    while (p_Source_s[i] != p_delimiter_s && p_Source_s[i] != '\0' && p_Source_s[i] != '\n') {
        i++;
    }
    return i;
}

/* Hàm đọc file Csv*/
void Reg_ReadCsvFile(FILE* p_CsvFile_file, FILE* p_HeaderFile_file) {
    const unsigned int f_BufferSize_u32 = 1000;
    char f_Buffer_aa[f_BufferSize_u32];
    int f_DelimiterPos_u32;  // Khai báo biến delimiterPos ở đầu hàm

    char f_BaseAddress_aa[12];
    char f_PeripheralName_aa[20];
    char f_OffsetAddress_aa[100];
    char f_RegisterName_aa[100];
    char f_Width_aa[100];
    char f_Access_aa[10];

    int f_Calcwidth_u32;
    char f_DisplayDatatype_aa[100];

    int f_PreOffsetAdress_u32;
    int f_DecOffsetAdress_u32=0;
    int f_ValueDummy_u32 = 0;
    int f_CountDummy_u32=0;
    int f_CheckStatusBaseAddress_u32=0;
    char f_PrePeripheralName_aa[20];
    char f_PreBaseAddress_aa[12];
    
    while (fgets(f_Buffer_aa, f_BufferSize_u32, p_CsvFile_file) != NULL) {
        /* Write #define */
        if (f_Buffer_aa[0] == '0' && f_Buffer_aa[1] == 'x' && f_CheckStatusBaseAddress_u32 == 1 || f_Buffer_aa[0]=='E' && f_Buffer_aa[1] =='O' && f_Buffer_aa[2] == 'F'){
            fprintf(p_HeaderFile_file,"} %s_REG;\n\n",f_PrePeripheralName_aa);
            fprintf(p_HeaderFile_file,"#define %s_BASE_ADDRS (%s)\n",f_PrePeripheralName_aa, f_PreBaseAddress_aa);
            fprintf(p_HeaderFile_file,"#define %s ((%s_REG*)%s_BASE_ADDRS)\n",f_PrePeripheralName_aa,f_PrePeripheralName_aa,f_PrePeripheralName_aa); 
          }

        if (f_Buffer_aa[0] == '0' && f_Buffer_aa[1] == 'x') {
          /* Read base address */
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', 0);
            Reg_ExtractSubstring(f_Buffer_aa, f_BaseAddress_aa, 0, f_DelimiterPos_u32);
            
            /* Read peripheral name */
            int start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_PeripheralName_aa, start, f_DelimiterPos_u32);
            
            /* Read offset address */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_OffsetAddress_aa, start, f_DelimiterPos_u32);

            /* Read register name */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_RegisterName_aa, start, f_DelimiterPos_u32);

            /* Read f_Width_aa */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_Width_aa, start, f_DelimiterPos_u32);

            /* Read f_Access_aa */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, '\0', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_Access_aa, start, f_DelimiterPos_u32);

            /* Lưu pre_offsetAddress */
            f_PreOffsetAdress_u32 = strtol (f_OffsetAddress_aa, NULL ,16);
            
            /* bắt đầu khai báo */
            fprintf (p_HeaderFile_file,"\n/* %s */\n",f_PeripheralName_aa);
            fprintf (p_HeaderFile_file,"typedef struct {\n");          
        }
        else if (f_Buffer_aa[0] == ',' && f_Buffer_aa[1] == ','){
          /* Process special case */
            int start = 2;

            /* Read offset address */
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);          
            Reg_ExtractSubstring(f_Buffer_aa, f_OffsetAddress_aa, start, f_DelimiterPos_u32);

            /* Read register name */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_RegisterName_aa, start, f_DelimiterPos_u32);

            /* Read f_Width_aa */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, ',', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_Width_aa, start, f_DelimiterPos_u32);

            /* Read f_Access_aa */
            start = f_DelimiterPos_u32 + 1;
            f_DelimiterPos_u32 = Reg_FindDelimiter(f_Buffer_aa, '\0', start);
            Reg_ExtractSubstring(f_Buffer_aa, f_Access_aa, start, f_DelimiterPos_u32);
        }

        /* Xử lý độ rộng của thanh ghi - data type */
        if (strcmp(f_Width_aa, "32") == 0){
            f_Calcwidth_u32 = 4;
            strcpy(f_DisplayDatatype_aa,"unsigned int");
        }

        /* Xử lý dummy */
        if (f_Buffer_aa[0] == ',' && f_Buffer_aa[1] == ','){
        f_DecOffsetAdress_u32 = strtol (f_OffsetAddress_aa, NULL ,16);
        f_ValueDummy_u32 = f_DecOffsetAdress_u32 - f_PreOffsetAdress_u32-f_Calcwidth_u32;
          if (f_ValueDummy_u32 > 0){
            f_CountDummy_u32++;
            fprintf (p_HeaderFile_file,"  char dummy%d[%d];\n",f_CountDummy_u32, f_ValueDummy_u32);
          }
        f_PreOffsetAdress_u32 = f_DecOffsetAdress_u32; 
        }

        /* khai báo thanh ghi */
        if (f_Buffer_aa[0] == '0' && f_Buffer_aa[1] == 'x' && f_CheckStatusBaseAddress_u32 == 0 || f_Buffer_aa[0] == ',' && f_Buffer_aa[1] == ','){
          if (strcmp(f_Access_aa, "RO") == 0){
            fprintf(p_HeaderFile_file,"  volatile const %s %s;\n",f_DisplayDatatype_aa, f_RegisterName_aa);
          }
          else if(strcmp(f_Access_aa,"RW") == 0) {
            fprintf(p_HeaderFile_file,"  volatile %s %s;\n",f_DisplayDatatype_aa, f_RegisterName_aa);
          }
        }
        
        /* Xử lý khai báo define */
        if (f_Buffer_aa[0] == '0' && f_Buffer_aa[1] == 'x' && f_CheckStatusBaseAddress_u32 == 0)
        {
          strcpy(f_PrePeripheralName_aa, f_PeripheralName_aa);
          strcpy(f_PreBaseAddress_aa,f_BaseAddress_aa);
          f_CheckStatusBaseAddress_u32 = 1;
        }
        else if (f_Buffer_aa[0] == '0' && f_Buffer_aa[1] == 'x' && f_CheckStatusBaseAddress_u32 == 1)
        {
          strcpy(f_PrePeripheralName_aa, f_PeripheralName_aa);
          strcpy(f_PreBaseAddress_aa,f_BaseAddress_aa);
        } 

    }
}

/* Hàm sao chép nội dung từ file .txt vào file .h */
void Reg_CopyTxtFileToHeaderFile(const char* p_TxtFilePath_s, const char* p_HeaderFilePath_s, const char* p_FileName_s, const char* p_CsvFilePath_s) {
    FILE* f_TxtFile_ptr = fopen(p_TxtFilePath_s, "r");
    FILE* f_HeaderFile_ptr = fopen(p_HeaderFilePath_s, "w");
    FILE* f_CsvFile_ptr = fopen(p_CsvFilePath_s, "r");

    if (f_TxtFile_ptr == NULL || f_HeaderFile_ptr == NULL || f_CsvFile_ptr == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    const unsigned int f_BufferSize_u32 = 1000;
    char f_Buffer_aa[f_BufferSize_u32];
    int f_CheckContent_u32 = 0;

    while (fgets(f_Buffer_aa, f_BufferSize_u32, f_TxtFile_ptr) != NULL) {
        if (strstr(f_Buffer_aa, "/*start_define_register*/") != NULL) {
            f_CheckContent_u32 = 1;
            /* Gọi hàm để đọc nội dung từ CSV và ghi vào file header */
            Reg_ReadCsvFile(f_CsvFile_ptr, f_HeaderFile_ptr); 
            /* Bỏ qua nội dung và không ghi vào file header */
            continue; 
        }
        else if (strstr(f_Buffer_aa, "/*end_define_register*/") != NULL) {
            f_CheckContent_u32 = 0;
            
            continue; 
        }

        if (!f_CheckContent_u32) {
            Reg_ReplaceTime(f_Buffer_aa);
            Reg_ReplaceFileName(f_Buffer_aa, p_FileName_s);
            fputs(f_Buffer_aa, f_HeaderFile_ptr);
        }
    }

    fclose(f_TxtFile_ptr);
    fclose(f_HeaderFile_ptr);
    fclose(f_CsvFile_ptr);

    printf("\ncreate header file successfully\n");
}

/* Hàm Main*/
int main()
{
unsigned int f_choiceMenu_u32;
unsigned int f_choiceMenu1_u32;

char f_InputFilePath_s[100];
char f_outFilePath_s[100];

char f_TxtFilePath_s[100];
char f_CsvFilePath_s[100];
char f_TxtFileName_s[100];
char f_CsvFileName_s[100];
char f_HeaderFilepath_s[100];

  do
  {
    printf ("\n1. Choose location option\n");
    printf ("2. Generate file Register_S32K144.h\n");
    printf ("3. Exits\n");
    printf ("Entry your choice :");
    scanf ("%d",&f_choiceMenu_u32);
    getchar();

      switch (f_choiceMenu_u32)
      {
        case 1:
          do 
          {
            printf ("\n1.1 Set input file location\n");
            printf ("1.2 Set output file location\n");
            printf ("1.3 Back\n");
            printf ("Entry your choice :");
            scanf ("%d",&f_choiceMenu1_u32);
            getchar();

            switch (f_choiceMenu1_u32)
            {
              case 1:
                Reg_GetInputFilePath(f_InputFilePath_s);
                //printf("%s\n",f_InputFilePath_s);
                Reg_GetTxtAndCsvFilePath(f_InputFilePath_s, f_TxtFilePath_s, f_CsvFilePath_s, f_TxtFileName_s, f_CsvFileName_s);
                break;
              case 2:
                Reg_GetOutputFilePath(f_outFilePath_s);
                //printf("%s",f_outFilePath_s);
                break;
              case 3:
                printf ("Back");
                break;
              default :
              printf ("Your choice must be 1->3. Please entry again");
            }

          }while (f_choiceMenu1_u32 != 3);

          break;
        /* Generate header file */  
        case 2:
          Reg_GenerateHeaderFile(f_outFilePath_s,f_CsvFileName_s,f_HeaderFilepath_s);
          Reg_CopyTxtFileToHeaderFile(f_TxtFilePath_s,f_HeaderFilepath_s,f_CsvFileName_s,f_CsvFilePath_s);
          
          break;
        case 3:
          printf ("Exits program");
          break;     
        default:
          printf ("Your choice must be 1->3. Please entry again");
      }
  }while(f_choiceMenu_u32 != 3);

return 0;
    
}