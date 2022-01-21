#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {

    // Получаем имя файла
    char* nfile;
    if (argc > 1) {
        nfile = argv[1];
        //printf("argv1 - %s\n", nfile);
    } else {
        //При отсутствии выходим с сообщением
        printf("Please, enter a filename  - %s filename.jpg\n", argv[0]);
        return 0;
    }

    // Открываем файл и получаем указатель
    FILE *fp;
    long position = 0; // позиция для считывания
    int i = 0;              // количество zip файлов
    int ch = 0;        // символ

    if((fp=fopen(nfile, "rb"))==NULL) {
        //При невозможности открыть выходим с сообщением
        printf ("Cannot open file: %s\n", nfile);
        return 0;
    }

    int numChars = 4;                           // количество символов для проверки
    int8_t template[] = {0x50, 0x4b, 0x03, 0x04};  // сигнатура
    int8_t buffer[numChars];                       // буфер для проверки
    int length;                                 // длина имени zip файла

    while(!ferror(fp) && ch != EOF) {
        ch = fgetc (fp);
        if (ch == template[0]) {                // Если совпал первый символ
            buffer[0] = (int8_t)ch;                     // записываем его в буфер
            for (int n = 1; n < numChars; n++)  // и считываем остальные
            {
                ch = fgetc (fp);
                buffer[n]= (int8_t)ch;
            }
            if (memcmp(template, buffer, numChars) == 0) {    // При совпадении буфера и сигнатуры
                printf("zip file is detected - ");              // выводим сообщение
                i++;                                            // увеличиваем кол-во zip файлов
                position += 26;                                 // переходим на позицию длины имени файла
                fseek( fp, position, SEEK_SET);
                ch = fgetc (fp);                                // считываем длину
                length= ch;
                position += 4;                                  // переходи на позицию имени файла
                fseek( fp, position, SEEK_SET);
                for (int m = 0; m < length; m++)                // считываем имя zip файла
                {
                    ch = fgetc (fp);
                    printf ("%c", ch);
                }
                printf("\n");
                position += length;                             // переходим на позицию после имени
            }
        }
        position++;
        fseek( fp, position, SEEK_SET);
    }

    if (i == 0)
        printf("zip isn't detected \n");

    fclose(fp);

    return 0;
}
