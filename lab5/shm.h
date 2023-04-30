#define DEF_KEY_FILE "key"
const int BUFFER_SIZE = 100;
const int BUF_SIZE = 5;
typedef struct{
	long type;
	char buf[100];
} Message;

static struct sembuf readEna[1] = {0,-1,0};
static struct sembuf writeEna[1] = {1,-1,0};
static struct sembuf setReadEna[1] = {0,1,0};
static struct sembuf setWriteEna[1] = {1,1,0};

static struct sembuf memLock[1] = {2, -1,0};
static struct sembuf memUnlock[1] = {2, 1,0};
static struct sembuf waitNotFull[1] = {0, -1, 0};
static struct sembuf waitNotEmpty[1] = {1, -1, 0};
static struct sembuf releaseEmpty[1] = {0, 1, 0};
static struct sembuf releaseFull[1] = {1, 1, 0};
static struct sembuf setFree[1] = {0, 5, 0};
