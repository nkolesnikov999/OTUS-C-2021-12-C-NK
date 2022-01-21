#include <stdio.h>
#include <string.h>

/*
*    Символы в таблицах расположены в следующем порядке
*    АБВГ ДЕЖЗ ИЙКЛ МНОП
*    РСТУ ФХЦЧ ШЩЪЫ ЬЭЮЯ
*    абвг дежз ийкл мноп
*    рсту фхцч шщъы ьэюя
*    Для utf8 при записи для 1-3 строк в начало нужно поставить D0,
*    для 4 строки - D1.
*/

int utf8[64] = { 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
               0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
               0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
               0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F };

int koi8[64] = { 0xE1, 0xE2, 0xF7, 0xE7, 0xE4, 0xE5, 0xF6, 0xFA, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0,
               0xF2, 0xF3, 0xF4, 0xF5, 0xE6, 0xE8, 0xE3, 0xFE, 0xFB, 0xFD, 0xFF, 0xF9, 0xF8, 0xFC, 0xE0, 0xF1,
               0xC1, 0xC2, 0xD7, 0xC7, 0xC4, 0xC5, 0xD6, 0xDA, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
               0xD2, 0xD3, 0xD4, 0xD5, 0xC6, 0xC8, 0xC3, 0xDE, 0xDB, 0xDD, 0xDF, 0xD9, 0xD8, 0xDC, 0xC0, 0xD1 };

int cp1251[64] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
                 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
                 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };

int iso8859[64] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
                  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
                  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF };



int main(int argc, char* argv[])
{
    char* message = "Mode must be koi8, or cp1251, or iso8859. \n";

    // Получаем данные файлов из командной строки
    char* infile;
    char* str_coding;
    char* outfile;

    if (argc == 4) {        // Проверяем кол-во аргументов
        infile = argv[1];
        str_coding = argv[2];
        outfile = argv[3];
        //printf("argvs - %s %s %s\n", infile, str_coding, outfile);
    } else {
        //При несовпадении выходим с сообщением
        printf("Please, enter correct  - %s infile.txt mode outfile.txt \n %s", argv[0], message);
        return 0;
    }

    // Проверяем требуемую кодировку и устанавливает соответствующую таблицу
    int *cod_table;

    if (strcmp(str_coding, "koi8") == 0) {
        cod_table = koi8;
    } else if (strcmp(str_coding, "cp1251") == 0) {
        cod_table = cp1251;
    } else if (strcmp(str_coding, "iso8859") == 0) {
        cod_table = iso8859;
    } else {
        // Если кодировка задана неверно, выходим с сообщением
        printf ("You entered mode - %s. \n%s", str_coding, message);
        return 0;
    }

    // Открываем файлы и получаем указатели
    FILE *in_ptr_file;
    FILE *out_ptr_file;

    if((in_ptr_file = fopen(infile, "r"))==NULL) {
        //При невозможности открыть выходим с сообщением
        printf ("Cannot open file: %s\n", infile);
        return 0;
    }

    if((out_ptr_file = fopen(outfile, "w"))==NULL) {
        //При невозможности открыть выходим с сообщением
        printf ("Cannot open file: %s\n", outfile);
        return 0;
    }

    int ch = 0;        // символ
    while(!ferror(in_ptr_file) && ch != EOF) {
        ch = fgetc(in_ptr_file);            // Получаем символ из входного файла
        if (ch < 0x80) {                    // Если меньше 128, то записываем его без изменений
            fputc( ch, out_ptr_file );
        } else {
            int i;
            for (i = 0; i < 64; i++) {
                if (ch == cod_table[i]) {   // Иначе ищем символ в выбранной таблице
                    if (i < 48)             // Для 1-3 строк таблицы добавляем D0
                    {
                        fputc( 0xD0, out_ptr_file );
                    } else {                // Для 4 строки - D1
                        fputc( 0xD1, out_ptr_file );
                    }
                    fputc( utf8[i], out_ptr_file ); // Записываем символ в файл
                    break;
                }
            }
            if (i==64)
                // Если символ не найден, выводим сообщение
                printf("Cannot find character - %x \n", ch);
        }
    }

    // Сообщаем об окончании
    printf("File %s is created! \n", outfile);

    fclose(in_ptr_file);
    fclose (out_ptr_file);

    return 0;
}