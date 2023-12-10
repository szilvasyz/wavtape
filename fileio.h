int fileio_init(char * recdir);
int fileio_getlabel(char * l);
int fileio_first(char * d, char * m);
int fileio_next(char * m);
int fileio_prev(char * m);
char * fileio_filename();
char * fileio_path();
int fileio_isdir();
int fileio_dirpos();
int fileio_fopenr(char * name);
int fileio_fopenw(char * name);
int fileio_fclose();
int fileio_fread(uint8_t * b, int l);
int fileio_fwrite(uint8_t * b, int l);
void fileio_frewind();
int fileio_ftrunc(long l);
int fileio_fseek(long l);
int fileio_feof();
int fileio_ferror();
int fileio_dopen(char * name);
int fileio_fexists(char * name);
