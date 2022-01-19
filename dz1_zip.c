#include <stdio.h>

// Функция сравнения двух массивов
int compTemplates(const int tmp[], const int buf[], int size) {
    for (int i = 0; i < size; i++) {
        if (tmp[i] != buf[i])   // Проверяем поэлементно
            return 0;           // при первом несовпадении возвращаем 0
    }
    return 1;
}

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
    long long position = 0; // позиция для считывания
    int i = 0;              // количество zip файлов
    int ch;                 // символ
    fpos_t *ppos;           // указатель на позицию
    ppos = &position;
    if((fp=fopen(nfile, "rb"))==NULL) {
        //При невозможности открыть выходим с сообщением
        printf ("Cannot open file: %s\n", nfile);
        return 0;
    }

    int numChars = 4;                           // количество символов для проверки
    int template[] = {0x50, 0x4b, 0x03, 0x04};  // сигнатура
    int buffer[numChars];                       // буфер для проверки
    int length;                                 // длина имени zip файла

    while(!ferror(fp) && ch != EOF) {
        ch = fgetc (fp);
        if (ch == template[0]) {                // Если совпал первый символ
            buffer[0] = ch;                     // записываем его в буфер
            for (int n = 1; n < numChars; n++)  // и считываем остальные
            {
                ch = fgetc (fp);
                buffer[n]= ch;
            }
            if (compTemplates(template, buffer, numChars)) {    // При совпадении буфера и сигнатуры
                printf("zip file is detected - ");              // выводим сообщение
                i++;                                            // увеличиваем кол-во zip файлов
                position += 26;                                 // переходим на позицию длины имени файла
                fsetpos(fp, ppos);
                ch = fgetc (fp);                                // считываем длину
                length= ch;
                position += 4;                                  // переходи на позицию имени файла
                fsetpos(fp, ppos);
                //printf("Position - %llx \n", position);
                //printf ("%x \n", length);
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
        fsetpos(fp, ppos);
        //printf ("%x %x %x %x \n", buffer[0], buffer[1], buffer[2], buffer[3]);
    }
    //fgetpos(fp, ppos);
    //printf("Last Position - %llx \n", *ppos);
    //printf("i = %d \n", i);

    if (i == 0)
        printf("zip isn't detected \n");

    fclose(fp);

    return 0;
}
