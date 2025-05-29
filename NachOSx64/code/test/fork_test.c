#include "syscall.h"
// extracted from https://os.ecci.ucr.ac.cr/ci0123/Asignaciones/NachOS/asistenciaFork.html

void hijo(int);
int id;
int main(){
	id = SemCreate(0);
	Fork(hijo);

	SemWait(id);
	Write("padre", 5, 1);
	SemDestroy(id);
	Exit(0);
	return;
}


void hijo(int dummy){
	Write( "hijo", 4, 1 );
	SemSignal(id);
	Exit(0);
	return;
}