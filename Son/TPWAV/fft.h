#ifndef FFT_H
#define FFT_H

/* function prototypes */
void fft(int N, double (*x)[2], double (*X)[2]);
void fft_rec(int N, int offset, int delta,
			 double (*x)[2], double (*X)[2], double (*XX)[2]);
void ifft(int N, double (*x)[2], double (*X)[2]);

#endif // FFT_H
