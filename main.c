#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

// ��������� ���������� ���������� �����:
float funcMagnetic(double roll, double yaw, double pitch, int number){                 
	float sr = sin (roll),
		cr = cos (roll),
		sp = sin (pitch),
		cp = cos (pitch),
		sc = sin (yaw),
		cc = cos (yaw), Bb[3], Bx, Bz, MagneticWell,
		ms[3] = {1,
			   	 1,     
				 1},                                                                                          //�������� �� �����, �������� � ������� ������-�������
		K[9]= 		{                                                                                         /*������� �������� �������������� �� ������� � ��������� ��*/
					1,0,0,
					0,1,0,
					0,0,1
					},
		MatrixDirectionCos[9];

			//�� ��������� � ������ �� (�� ������� � ��������� �� - ��������������� ������ �������)		
	K[0] = cp*cc;         K[3] = -cr*sp*cc+sr*sc;          K[6] = sr*sp*cc+cr*sc;
	K[1] = sp;            K[4] = cp*cr;                    K[7] = -cp*sr;
	K[2] = -cp*sc;        K[5] = cr*sp*sc+sr*cc;           K[8] = -sr*sp*sc+cr*cc;
	
	//��������� 
	
	Bb[1] = K[0]*ms[1] + K[1]*ms[2] + K[2]*ms[3];
	Bb[2] = K[3]*ms[1] + K[4]*ms[2] + K[5]*ms[3];                                                             //��������� ������������ ����������������� ������ ������� � �������-������� ms
	Bb[3] = K[6]*ms[1] + K[7]*ms[2] + K[8]*ms[3];
	
	//���������� ������� ������������ ��������� � NHE  �� ��������� � ������������ ������� ���������
	MatrixDirectionCos[0] = cp;      MatrixDirectionCos[3] = -cr*sp;     MatrixDirectionCos[6] = sr*sp;       //������� ���� ����� � �������� ��������
	MatrixDirectionCos[1] = sp;      MatrixDirectionCos[4] = cp*cr;      MatrixDirectionCos[7] = -cp*sr;      //�������� 25 yaw = 0  sc=0, cc=1. ��� ��������� �������������
	MatrixDirectionCos[2] = 0;       MatrixDirectionCos[5] = sr;         MatrixDirectionCos[8] = cr;
	
	
	
	Bx = MatrixDirectionCos[0]*Bb[1] + MatrixDirectionCos[3]*Bb[2] + MatrixDirectionCos[6]*Bb[3];             //��������� ��������� ������ �������� � �������� �������:
	Bz = MatrixDirectionCos[2]*Bb[1] + MatrixDirectionCos[5]*Bb[2] + MatrixDirectionCos[8]*Bb[3];             //(�� ��������� � �����������) * (�� ������ � ���������(����������������))
	
	MagneticWell = 266.81 + atan(-Bz/ Bx)*180/3.1415926535;
	printf("Magnetic well:  %f degree\n", MagneticWell);
	
	//���������� ��������� �����
	//	float TrueWell, deviation = 2;
		
//	TrueWell = MagneticWell - deviation;
	
//	printf("True well: %f radian\n", TrueWell);
return MagneticWell;
}
int open_file(float MagneticWell , int operation, int number, float t){
	FILE *fileForWrite;
	
	fileForWrite = fopen("output_inert.txt", "w");                                           //������� � ��������� ������� ���������� �����
	fclose(fileForWrite);
	
	fileForWrite = fopen("output_inert.txt", "a");                                           //������� ���� � ���������� "�" ��� ���������� ������ ����������� ����� ������
	
	if (fileForWrite) {
		printf("File open %d time!\n", operation-1);
	}
	
	else {
		printf("Error open file in %d time\n", operation-1);
	}
	
	fprintf(fileForWrite, "%f %f\n", MagneticWell, t);	                                        //�������� � ���� ������ ���������� �����
return 0;
fclose(fileForWrite);
}


int main() {
	
	FILE *fileForRead;
	
	fileForRead = fopen("input_inert.txt", "r");                                                      //�������� ������������ �����
	
	if (fileForRead) {
		printf("File open!\n");
	} 
	
	else {
		printf("Error open file");
	}

		int i;
		
		float t, Hz;
		
		float *omegaX, *omegaY, *omegaZ;                                                        //��������� �� ������ ��������
		float *accelerationX, *accelerationY, *accelerationZ;                                   //��������� �� ������ ���������
		float *alphaX, *alphaY, *alphaZ;                                                        //��������� �� ������ �����
	                    

	
	int number;
		
	printf("Write to qrequency(Hz>0): ");
	scanf("%f", &Hz);
	t = 1/Hz;
	
	printf("\nWrite kolichestvo tochek: ");                                                     // ���������� ����� ���������� �������� (��������� ����� ������� ���������)
	scanf("%d", &number);
	
	
	
if (number >= 1 && t > 0){                                                                      //�������� �� ���������� ����

	omegaX = (float*)malloc((number+1)*sizeof(int));                                            //��������� ������ ��� ��������
	omegaY = (float*)malloc((number+1)*sizeof(int));
	omegaZ = (float*)malloc((number+1)*sizeof(int));
	
	accelerationX = (float*)malloc((number+1)*sizeof(int));                                     //��������� ������ ��� ���������
	accelerationY = (float*)malloc((number+1)*sizeof(int));
	accelerationZ = (float*)malloc((number+1)*sizeof(int));
	
	alphaX = (float*)malloc((number+1)*sizeof(int));                                            //��������� ������ ��� ����
	alphaY = (float*)malloc((number+1)*sizeof(int)); 
	alphaZ = (float*)malloc((number+1)*sizeof(int)); 
		
		
	//���������� �� ����� ������� CSV->txt
	char x1,x2,x3,x4,x5;                                                                        //������������ ���������� ���� ; ��� ���������� txt ���������, ����������������� �� CSV �����
	
	while ( i<=number){                                                                         //�������� ����� � ���������� �� ��������� ��������. ���� ����� ���: ax,ay,az, wx,wy,wz
		fscanf(fileForRead, "%f%c%f%c%f%c%f%c%f%c%f", &(accelerationX[i]), &x1, &(accelerationY[i]), &x2, &(accelerationZ[i]), &x3, &(omegaX[i]), &x4, &(omegaY[i]), &x5, &(omegaZ[i]));
		printf ("ax[m/s*s] = %f; ay[m/s*s] = %f; az[m/s*s] %f \n wx[1/s] = %f; wy[1/s] = %f; wz[1/s] = %f; \n",accelerationX[i], accelerationY[i], accelerationZ[i], omegaX[i], omegaY[i],omegaZ[i] );
		i++;
	}
	i = 1;    
	
	//�������� �� ������� ��������
	while (i<=number){
		alphaX[i] = omegaX[i]* t*3.1415926535/180;
		alphaY[i] = omegaY[i]* t*3.1415926535/180;
		alphaZ[i] = omegaZ[i]* t*3.1415926535/180;
		i++;
	}
	

	fclose(fileForRead);
	
	//�������� ������� ���������� ���������� �����
	
	int operation = 1;                                                                           //����� ������ ������� �������� ����� ��� MATLAB
	i=0;
	t = 0;                                                                                       //i=0, t=0 ��� ������ ������� ���������� ����� �� ����� ���������
	
	while (i<=number){   //����� ���� ������� �� ��������� �������� ����(number) � ������ �� ������ �������� ��������� ����, ������ � ����� txt ���� ��� ���������� ������� � �������
		open_file(funcMagnetic(alphaX[i], alphaY[i], alphaZ[i], number), operation, number, t);
		t=t+1/Hz;
		i++;
		operation++;	
	}

}

else {
	printf("Error number! Value point only >=1 and time > 0");
}

}
