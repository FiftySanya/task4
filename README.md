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
   ```bash
    malloc(0) = 0x368216808008
   ```

### Висновки
- Повернений вказівник можна безпечно передати у free(), навіть якщо він вказує на блок розміром 0.

## Завдання 4: Виправлення помилок у циклічному виділенні пам'яті

### Опис
Ідентифікація та виправлення помилки у коді, який виділяє пам'ять у циклі.

Критична помилка полягає в тому, що пам'ять звільняється (free(ptr)) на кожній ітерації циклу, але ptr не встановлюється в NULL після звільнення.
- ```c
  void *ptr = NULL;
  while (<some-condition-is-true>) {
      if (!ptr)
          ptr = malloc(n);
      [... <використання 'ptr'> ...]
      free(ptr);
  }
  ```

### Наслідки помилки
- На першій ітерації: ptr = NULL, виділяється пам'ять
- На наступних ітераціях: ptr не NULL (але вже звільнений), тому не виділяється НОВА пам'ять
- Програма використовує вже звільнену пам'ять

### Правильне рішення
- Після виклику free(ptr) необхідно встановити ptr = NULL, щоб забезпечити виділення нової пам'яті на наступній ітерації.

### Висновки
- Важливо завжди присвоювати NULL звільненим вказівникам, якщо вони можуть бути використані пізніше.

## Завдання 5: Поведінка при неможливості виділення пам'яті realloc

### Опис
Дослідження поведінки realloc() при неможливості виділити пам'ять для нового розміру.

### Поведінка realloc
- Коли realloc() не може виділити пам'ять, він повертає NULL.
- Оригінальний блок пам'яті залишається недоторканим і дійсним.
- Дані в оригінальному блоці зберігаються.

### Виконання програми
- ```bash
  ./task5
   Початкові дані: Дані
   realloc() не зміг виділити пам'ять
   Оригінальні дані збережені: Дані
  ```
  
### Висновки
- realloc() забезпечує безпечне перевиділення пам'яті - при невдачі оригінальні дані не втрачаються, якщо не перезаписувати оригінальний вказівник.

## Завдання 6: Випадки realloc з NULL або розміром 0

### Опис
Дослідження спеціальних випадків використання realloc: з NULL-вказівником або розміром 0.

### Результати виконання
- ```bash
  ./task6
   realloc(NULL, 10) повернув: 0x3510c6009000
   realloc(ptr, 0) повернув: 0x3510c6008008
  ```
  
- realloc(NULL, size)**:
  - Еквівалентний malloc(size)
  - Створює новий блок пам'яті розміром size
  - Повертає вказівник на виділений блок

- realloc(ptr, 0)**:
  - Еквівалентний free(ptr)
  - Звільняє пам'ять, на яку вказує ptr
  - повертає вказівник (залежно від реалізації)

### Висновки
- Ці спеціальні випадки realloc() роблять його гнучким інструментом для керування пам'яттю.
- Важливо правильно інтерпретувати повернене значення в кожному випадку.

## Завдання 7: Використання reallocarray

### Опис
Дослідження переваг використання reallocarray() порівняно з realloc().

### Різниця між викликами функцій в ltrace
- ```bash
  // Використання realloc
  realloc(0x3df0bd009580, 52000) = 0x3df0bd009580
  
  // Використання reallocarray
  reallocarray(0x3df0bd009bc0, 500, 104, 0x923f26424516e575, 0x821f2f618) = 0x3df0bd009bc0
  ```

### Переваги reallocarray
- Функція reallocarray() внутрішньо перевіряє можливість переповнення при множенні nmemb * size.
- Запобігає виділенню некоректного обсягу пам'яті через переповнення арифметики.

### Висновки
- reallocarray() рекомендується використовувати замість комбінації realloc() і ручного множення для розміру масиву.

## Завдання 8: Порівняння malloc у різних бібліотеках

### Опис
Дослідження різниці між реалізаціями malloc у glibc, musl та jemalloc.

### glibc malloc (GNU libc - Linux)
- Використовує алгоритм "ptmalloc2" (pthread malloc)
- Підтримує багатопотоковість з аренами пам'яті
- Складні алгоритми для оптимізації швидкості та мінімізації фрагментації
- Може споживати більше пам'яті через додаткові метадані

### musl malloc (musl libc - альтернативна реалізація для Linux)
- Простіша реалізація, оптимізована для розміру
- Менше споживання пам'яті
- Хороша продуктивність для вбудованих систем
- Менше функціональних можливостей

### jemalloc (FreeBSD 14.2 malloc)
- ```bash
   ./task8
   Час виконання: 0.005029 секунд
  ```
- FreeBSD використовує jemalloc як стандартний алокатор
- Дуже ефективний для багатопотокових програм
- Менша фрагментація пам'яті
- Дуже хороша масштабованість

### Практичні відмінності
- glibc malloc: оптимізований для загального використання, краще працює у випадках з великою кількістю великих виділень
- musl malloc: оптимізований для розміру та ефективності, краще для вбудованих систем
- jemalloc: оптимізований для багатопотокових додатків, має кращу масштабованість

### Висновки
- Вибір бібліотеки може суттєво впливати на продуктивність програми, особливо для додатків, які інтенсивно використовують динамічну пам'ять.
- jemalloc (FreeBSD) забезпечує переваги для високонавантажених серверних програм із багатьма потоками.
