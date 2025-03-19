# Динамічний розподіл пам'яті

## Зміст
- [Завдання 1: Максимальний розмір виділення пам'яті malloc](#завдання-1-максимальний-розмір-виділення-памяті-malloc)
- [Завдання 2: Передача від'ємного аргументу в malloc](#завдання-2-передача-відємного-аргументу-в-malloc)
- [Завдання 3: Поведінка malloc(0)](#завдання-3-поведінка-malloc0)
- [Завдання 4: Виправлення помилок у циклічному виділенні пам'яті](#завдання-4-виправлення-помилок-у-циклічному-виділенні-памяті)
- [Завдання 5: Поведінка при неможливості виділення пам'яті realloc](#завдання-5-поведінка-при-неможливості-виділення-памяті-realloc)
- [Завдання 6: Випадки realloc з NULL або розміром 0](#завдання-6-випадки-realloc-з-null-або-розміром-0)
- [Завдання 7: Використання reallocarray](#завдання-7-використання-reallocarray)
- [Завдання 8: Порівняння malloc у різних бібліотеках](#завдання-8-порівняння-malloc-у-різних-бібліотеках)

---

## Завдання 1: Максимальний розмір виділення пам'яті malloc

### Опис
Дослідження теоретичного та практичного максимального обсягу пам'яті, який може бути виділений за один виклик malloc(3).

### Результати
- Теоретично максимальний розмір, який можна передати malloc() - це максимальне значення size_t.
- На 64-бітній системі size_t зазвичай 8 байтів (64 біти), тому максимальне значення становить 2^64-1 (приблизно 16 ексабайтів).
- Однак фактичний максимум складає близько 8 ексабайтів (2^63-1), а не 16 ексабайтів.

### Пояснення обмеження
У Unix системах віртуальний адресний простір процесу поділений на дві частини:
 - Верхня половина (вище адреси 2^63) зарезервована для ядра (kernel space)
 - Нижня половина (нижче 2^63) доступна для користувацьких процесів (user space)
Через таке розділення адресного простору, користувацький процес може адресувати лише до 2^63-1 байтів (8 ексабайтів).

### Відмінності між платформами
- На x86_64 (64-біт): максимальний теоретичний розмір ~16 ексабайтів, практичний ~8 ексабайтів
- На x86 (32-біт): максимальний теоретичний розмір 4 ГБ, практичний ще менше через поділ адресного простору

## Завдання 2: Передача від'ємного аргументу в malloc

### Опис
Дослідження поведінки malloc при передачі від'ємного аргументу та проблеми переповнення знакових типів.

### Що станеться, якщо передати malloc() від’ємний аргумент?
- Коли в malloc() передається від'ємне число, воно інтерпретується як дуже велике невід'ємне число, оскільки size_t - беззнаковий тип.
- Це відбувається через особливості перетворення знакових типів у беззнакові.

### Виконання програми при переповненні int
- ```bash
  ./task2
   malloc не зміг виділити -727379968 байтів
  ```

### Поведінка при переповненні
- xa * xb = 1000000000000 (1 трильйон), що значно перевищує максимальне значення для 32-бітного int (2147483647).
- При переповненні для цілих чисел зі знаком у C поведінка формально є невизначеною, але на практиці на більшості систем результатом буде від’ємне число. На 32-бітній і 64-бітній системі це може бути приблизно -727379968.
- malloc() повертає NULL, оскільки система не може виділити такий великий обсяг пам'яті.

## Завдання 3: Поведінка malloc(0)

### Опис
Дослідження поведінки malloc(0) та її залежність від реалізації.

### Результати
- Стандарт C визначає, що malloc(0) може повернути або NULL, або вказівник, який можна передати в free().
- У FreeBSD 14.2 malloc(0) повертає дійсний вказівник.
- При запуску програми з ltrace спостерігається виклик malloc(0), який повертає вказівник, а не NULL.
-  ```bash
    malloc(0) = 0x368216808008
   ```

### Висновки
- Повернений вказівник можна безпечно передати у free(), навіть якщо він вказує на блок розміром 0.

## Завдання 4: Виправлення помилок у циклічному виділенні пам'яті

### Опис
Ідентифікація та виправлення помилки у коді, який виділяє пам'ять у циклі.

### Виявлена помилка
У наведеному коді:
```c
void *ptr = NULL;
while (<some-condition-is-true>) {
    if (!ptr)
        ptr = malloc(n);
    [... <використання 'ptr'> ...]
    free(ptr);
}
```

Критична помилка полягає в тому, що пам'ять звільняється (free(ptr)) на кожній ітерації циклу, але ptr не встановлюється в NULL після звільнення.

### Наслідки помилки
1. На першій ітерації: ptr = NULL, виділяється пам'ять
2. На наступних ітераціях: ptr не NULL (але вже звільнений), тому не виділяється нова пам'ять
3. Програма використовує і звільняє вже звільнену пам'ять (use-after-free)

### Правильне рішення
- Після виклику free(ptr) необхідно встановити ptr = NULL, щоб забезпечити виділення нової пам'яті на наступній ітерації.

### Висновки
- Помилка типу "use-after-free" є небезпечною, оскільки може призвести до непередбачуваної поведінки програми.
- Важливо завжди присвоювати NULL звільненим вказівникам, якщо вони можуть бути використані пізніше.

## Завдання 5: Поведінка при неможливості виділення пам'яті realloc

### Опис
Дослідження поведінки realloc() при неможливості виділити пам'ять для нового розміру.

### Поведінка realloc
- Коли realloc() не може виділити пам'ять, він повертає NULL.
- Оригінальний блок пам'яті залишається недоторканим і дійсним.
- Дані в оригінальному блоці зберігаються.

### Важливі аспекти
- Необхідно завжди перевіряти повернене значення realloc().
- Не можна безпосередньо перезаписувати оригінальний вказівник, бо в разі невдачі виникне витік пам'яті.

### Правильний підхід
```
char* new_buffer = (char*)realloc(buffer, huge_size);
if (new_buffer == NULL) {
    // realloc не вдався, але buffer все ще дійсний
    // використовуємо оригінальний buffer або звільняємо його
} else {
    // realloc успішний, використовуємо new_buffer
    buffer = new_buffer;
}
```

### Висновки
- realloc() забезпечує безпечне перевиділення пам'яті - при невдачі оригінальні дані не втрачаються.
- Правильна обробка помилок перевиділення пам'яті запобігає витокам пам'яті та втраті даних.

## Завдання 6: Випадки realloc з NULL або розміром 0

### Опис
Дослідження спеціальних випадків використання realloc: з NULL-вказівником або розміром 0.

### Результати досліджень
1. **realloc(NULL, size)**:
   - Еквівалентний malloc(size)
   - Створює новий блок пам'яті розміром size
   - Повертає вказівник на виділений блок або NULL при помилці

2. **realloc(ptr, 0)**:
   - Еквівалентний free(ptr)
   - Звільняє пам'ять, на яку вказує ptr
   - Може повернути NULL або невеликий вказівник (залежно від реалізації)

### Практичні наслідки
- realloc можна використовувати як універсальну функцію для виділення, перевиділення та звільнення пам'яті.
- При отриманні вказівника від realloc(ptr, 0), якщо він не NULL, його також потрібно звільнити.

### Висновки
- Ці спеціальні випадки realloc() роблять його гнучким інструментом для керування пам'яттю.
- Важливо правильно інтерпретувати повернене значення в кожному випадку.

## Завдання 7: Використання reallocarray

### Опис
Дослідження переваг використання reallocarray() порівняно з realloc() при роботі з масивами.

### Різниця між функціями
```c
// Використання realloc
newptr = realloc(ptr, 500 * sizeof(struct sbar));

// Використання reallocarray
newptr2 = reallocarray(ptr2, 500, sizeof(struct sbar));
```

### Переваги reallocarray
- Функція reallocarray() внутрішньо перевіряє можливість переповнення при множенні nmemb * size.
- Запобігає виділенню некоректного обсягу пам'яті через переповнення арифметики.
- Підвищує безпеку коду, особливо при роботі з великими обсягами даних.

### Аналіз через ltrace
- При використанні ltrace видно, що для realloc викликається: `realloc(ptr, 500*sizeof(struct sbar))`
- Для reallocarray: `reallocarray(ptr2, 500, sizeof(struct sbar))`
- Функціонально вони виконують однакову роботу, але reallocarray додатково перевіряє переповнення.

### Висновки
- reallocarray() рекомендується використовувати замість комбінації realloc() і ручного множення для розміру масиву.
- Це особливо важливо при роботі з великими масивами або складними структурами даних.

## Завдання 8: Порівняння malloc у різних бібліотеках

### Опис
Дослідження різниці між реалізаціями malloc у glibc, musl та FreeBSD.

### glibc malloc (GNU libc - Linux)
1. Використовує алгоритм "ptmalloc2" (pthread malloc)
2. Підтримує багатопотоковість з аренами пам'яті
3. Складні алгоритми для оптимізації швидкості та мінімізації фрагментації
4. Може споживати більше пам'яті через додаткові метадані

### musl malloc (musl libc - альтернативна реалізація для Linux)
1. Простіша реалізація, оптимізована для розміру
2. Менше споживання пам'яті
3. Хороша продуктивність для вбудованих систем
4. Менше функціональних можливостей

### FreeBSD 14.2 malloc (jemalloc)
1. FreeBSD використовує jemalloc як стандартний алокатор
2. Дуже ефективний для багатопотокових програм
3. Менша фрагментація пам'яті
4. Дуже хороша масштабованість

### Практичні відмінності
- glibc malloc: оптимізований для загального використання, краще працює у випадках з великою кількістю великих виділень
- musl malloc: оптимізований для розміру та ефективності, краще для вбудованих систем
- jemalloc: оптимізований для багатопотокових додатків, має кращу масштабованість

### Висновки
- Вибір бібліотеки може суттєво впливати на продуктивність програми, особливо для додатків, які інтенсивно використовують динамічну пам'ять.
- Для критичних до продуктивності програм може мати сенс тестування з різними алокаторами пам'яті.
- jemalloc (FreeBSD) забезпечує переваги для високонавантажених серверних програм із багатьма потоками.
