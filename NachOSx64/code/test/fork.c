#include "syscall.h"

void rutina(int);
int id;

int main() {
   Fork( rutina );
   Write( "basura", 6, 1 );
}


void rutina( int param ) {
   Write( "hola, nuevo fork2\n", 19, 1 );
}