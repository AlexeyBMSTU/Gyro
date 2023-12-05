#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

// Алшгоритм вычисления магнитного курса:
float funcMagnetic(double roll, double yaw, double pitch, int number, float magn, float gyro){                 
	float sr = sin (roll),
		cr = cos (roll),
		sp = sin (pitch),
		cp = cos (pitch),
		sc = sin (yaw),
		cc = cos (yaw), Bb[3], Bx, Bz, MagneticWell, gyroEquation,
		ms[3] = {1,
			   	 1,     
				 1},                                                                                          //Задержка по крену, рысканию и тангажу вектор-столбец
		K[9],
		MatrixDirectionCos[9];

			//Из связанной в земную СК 	
//	K[0] = cp*cc;         K[3] = -cr*sp*cc+sr*sc;          K[6] = sr*sp*cc+cr*sc;
//	K[1] = sp;            K[4] = cp*cr;                    K[7] = -cp*sr;
//	K[2] = -cp*sc;        K[5] = cr*sp*sc+sr*cc;           K[8] = -sr*sp*sc+cr*cc;

//          Из земной в связанную СК для использования данные измерительной аппаратуры получаемых в связанной СК
	K[0] = cp*cc;                      K[3] = sp;              K[6] = -cp*sc;
	K[1] = -cr*sp*cc+sr*sc;            K[4] = cp*cr;           K[7] = cr*sp*sc+sr*cc;
	K[2] = sr*sp*cc+cr*sc;             K[5] = -cp*sr;          K[8] = -sr*sp*sc+cr*cc;
	//Вычисляем 
	
	Bb[1] = K[0]*ms[1] + K[3]*ms[2] + K[6]*ms[3];                                                             // Необходима матрица из земной в связанную
	Bb[2] = K[1]*ms[1] + K[4]*ms[2] + K[7]*ms[3];                                                             //векторное произведение транспонированной верхей матрицы и вектора-столбца ms
	Bb[3] = K[2]*ms[1] + K[5]*ms[2] + K[8]*ms[3];
	
	//Составляем матрицу направляющих косинусов в NHE  из связанной в инерциальную систему координат
	MatrixDirectionCos[0] = cp;      MatrixDirectionCos[3] = -cr*sp;     MatrixDirectionCos[6] = sr*sp;       //матрицу выше взять и обнулить рыскание
	MatrixDirectionCos[1] = sp;      MatrixDirectionCos[4] = cp*cr;      MatrixDirectionCos[7] = -cp*sr;      //страница 25 yaw = 0  sc=0, cc=1. Оси совпадают вертиклальные
	MatrixDirectionCos[2] = 0;       MatrixDirectionCos[5] = sr;         MatrixDirectionCos[8] = cr;
	
	
	
	Bx = MatrixDirectionCos[0]*Bb[1] + MatrixDirectionCos[3]*Bb[2] + MatrixDirectionCos[6]*Bb[3];             //векторное умножение матриц перехода в обратном порядке:
	Bz = MatrixDirectionCos[2]*Bb[1] + MatrixDirectionCos[5]*Bb[2] + MatrixDirectionCos[8]*Bb[3];             //(из связанной в инерцальную) * (из земной в связанную)
	
	MagneticWell = 269.81 + atan(-Bz/ Bx)*180/3.1415926535;
//	MagneticWell = 80.9 + atan(-Bz/ Bx)*180/3.1415926535;
	gyroEquation = (number*MagneticWell + magn)/(number+1);
	
//	printf("Magnetic(inert) well:  %f degree\n GyroMagnetic: %f \n", MagneticWell, gyroEquation );
	
	//Вычисление истинного курса
	//	float TrueWell, deviation = 2;
		
//	TrueWell = MagneticWell - deviation;
	
//	printf("True well: %f radian\n", TrueWell);

return MagneticWell;

}

float FuncGyroEquation (float MagneticWell,float t, int number, float magn){
	float gyroEquation;
	//gyroEquation = (number*MagneticWell + magn)/(number+1);
	gyroEquation = (MagneticWell + magn*t/900)/(1+t/900);
	return gyroEquation;
}

int open_file(float MagneticWell, float gyroEquation, int operation, int number, float t, float magn, float gyro){
	FILE *fileForWrite;
//printf("GYRO(equation): %f \n INERT(equation): %f \n", gyroEquation, MagneticWell);
fileForWrite = fopen("output_inert.txt", "a");                                                 //открыть файл с оператором "а" для добавления поверх содержимого новые данные
	if (fileForWrite ) {
		   fprintf(fileForWrite, "%f %f %f %f %f\n", MagneticWell, magn, gyro, gyroEquation, t);     //Инерционный курс, магнитный курс(СОК), гиромагнитный курс(СОК), гиромагнитный курс(РАССЧИТАННЫЙ), время
			printf("File open %d time!\n", operation-1);

	}
	
	else {
		printf("Error open file in %d time\n", operation-1);
	}
	fclose(fileForWrite);
	return 0;
}



int main() {
	
	FILE *fileForRead, *fileForWrite1;
	
	fileForRead = fopen("input.txt", "r");                                                      //открытие изначального файла
	
	if (fileForRead) {
		printf("File open!\n");
	} 
	
	else {
		printf("Error open file");
	}

		int i;
		
		float t, Hz;
		
		float *omegaX, *omegaY, *omegaZ;                                                        //указатель на массив скорости
		float *accelerationX, *accelerationY, *accelerationZ;                                   //указатель на массив ускорения
		float *alphaX, *alphaY, *alphaZ;                                                        //указатель на массив углов
	    float *magn, *gyro;                

	
	int number;
	
	char s[5090],*s1="start " ,s2[5000];
  
 
  printf("write to road exe file(matlab for plot): ");
  gets(s2);
  
  strcpy(s,s1);   
  
  strcat(s,s2);

	printf("Write to qrequency(Hz>0): ");
	scanf("%f", &Hz);
	t = 1/Hz;
	
	printf("\nWrite kolichestvo tochek: ");                                                     // количество точек абсолютных значений (векторная сумма угловых скоростей)
	scanf("%d", &number);
	
	
	
if (number >= 1 && t > 0){                                                                      //проверка на правильный ввод

	omegaX = (float*)malloc((number+1)*sizeof(int));                                            //выделение памяти под скорости
	omegaY = (float*)malloc((number+1)*sizeof(int));
	omegaZ = (float*)malloc((number+1)*sizeof(int));
	
	accelerationX = (float*)malloc((number+1)*sizeof(int));                                     //выделение памяти под ускорения
	accelerationY = (float*)malloc((number+1)*sizeof(int));
	accelerationZ = (float*)malloc((number+1)*sizeof(int));
	
	alphaX = (float*)malloc((number+1)*sizeof(int));                                            //выделение памяти под углы
	alphaY = (float*)malloc((number+1)*sizeof(int)); 
	alphaZ = (float*)malloc((number+1)*sizeof(int)); 
	
	magn = 	 (float*)malloc((number+1)*sizeof(int)); 
	gyro =   (float*)malloc((number+1)*sizeof(int)); 

		
	//Считывание из файла формата CSV->txt
	char x1,x2,x3,x4,x5,x6,x7;                                                                        //присваевание переменным знак ; при считывании txt документа, конвентированного из CSV файла
	
	while ( i<=number){                                                                         //Открытие файла и считывание до заданного значения. Файл имеет вид: ax,ay,az, wx,wy,wz
		fscanf(fileForRead, "%f%c%f%c%f%c%f%c%f%c%f%c%f%c%f", &(accelerationX[i]), &x1, &(accelerationY[i]), &x2, &(accelerationZ[i]), &x3, &(omegaX[i]), &x4, &(omegaY[i]), &x5, &(omegaZ[i]), &x6, &(magn[i]), &x7, &(gyro[i]));
//		fscanf(fileForRead, "%f%f%f%f%f%f", &(accelerationX[i]), &(accelerationY[i]), &(accelerationZ[i]), &(omegaX[i]), &(omegaY[i]), &(omegaZ[i]));

		printf ("ax[m/s*s] = %f; ay[m/s*s] = %f; az[m/s*s] %f \n wx[1/s] = %f; wy[1/s] = %f; wz[1/s] = %f; \n",accelerationX[i], accelerationY[i], accelerationZ[i], omegaX[i], omegaY[i],omegaZ[i] );
		i++;
	}
	i = 1;    
	
	//Интеграл от угловой скорости
	while (i<=number){
		alphaX[i] = omegaX[i]* t*3.1415926535/180;
		alphaY[i] = omegaY[i]* t*3.1415926535/180;
		alphaZ[i] = omegaZ[i]* t*3.1415926535/180;
		
		i++;
	}
	

	fclose(fileForRead);
	
	//Вызываем функцию вычисления магнитного курса
	
	int operation = 1;                                                                           //число вызова функции открытие файла для MATLAB
	i=0;
	t = 0;                                                                                       //i=0, t=0 для начала графика магнитного курса из нулей координат
	
		fileForWrite1 = fopen("output_inert.txt", "w");                                           //создать и отчистить прошлое содержимое файла
		
		//fprintf(fileForWrite1, "InertCourse MagneticCourse GyroMagnetic(practic) GyroMagnetic(equation) t \n");
		
		fclose(fileForWrite1);
	while (i<=number){   //Вызов всех функций до заданного значения выше(number) и расчет по каждой скорости магнитный курс, запись в новый txt файл для построения графика в матлабе
		open_file(funcMagnetic(alphaX[i], alphaY[i], alphaZ[i],  number, magn[i], gyro[i]), FuncGyroEquation(funcMagnetic(alphaX[i], alphaY[i], alphaZ[i],  number, magn[i], gyro[i]), t, number, magn[i]), operation, number, t, magn[i], gyro[i]);

		t=t+1/Hz;
		i++;
		operation++;
			
	}


}
else {
	printf("Error number! Value point only >=1 and time > 0");
}

system(s);
		if (system(s)!=NULL){
				printf("error");
			}
		else {
				printf("Application has open!");
			}
			
return 0;
}

