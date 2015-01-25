#include <fcntl.h>
#include <termios.h>

int fd; /* port file descriptor */
char port[20] = “/dev/ttyS0″; /* port to connect to */
speed_t baud = B9600; /* baud rate */

fd = open(port, O_RDWR); /* connect to port */

/* set the other settings (in this case, 9600 8N1) */
struct termios settings;
tcgetattr(fd, &settings);

cfsetospeed(&settings, baud); /* baud rate */
settings.c_cflag &= ~PARENB; /* no parity */
settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
settings.c_cflag &= ~CSIZE;
settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
settings.c_lflag = ICANON; /* canonical mode */
settings.c_oflag &= ~OPOST; /* raw output */

tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
tcflush(fd, TCOFLUSH);

int main(void){
	write(fd,"L\r",2);

	/* — code to use the port here — */

	close(fd); /* cleanup */

}