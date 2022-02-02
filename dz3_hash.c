#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

//Сравнение ключей
#define CMP_EQ(a, b) (strcmp((a), (b)) == 0)

const float LOAD_FACTOR = 0.5f;
const size_t INITIAL_SIZE = 100;
const float MULTIPLIER = 2.0f;

//Тип для ключа и для значения
typedef char* K;
typedef int V;

//Пара <ключ, значение>
typedef struct {
    K key;
    V value;
} Entry;

size_t hashcode(const char* s){ //хеш-функция для строк
    size_t hash = 0;
    const int p = 26;
    size_t p_pow = 1;
    for(; *s; ++s) {
        hash += ((size_t)*s - 'a' + 1) * p_pow;
        p_pow *= p;
    }
    return hash;
}

typedef struct {
    Entry **data;        //массив корзин
    size_t size;        //количество элементов в карте
    size_t arr_size;    //размер массива корзин
    size_t limit;       //целочисленное значение количества элементов карты
    //при превышении которого происходит пересборка
    float loadFactor;   //процентное заполнение карты, при котором
    //происходит пересборка карты
    float multiplier;   //во столько раз увеличится размер карты при пересборке
} Hashmap;

Hashmap* rehashUp(Hashmap **_map_, Entry* e);

//функция создания хэш-таблицы
Hashmap* createHashmap(size_t limit, float loadFactor, float multiplier) {
    Hashmap *tmp = (Hashmap*)malloc(sizeof(Hashmap));
    tmp->arr_size = (limit >= INITIAL_SIZE) ? limit : INITIAL_SIZE;
    tmp->loadFactor = (loadFactor >= LOAD_FACTOR &&
                       loadFactor <= 1.0f) ? loadFactor : LOAD_FACTOR;
    tmp->limit = (int)(tmp->loadFactor * (float)tmp->arr_size);
    tmp->size = 0;
    tmp->multiplier = (multiplier >= MULTIPLIER) ? multiplier : MULTIPLIER;

    tmp->data = (Entry**)calloc(tmp->arr_size, sizeof(Entry*));
    return tmp;
}

//Функция добавление новой пары
void raw_put(Hashmap **_map_, Entry *e) {
    //Вспомогательный указатель, чтобы не писать *
    Hashmap *map = *_map_;
    //Находим хэш и положение в массиве
    unsigned long long hash = hashcode(e->key);
    size_t index = (hash % map->arr_size);

    if (map->size <= map->limit) {
        if (map->data[index] == NULL) {
            map->data[index] = e;
        }
        else {
            //Если занято, вставляем в первый свободный
            while (map->data[index]) {
                index++;
                if (index >= map->arr_size) {
                    index = 0;
                }
            }
            map->data[index] = e;
        }
    }
    else {
        //Если достигнут лимит занятости, пересоздаем таблицу
        *_map_ = rehashUp(_map_, e);
    }
    (*_map_)->size++;
}

void put(Hashmap **map, K key, V value) {
    Entry *e = (Entry*)malloc(sizeof(Entry));
    e->key = key;
    e->value = value;
    raw_put(map, e);
}

//функция пересоздания хэш-таблицы
Hashmap* rehashUp(Hashmap **_map_, Entry* e) {
    Hashmap *newMap = createHashmap((size_t)((float)(*_map_)->arr_size * (*_map_)->multiplier),
                                    (*_map_)->loadFactor,
                                    (*_map_)->multiplier);

    size_t size;
    Hashmap *map = (*_map_);
    size = (*_map_)->arr_size;
    for (size_t i = 0; i < size; i++) {
        //не создаём новых пар, вставляем прежние
        if (map->data[i]) {
            raw_put(&newMap, map->data[i]);
        }
    }
    free(map->data);
    free(*_map_);
    raw_put(&newMap, e);
    *_map_ = newMap;
    return newMap;
}

//Если находим слово, то увеличиваем значение. Иначе возвращается нулл
Entry* findAndIncrement(Hashmap *map, K key) {
    unsigned long long hash = hashcode(key);
    size_t index = (hash % map->arr_size);
    size_t startIndex = index;
    Entry *retVal = NULL;

    if (map->data[index] != NULL) {
        if (CMP_EQ(map->data[index]->key, key)) {
            retVal = map->data[index];
            map->data[index]->value++;
        } else {
            while (map->data[index] == NULL || !CMP_EQ(map->data[index]->key, key)) {
                index++;
                if (index >= map->arr_size) {
                    index = 0;
                }
                if (index == startIndex)
                    return NULL;
            }
            retVal = map->data[index];
            map->data[index]->value++;
        }
    }

    return retVal;
}

//Фунция перебора
void mapIterate(Hashmap *map, void(*f)(Entry*)) {
    size_t size;
    size = map->arr_size;
    for (size_t i = 0; i < size; i++) {
        if (map->data[i]) {
            //printf("i - %lu ", i);
            f(map->data[i]);
        }
    }
}

//Функция печати
void printEntry(Entry *e) {
    printf("%s - %d \n", e->key, e->value);
    /*  for internal usage
    static int maxValue = 0;
    if (e->value > maxValue) {
       maxValue = e->value;
       printf(" - MAX");
     }
     printf("\n");
    */
}

//Фунуция освобождения пары
void freeEntry(Entry **e) {
    free((*e)->key);
    free(*e);
}

//Фунуция уничтожения таблицы
void destroyHashmap(Hashmap **_map_) {
    Hashmap *map = *_map_;
    size_t size;

    size = map->arr_size;
    for (size_t i = 0; i < size; i++) {
        if (map->data[i]) {
            freeEntry(&(map->data[i]));
        }
    }

    free(map->data);
    free(*_map_);
    *_map_ = NULL;
}

//Функция создания копии найденного слова
char* initAndCopy(const char *str) {
    char *word = (char*)malloc(strlen(str) + 1);
    strcpy(word, str);
    return word;
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
    FILE *file;
    int ch;
    char temp[255];
    int j=0;
    file = fopen(nfile, "r");

    if(file == 0) {
        printf("Cant open file '%s\n'", nfile);
        return 0; }

    //Создаем хэш-таблицу
    Hashmap *map = createHashmap(2, 0.5f, 2.0f);

    while (!ferror(file)) {
        ch = fgetc(file);
        //Если буква, то в нижний регистр и в буфер
        if (isalpha(ch)) {
            temp[j] = (char)tolower(ch);
            temp[++j] = '\0';
        }
        //Иначе проверяем буфер и меняем таблицу
        else if (j != 0){
            //printf("%s ", temp);
            if (findAndIncrement(map, temp) == NULL) {
                put(&map, initAndCopy(temp), 1);
            }
            //size_t hash = hashcode(temp);
            //printf("hash = %lu \n", hash);
            temp[0] ='\0';
            j = 0;
        }
        if (ch == EOF)
            break;
    }
    fclose(file);

    //Проходим по таблице и печатаем
    mapIterate(map, printEntry);
    destroyHashmap(&map);

    return 0;
}

