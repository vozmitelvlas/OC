#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
int main()
{
 int fd;
 struct input_event ev;
 fd = open("/dev/input/event2", O_RDONLY);
 if (fd == -1)
 {
 perror("Ошибка при открытии устройства клавиатуры");
 exit(EXIT_FAILURE);
 }
 // Бесконечный цикл чтения событий клавиатуры
 for (;;)
 {
 // Чтение очередного события
 read(fd, &ev, sizeof(struct input_event));
 if (ev.type == EV_KEY && ev.value == 1)
 {
 // Обработка события нажатия клавиши
 switch (ev.code)
 {
 case KEY_A:
 printf(" \nклавиша 'A'\n");
 break;
 case KEY_B:
 printf(" \nклавиша 'B' \n");
 break;
 default:
 printf(" \nдругая клавиша\n");
 break;
 }
 }
 }
 close(fd);
 return 0;
}
