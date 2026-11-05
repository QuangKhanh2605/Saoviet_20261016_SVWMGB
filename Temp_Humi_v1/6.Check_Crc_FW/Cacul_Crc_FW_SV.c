#include <conio.h>
#include <stdio.h>


#define INPUT_FILE  	"SVWM_GSM_MNS_V1_0_0.bin"  //  "SVWM_GSM_LCS_BOOT_V4.bin"  //  "SVWM_GSM_LCS_V1_2_2.bin"
#define HEADER_STR 		",0x08004000@"     //    ",0x08000000@"   //  ",0x08004000@"
#define HEADER_LEN 		31



unsigned int 	crc, crc_boot = 0;
unsigned char 	Temp = 0;
unsigned char	Crc_u8, Crc_u8_boot = 0;
FILE 			*fp, *fp_out;
unsigned int 	Total, total_boot = 0;


int main ()
{
	fp = fopen(INPUT_FILE,"rb");   
	
	if(fp == NULL)
	{
		printf("Error: Could not open file.");
		return 0;
	}

	// Tro toi header cua file
    char headerStr[256] = {0};
    strcpy(headerStr, INPUT_FILE);

    // Tim phan mo rong.bin
    char *ext = strrchr(headerStr, '.');
    if (ext != NULL) {
        *ext = '\0'; // cat bo phan .bin
    }

    strcat(headerStr, HEADER_STR);
    
    printf("HEADER_STR: %s\n", headerStr);
    
	// kiem tra co header chua
    unsigned char checkHeader[HEADER_LEN];
    size_t readBytes = fread(checkHeader, 1, HEADER_LEN, fp);
    
    if (readBytes == HEADER_LEN && memcmp(checkHeader, HEADER_STR, HEADER_LEN) == 0) {
        fseek(fp, HEADER_LEN + 1, SEEK_SET);
        printf("File da co header, cap nhat lai...\n");
    } else {
        printf("File chua co header, them moi...\n");
        fseek(fp, 0, SEEK_SET); 
    }

	while (!feof(fp))
	{
		if(fread(&Temp, 1, 1, fp) < 1)
			break;
		crc += Temp;
		Crc_u8 = (unsigned char) crc;
		Total++;
	}
	
	
	printf("Tong phan tu: %d \r\n", Total);
	printf("Gia tri crc: %d \r\n",crc);
	printf("Gia tri crc_u8 : %d", Crc_u8);
	
	char outputFile[256] = {0};
    strcpy(outputFile, INPUT_FILE);

    // Tìm v? trí ".bin"
    char *find = strrchr(outputFile, '.');
    if (find != NULL) {
        *find = '\0'; // cat bo phan.bin
    }
    strcat(outputFile, "_crc.bin"); //them hau to
	
	// Neu file cu da ton tai thi xoa di
    if (remove(outputFile) == 0) {
        printf("\r\nXoa file output cu: %s\n", outputFile);
    }

	printf("\r\nTao file output: %s\n", outputFile);
	// Tao file output
    fp_out = fopen(outputFile, "wb");
    if (fp_out == NULL) {
        printf("Error: Could not create output file.\n");
        fclose(fp);
        return 0;
    }

    // Ghi header
    fwrite(headerStr, 1, HEADER_LEN, fp_out);

    // Ghi CRC (byte thu 32)
    fwrite(&Crc_u8, 1, 1, fp_out);

    // Ghi lai file goc
    fseek(fp, 0, SEEK_SET);
    while (fread(&Temp, 1, 1, fp) == 1) {
        fwrite(&Temp, 1, 1, fp_out);
    }

    fclose(fp);
    fclose(fp_out);

    printf("\r\n HOAN THANH!  \n");
	
	return 1;
}
